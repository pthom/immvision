from typing import Callable

from .imgui_app_params import ImguiAppParams, _ImguiAppParamsHelper
from .power_save import set_max_wait_before_next_frame
from .imgui_runner import run_with_backend
from .backend_types import GuiFunction


_USE_SDL = True
_USE_GLFW = False

if _USE_GLFW:
    def run(gui_function: GuiFunction, imgui_app_params: ImguiAppParams = None):
        from .glfw_backend import GlfwBackend as _GlfwBackend
        backend = _GlfwBackend(imgui_app_params)
        run_with_backend(gui_function, backend, imgui_app_params)

elif _USE_SDL:

    def run(gui_function: GuiFunction, imgui_app_params: ImguiAppParams = None):
        from .sdl_backend import SdlBackend as _SdlBackend

        if imgui_app_params is None:
            imgui_app_params = ImguiAppParams()
        imgui_app_params.app_shall_exit = False

        imgui_app_params_helper = _ImguiAppParamsHelper(imgui_app_params)
        backend = _SdlBackend(imgui_app_params_helper)
        run_with_backend(gui_function, backend, imgui_app_params_helper)
