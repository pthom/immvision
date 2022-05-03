_DEBUG_IMGUI_RUNNER = True
from .imgui_app_params import ImguiAppParams, _ImguiAppParamsHelper
from .power_save import set_max_wait_before_next_frame
from .imgui_runner import run_with_backend as _run_with_backend
from .gui_types import GuiFunction
from .backend_factory import factor_backend as _factor_backend, BackendType


_BACKEND_TYPE = BackendType.SDL
_IMGUI_APP_PARAMS_HELPER_MAIN: _ImguiAppParamsHelper = None


def run(gui_function: GuiFunction, imgui_app_params: ImguiAppParams = None):
    global _IMGUI_APP_PARAMS_HELPER_MAIN
    if imgui_app_params is None:
        imgui_app_params = ImguiAppParams()
    imgui_app_params.app_shall_exit = False
    imgui_app_params_helper = _ImguiAppParamsHelper(imgui_app_params)
    _IMGUI_APP_PARAMS_HELPER_MAIN = imgui_app_params_helper

    backend = _factor_backend(_BACKEND_TYPE, imgui_app_params_helper)
    _run_with_backend(gui_function, backend, imgui_app_params_helper)


def exit_app():
    _IMGUI_APP_PARAMS_HELPER_MAIN.imgui_app_params.app_shall_exit = True


def raise_app_window():
    _IMGUI_APP_PARAMS_HELPER_MAIN.imgui_app_params.app_shall_raise_window = True


if _DEBUG_IMGUI_RUNNER:
    import logging
    logging.basicConfig(level=logging.DEBUG)
