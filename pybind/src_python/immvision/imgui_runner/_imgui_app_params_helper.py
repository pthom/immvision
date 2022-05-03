from typing import List, Optional
import logging
import os

from . import _DEBUG_IMGUI_RUNNER
from .imgui_app_params import ImguiAppParams
from .gui_types import WindowPosition, WindowSize, WindowBounds


APP_WINDOW_POS_INI_FILE = "imgui_app_window.ini"


class _ImguiAppParamsHelper:
    imgui_app_params: ImguiAppParams

    def __init__(self, imgui_app_params: ImguiAppParams):
        self.imgui_app_params = imgui_app_params

    def has_windows_size_info(self):
        if self.imgui_app_params.app_window_maximized or self.imgui_app_params.app_window_full_screen:
            return True
        if self.imgui_app_params.app_window_size is not None:
            return True
        if self.imgui_app_params.app_window_size_restore_last and _ImguiAppParamsHelper._read_last_run_window_bounds().window_size is not None:
            return True
        return False

    def has_windows_position_info(self):
        if self.imgui_app_params.app_window_maximized:
            return True
        if self.imgui_app_params.app_window_position is not None:
            return True
        if self.imgui_app_params.app_window_position_restore_last and _ImguiAppParamsHelper._read_last_run_window_bounds().window_position is not None:
            return True
        return False

    def app_window_bounds_initial(
            self,
            all_monitor_work_areas: List[WindowBounds],
            real_window_size_after_auto_size: Optional[WindowSize]
    ) -> WindowBounds:

        # if maximized, use the full work area of the selected monitor
        if self.imgui_app_params.app_window_maximized:
            window_bounds = all_monitor_work_areas[self.imgui_app_params.app_window_monitor_index]
            return window_bounds

        # if full screen, let the backend handle the positioning, but pass the size
        # so that it can change the resolution if required
        if self.imgui_app_params.app_window_full_screen:
            return WindowBounds(None, self.imgui_app_params.app_window_size)

        #
        # Standard windowed mode
        #
        window_bounds_last_run = _ImguiAppParamsHelper._read_last_run_window_bounds()

        # Window size
        def compute_size() -> WindowSize:
            if real_window_size_after_auto_size is not None:
                return real_window_size_after_auto_size
            if self.imgui_app_params.app_window_size is not None:
                return self.imgui_app_params.app_window_size
            if self.imgui_app_params.app_window_size_restore_last \
                    and window_bounds_last_run is not None \
                    and window_bounds_last_run.window_size is not None:
                return window_bounds_last_run.window_size
            else:
                return (150, 150)

        window_size = compute_size()

        # Window position
        def center_window_on_monitor() -> WindowPosition:
            monitor_idx = self.imgui_app_params.app_window_monitor_index
            assert 0 <= monitor_idx < len(all_monitor_work_areas)
            pos = all_monitor_work_areas[monitor_idx].win_position_centered(window_size)
            return pos

        def compute_position() -> WindowPosition:
            if self.imgui_app_params.app_window_position is not None:
                return self.imgui_app_params.app_window_position

            if self.imgui_app_params.app_window_position_restore_last \
                    and window_bounds_last_run is not None \
                    and window_bounds_last_run.window_position is not None:
                return window_bounds_last_run.window_position
            else:
                return center_window_on_monitor()

        window_position = compute_position()

        window_bounds_initial = WindowBounds(window_position, window_size)
        return window_bounds_initial

    @staticmethod
    def write_last_run_window_bounds(window_bounds: WindowBounds):
        content = f"""
[WIN]
WindowPosition={window_bounds.window_position[0]},{window_bounds.window_position[1]}
WindowSize={window_bounds.window_size[0]},{window_bounds.window_size[1]}
    """
        with open(APP_WINDOW_POS_INI_FILE, "w") as f:
            f.write(content)
        if _DEBUG_IMGUI_RUNNER:
            logging.debug(f"write_last_run_window_bounds:{content}")

    @staticmethod
    def _read_last_run_window_bounds() -> Optional[WindowBounds]:
        if not os.path.isfile(APP_WINDOW_POS_INI_FILE):
            return WindowBounds()
        try:
            with open(APP_WINDOW_POS_INI_FILE, "r") as f:
                lines = f.readlines()

            window_bounds = WindowBounds()
            for line in lines:
                if "WindowPosition=" in line:
                    items = line[len("WindowPosition="):].split(",")
                    window_bounds.window_position = (int(items[0]), int(items[1]))
                if "WindowSize=" in line:
                    items = line[len("WindowSize="):].split(",")
                    window_bounds.window_size = (int(items[0]), int(items[1]))
            return window_bounds
        except Exception:
            return WindowBounds()

    def want_autosize(self) -> bool:
        return not self.has_windows_size_info()
