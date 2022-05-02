from .imgui_app_params import ImguiAppParams, _ImguiAppParamsHelper
from .power_save import set_max_wait_before_next_frame
from .imgui_runner import run_with_backend
from .gui_types import GuiFunction
from .backend_factory import factor_backend as _factor_backend, BackendType


BACKEND_TYPE = BackendType.SDL


def run(gui_function: GuiFunction, imgui_app_params: ImguiAppParams = None):
    if imgui_app_params is None:
        imgui_app_params = ImguiAppParams()
    imgui_app_params.app_shall_exit = False
    imgui_app_params_helper = _ImguiAppParamsHelper(imgui_app_params)

    backend = _factor_backend(BACKEND_TYPE, imgui_app_params_helper)
    run_with_backend(gui_function, backend, imgui_app_params_helper)
