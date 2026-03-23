"""Pure Python immdebug viewer using imgui_bundle.

Usage:
    python -m immdebug.viewer

Polls the temp directory for images sent by immdebug (C++ or Python client)
and displays them using immvision's inspector.
"""

import struct
import tempfile
import time
import threading
from pathlib import Path
from typing import Optional, Tuple

import numpy as np


# ImageDepth enum -> numpy dtype (must match ImmVision::ImageDepth in C++)
_DEPTH_TO_DTYPE = {
    0: np.dtype("uint8"),
    1: np.dtype("int8"),
    2: np.dtype("uint16"),
    3: np.dtype("int16"),
    4: np.dtype("int32"),
    5: np.dtype("float32"),
    6: np.dtype("float64"),
}


class _ImagePayload:
    def __init__(self) -> None:
        self.image: np.ndarray = np.zeros((0, 0), dtype=np.uint8)
        self.legend: str = ""
        self.zoom_center: Tuple[float, float] = (0.0, 0.0)
        self.zoom_ratio: float = -1.0
        self.zoom_key: str = ""
        self.color_adjustments_key: str = ""
        self.is_color_order_bgr: bool = True


def _read_value(data: bytes, offset: int, fmt: str) -> Tuple:
    size = struct.calcsize(fmt)
    values = struct.unpack_from(fmt, data, offset)
    return values, offset + size


def _read_string(data: bytes, offset: int) -> Tuple[str, int]:
    (length,), offset = _read_value(data, offset, "=Q")
    s = data[offset : offset + length].decode("utf-8")
    return s, offset + length


def _read_mat(data: bytes, offset: int) -> Tuple[np.ndarray, int]:
    # Match C++ ReadImageBuffer: width(int), height(int), channels(int), depth(int), step(size_t)
    (width, height, channels, depth_int, _step), offset = _read_value(data, offset, "=iiiiQ")

    dtype = _DEPTH_TO_DTYPE[depth_int]
    row_bytes = width * channels * dtype.itemsize
    data_size = row_bytes * height
    pixel_data = data[offset : offset + data_size]

    if channels == 1:
        image = np.frombuffer(pixel_data, dtype=dtype).reshape((height, width))
    else:
        image = np.frombuffer(pixel_data, dtype=dtype).reshape((height, width, channels))

    return image.copy(), offset + data_size


def _read_image_payload(data: bytes) -> _ImagePayload:
    payload = _ImagePayload()
    offset = 0
    payload.legend, offset = _read_string(data, offset)
    (cx, cy), offset = _read_value(data, offset, "=dd")
    payload.zoom_center = (cx, cy)
    (payload.zoom_ratio,), offset = _read_value(data, offset, "=d")
    payload.zoom_key, offset = _read_string(data, offset)
    payload.color_adjustments_key, offset = _read_string(data, offset)
    (payload.is_color_order_bgr,), offset = _read_value(data, offset, "=?")
    payload.image, offset = _read_mat(data, offset)
    return payload


def _immdebug_folder() -> Path:
    return Path(tempfile.gettempdir()) / "ImmDebug"


def _remove_old_images(max_age_seconds: float = 3600.0) -> None:
    folder = _immdebug_folder()
    if not folder.exists():
        return
    now = time.time()
    for entry in folder.rglob("*"):
        if not entry.is_file():
            continue
        try:
            if now - entry.stat().st_mtime > max_age_seconds:
                entry.unlink()
        except OSError:
            pass


def _read_one_payload() -> Optional[_ImagePayload]:
    from imgui_bundle import hello_imgui

    folder = _immdebug_folder()
    if not folder.exists():
        return None

    bindata_files = sorted(
        [f for f in folder.iterdir() if f.is_file() and f.suffix == ".bindata"],
        key=lambda f: f.stat().st_mtime,
    )
    if not bindata_files:
        return None

    oldest = bindata_files[0]
    try:
        data = oldest.read_bytes()
        payload = _read_image_payload(data)
        oldest.unlink()
        return payload
    except Exception as e:
        hello_imgui.log(hello_imgui.LogLevel.error, f"Error reading {oldest.name}: {e}")
        try:
            oldest.unlink()
        except OSError:
            pass
        return None


