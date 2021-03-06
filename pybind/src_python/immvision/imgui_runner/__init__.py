_DEBUG_IMGUI_RUNNER = False  # should be before the imports, since it is used by them
from .imgui_app_params import ImguiAppParams
from ._imgui_app_params_helper import _ImguiAppParamsHelper
from .power_save import power_save_set_max_wait_next_frame, power_save_enable, power_save_disable
from .imgui_runner import _ImGuiRunner
from .imgui_runner import get_backend_app_window, get_backend_gl_context
from .gui_types import GuiFunction
from .backend_factory import factor_backend as _factor_backend, BackendType

from typing import Optional

_DEFAULT_BACKEND_TYPE = BackendType.GLFW
_IMGUI_APP_PARAMS_HELPER_MAIN: _ImguiAppParamsHelper = None


def run(
        gui_function: GuiFunction,
        imgui_app_params: Optional[ImguiAppParams] = None,
        backend_type: Optional[BackendType]= None):
    """
    :param gui_function:
        The function that contains the code for the widgets you want to display
    :param imgui_app_params:
        Application and ImGui options (size, position, title, etc)
    :param backend_type:
        The backend to use (optional). Will use Glfw if not specified.

    When the app is running, you can exit it via exit_app(), and raise its window via raise_app_window
    """

    global _IMGUI_APP_PARAMS_HELPER_MAIN
    if imgui_app_params is None:
        imgui_app_params = ImguiAppParams()
    imgui_app_params.app_shall_exit = False
    imgui_app_params_helper = _ImguiAppParamsHelper(imgui_app_params)
    _IMGUI_APP_PARAMS_HELPER_MAIN = imgui_app_params_helper

    if backend_type is None:
        backend_type = _DEFAULT_BACKEND_TYPE

    backend = _factor_backend(backend_type)
    runner = _ImGuiRunner(gui_function, backend, imgui_app_params_helper)
    runner.run()


def exit_app():
    _IMGUI_APP_PARAMS_HELPER_MAIN.imgui_app_params.app_shall_exit = True


def raise_app_window():
    _IMGUI_APP_PARAMS_HELPER_MAIN.imgui_app_params.app_shall_raise_window = True


if _DEBUG_IMGUI_RUNNER:
    import logging
    logging.basicConfig(level=logging.DEBUG)
