from .imgui_runner import run as _run, GuiFunction, ImguiAppParams, BackendType
from . import _gl_provider_sentinel
from typing import Optional

from .imgui_runner import exit_app, raise_app_window, power_save_set_max_wait_next_frame, power_save_enable, power_save_disable


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

    Note: this function is a wrapper around imgui_runner.run(), that adds callback for OpenGL
    (they will create / destroy the ImmVision texture cache)
    """
    if imgui_app_params is None:
        imgui_app_params = ImguiAppParams()
    imgui_app_params.imgui_app_callbacks.on_open_gl_ready = \
        _gl_provider_sentinel.immvision_create_gl_sentinel
    imgui_app_params.imgui_app_callbacks.on_open_gl_before_destroy = \
        _gl_provider_sentinel.immvision_destroy_gl_sentinel

    _run(gui_function, imgui_app_params, backend_type)

