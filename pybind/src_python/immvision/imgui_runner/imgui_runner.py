import os.path

from immvision import _gl_provider_sentinel
from .imgui_app_params import ImguiAppParams, _ImguiAppParamsHelper
import OpenGL.GL as gl

import imgui

import imgui.internal
from typing import Callable
from .power_save import power_save_instance
from .auto_size_app_window import AutoSizeAppWindow
from .any_backend import AnyBackend
from .gui_types import GuiFunction


def run_with_backend(gui_function: GuiFunction, backend: AnyBackend, imgui_app_params_helper: _ImguiAppParamsHelper = None):
    running = True
    imgui_app_params = imgui_app_params_helper.imgui_app_params
    was_window_position_saved = False

    backend.init_backend()

    auto_size_app_window = AutoSizeAppWindow(imgui_app_params_helper, backend)

    imgui.create_context()

    power_save_instance().use_power_save = imgui_app_params.use_power_save

    backend.init_imgui_gl_renderer()

    while running:

        if power_save_instance().use_power_save:
            backend.wait_for_event_power_save()

        backend.poll_events()
        if backend.should_window_close():
            break

        imgui.new_frame()
        power_save_instance().on_new_frame()

        if imgui_app_params.app_shall_raise_window:
            backend.raise_window()
            imgui_app_params.app_shall_raise_window = False

        def gui_handler():
            nonlocal running, auto_size_app_window, was_window_position_saved
            if imgui_app_params.provide_default_window:
                auto_size_app_window.begin()
            gui_function()
            if imgui_app_params.provide_default_window:
                auto_size_app_window.end()

            if imgui_app_params.app_shall_exit:
                running = False
                imgui_app_params_helper.write_last_run_window_bounds(backend.get_window_bounds())
                was_window_position_saved = True
                backend.destroy_window()
        gui_handler()

        bg = imgui_app_params.gl_clear_color
        gl.glClearColor(bg[0], bg[1], bg[2], bg[3])
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        imgui.render()
        backend.render_gl_draw_data(imgui.get_draw_data())
        backend.swap_window()

    if not was_window_position_saved:
        imgui_app_params_helper.write_last_run_window_bounds(backend.get_window_bounds())

    backend.destroy_imgui_gl_renderer()
    ctx = imgui.get_current_context()
    if ctx is not None:
        imgui.destroy_context(ctx)

    backend.destroy_gl_context()
    backend.destroy_window()
    backend.quit()
