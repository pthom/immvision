"""Pure Python client for immdebug: send images to the immdebug_viewer app.

Usage:
    import numpy as np
    from immdebug import immdebug

    image = np.random.randint(0, 255, (480, 640, 3), dtype=np.uint8)
    immdebug(image, "my image")

The immdebug_viewer must be running separately. It polls a temp directory
for incoming image payloads.
"""

import os
import struct
import tempfile
import time
import random
from pathlib import Path
from typing import Tuple

import numpy as np


# ImageDepth enum values (must match ImmVision::ImageDepth in C++)
_DEPTH_FROM_DTYPE = {
    np.dtype("uint8"): 0,
    np.dtype("int8"): 1,
    np.dtype("uint16"): 2,
    np.dtype("int16"): 3,
    np.dtype("int32"): 4,
    np.dtype("float32"): 5,
    np.dtype("float64"): 6,
}

# Random base ID, incremented per call (mirrors C++ static id)
_next_id = random.randint(100_000_000_000, 1_000_000_000_000)


def _immdebug_folder(while_writing: bool) -> Path:
    base = Path(tempfile.gettempdir()) / "ImmDebug"
    folder = base / "Writing" if while_writing else base
    folder.mkdir(parents=True, exist_ok=True)
    return folder


def _make_unique_filename(while_writing: bool) -> Path:
    global _next_id
    _next_id += 1
    return _immdebug_folder(while_writing) / f"{_next_id}.bindata"


def _remove_old_images(max_age_seconds: float = 3600.0) -> None:
    folder = _immdebug_folder(False)
    now = time.time()
    for entry in folder.rglob("*"):
        if not entry.is_file():
            continue
        try:
            if now - entry.stat().st_mtime > max_age_seconds:
                entry.unlink()
        except OSError:
            pass


def _write_string(buf: bytearray, s: str) -> None:
    encoded = s.encode("utf-8")
    buf += struct.pack("=Q", len(encoded))  # size_t (8 bytes on 64-bit)
    buf += encoded


def _write_mat(buf: bytearray, image: np.ndarray) -> None:
    # Match C++ WriteImageBuffer: width(int), height(int), channels(int), depth(int), step(size_t)
    image = np.ascontiguousarray(image)
    height, width = image.shape[0], image.shape[1]
    channels = 1 if image.ndim == 2 else image.shape[2]

    depth = _DEPTH_FROM_DTYPE.get(image.dtype)
    if depth is None:
        raise ValueError(f"Unsupported dtype: {image.dtype}. Supported: {list(_DEPTH_FROM_DTYPE.keys())}")

    step = width * channels * image.dtype.itemsize
    buf += struct.pack("=iiiiQ", width, height, channels, depth, step)
    buf += image.tobytes()


def _immdebug_impl(
    image: np.ndarray,
    legend: str,
    zoom_center: Tuple[float, float],
    zoom_ratio: float,
    zoom_key: str,
    color_adjustments_key: str,
    is_color_order_bgr: bool,
) -> None:
    _remove_old_images()

    buf = bytearray()
    _write_string(buf, legend)
    buf += struct.pack("=dd", zoom_center[0], zoom_center[1])
    buf += struct.pack("=d", zoom_ratio)
    _write_string(buf, zoom_key)
    _write_string(buf, color_adjustments_key)
    buf += struct.pack("=?", is_color_order_bgr)
    _write_mat(buf, image)

    writing_path = _make_unique_filename(while_writing=True)
    done_path = _make_unique_filename(while_writing=False)

    with open(writing_path, "wb") as f:
        f.write(buf)

    os.rename(str(writing_path), str(done_path))


def immdebug(
    image: np.ndarray,
    legend: str = "",
    zoom_center: Tuple[float, float] = (0.0, 0.0),
    zoom_ratio: float = -1.0,
    zoom_key: str = "",
    color_adjustments_key: str = "",
) -> None:
    """Send an image to the immdebug_viewer application.

    Assumes RGB channel order (the default for most image libraries:
    stb, PIL/Pillow, matplotlib, etc.).

    Args:
        image: numpy array (HxW for grayscale, HxWxC for multi-channel)
        legend: display name in the viewer
        zoom_center: initial zoom center (x, y)
        zoom_ratio: initial zoom ratio (-1 for auto-fit)
        zoom_key: link zoom across images sharing this key
        color_adjustments_key: link color adjustments across images sharing this key
    """
    _immdebug_impl(image, legend, zoom_center, zoom_ratio, zoom_key, color_adjustments_key, False)


def immdebug_bgr(
    image: np.ndarray,
    legend: str = "",
    zoom_center: Tuple[float, float] = (0.0, 0.0),
    zoom_ratio: float = -1.0,
    zoom_key: str = "",
    color_adjustments_key: str = "",
) -> None:
    """Send an image to the immdebug_viewer application.

    Assumes BGR channel order (OpenCV default).

    Args:
        image: numpy array (HxW for grayscale, HxWxC for multi-channel)
        legend: display name in the viewer
        zoom_center: initial zoom center (x, y)
        zoom_ratio: initial zoom ratio (-1 for auto-fit)
        zoom_key: link zoom across images sharing this key
        color_adjustments_key: link color adjustments across images sharing this key
    """
    _immdebug_impl(image, legend, zoom_center, zoom_ratio, zoom_key, color_adjustments_key, True)
