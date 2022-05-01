import os.path

from immvision import _gl_provider_sentinel
from .imgui_app_params import ImguiAppParams, last_run_window_position, write_last_run_window_position, get_app_window_size_initial
import OpenGL.GL as gl

import imgui

import imgui.internal
from typing import Callable
from .power_save import power_save_instance
from .auto_size_app_window import AutoSizeAppWindow
from .any_backend import AnyBackend


def run(gui_function: Callable, backend: AnyBackend, imgui_app_params: ImguiAppParams = None):
    if imgui_app_params is None:
        imgui_app_params = ImguiAppParams()

    imgui_app_params.app_shall_exit = False
    running = True
    was_window_position_saved = False

    backend.init_backend(last_run_window_position(), get_app_window_size_initial(imgui_app_params))
    auto_size_app_window = AutoSizeAppWindow(imgui_app_params, backend)
    imgui.create_context()

    power_save_instance().use_power_save = imgui_app_params.use_power_save

    backend.init_gl_renderer()

    while running:

        if power_save_instance().use_power_save:
            backend.wait_for_event_power_save()

        was_window_closed = backend.poll_events()
        if was_window_closed:
            break

        imgui.new_frame()
        power_save_instance().on_new_frame()

        def gui_handler():
            nonlocal running, auto_size_app_window, was_window_position_saved
            if imgui_app_params.provide_default_window:
                auto_size_app_window.begin()
            gui_function()
            if imgui_app_params.provide_default_window:
                auto_size_app_window.end()

            if imgui_app_params.app_shall_exit:
                running = False
                write_last_run_window_position(backend.get_window_position())
                was_window_position_saved = True
                backend.destroy_window()
        gui_handler()

        bg = imgui_app_params.background_color
        gl.glClearColor(bg[0], bg[1], bg[2], bg[3])
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        imgui.render()
        backend.render_gl_draw_data(imgui.get_draw_data())
        backend.swap_window()

    if not was_window_position_saved:
        write_last_run_window_position(backend.get_window_position())

    backend.shutdown_gl_renderer()
    ctx = imgui.get_current_context()
    if ctx is not None:
        imgui.destroy_context(ctx)

    backend.delete_gl_context()
    backend.destroy_window()
    backend.quit()