# --- Single instance app (port of single_instance_app.cpp) ---

class _SingleInstanceApp:
    def __init__(self, lock_name: str) -> None:
        self._lock_name = lock_name
        self._exit = threading.Event()
        self._ping_received = threading.Event()
        self._thread: Optional[threading.Thread] = None

    def _ping_filename(self) -> Path:
        return Path(tempfile.gettempdir()) / f"{self._lock_name}.ping"

    def run_single_instance(self) -> bool:
        ping_file = self._ping_filename()

        # Check for stale ping file
        if ping_file.is_file():
            print("Ooops : stale ping file!")
            ping_file.unlink(missing_ok=True)
            time.sleep(0.1)

        # Create ping file and wait for master to remove it
        ping_file.write_text("Lock")
        time.sleep(0.12)

        if not ping_file.is_file():
            print("Other instance already launched!")
            return False

        # We are the first instance
        print("First instance!")
        ping_file.unlink(missing_ok=True)

        self._thread = threading.Thread(target=self._ping_loop, daemon=True)
        self._thread.start()
        return True

    def was_pinged(self) -> bool:
        if self._ping_received.is_set():
            self._ping_received.clear()
            return True
        return False

    def _ping_loop(self) -> None:
        while not self._exit.is_set():
            ping_file = self._ping_filename()
            if ping_file.is_file():
                print("Answering ping!")
                self._ping_received.set()
                ping_file.unlink(missing_ok=True)
            self._exit.wait(timeout=0.06)

    def stop(self) -> None:
        self._exit.set()
        if self._thread:
            self._thread.join()


# --- Main viewer ---

def _focus_window() -> None:
    import glfw
    from imgui_bundle import glfw_utils
    glfw.focus_window(glfw_utils.glfw_window_hello_imgui())


def main() -> None:
    import glfw
    from imgui_bundle import imgui, hello_imgui, immvision

    single_instance = _SingleInstanceApp("immdebug_viewer")

    if not single_instance.run_single_instance():
        print("Exit...")
        return

    frame_count = 0

    def add_incoming_images() -> bool:
        _remove_old_images()
        found_new = False
        while True:
            payload = _read_one_payload()
            if payload is None:
                break
            hello_imgui.log(hello_imgui.LogLevel.info, f"Received image: {payload.legend}")
            # Convert BGR images to RGB so the viewer always works in RGB mode
            if payload.is_color_order_bgr and payload.image.ndim == 3 and payload.image.shape[2] in (3, 4):
                img = payload.image.copy()
                img[..., 0], img[..., 2] = payload.image[..., 2], payload.image[..., 0]
                payload.image = img
            immvision.use_rgb_color_order()
            immvision.inspector_add_image(
                payload.image,
                payload.legend,
                payload.zoom_key,
                payload.color_adjustments_key,
                (payload.zoom_center[0], payload.zoom_center[1]),
                payload.zoom_ratio,
            )
            found_new = True
        if found_new:
            _focus_window()
        return found_new

    def gui() -> None:
        nonlocal frame_count
        immvision.inspector_show()

        # Poll for new images every 10 frames
        if frame_count % 10 == 0:
            add_incoming_images()
        frame_count += 1

        if single_instance.was_pinged():
            _focus_window()
            hello_imgui.log(hello_imgui.LogLevel.warning, "Pong")

    params = hello_imgui.RunnerParams()
    params.app_window_params.window_geometry.full_screen_mode = (
        hello_imgui.FullScreenMode.full_monitor_work_area
    )
    params.app_window_params.restore_previous_geometry = True
    params.app_window_params.window_title = "ImmVision - ImmDebug Viewer"
    params.callbacks.show_gui = gui
    params.ini_folder_type = hello_imgui.IniFolderType.app_user_config_folder
    params.ini_filename = "immdebug_viewer.ini"

    glfw.init()
    try:
        hello_imgui.run(params)
    finally:
        single_instance.stop()


if __name__ == "__main__":
    main()
