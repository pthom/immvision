from typing import Callable
from .imgui_app_params import ImguiAppParams
from .power_save import set_max_wait_before_next_frame


_USE_SDL = True


if _USE_SDL:
    from .imgui_runner import run as _run
    from .sdl_backend import SdlBackend as _SdlBackend

    def run(gui_function: Callable, imgui_app_params: ImguiAppParams = None):
        backend = _SdlBackend(imgui_app_params)
        _run(gui_function, backend, imgui_app_params)
