from immvision import _gl_provider_sentinel
from ._imgui_app_params_helper import _ImguiAppParamsHelper
from .power_save import _power_save_instance
from .auto_size_app_window import AutoSizeAppWindow
from .backend_any import BackendAny
from .gui_types import GuiFunction

import imgui
import imgui.internal

import OpenGL.GL as gl
from typing import Any
import traceback
import logging
import time


_CURRENT_BACKEND: BackendAny = None


def run_with_backend(gui_function: GuiFunction, backend: BackendAny, imgui_app_params_helper: _ImguiAppParamsHelper = None):
    global _CURRENT_BACKEND
    _CURRENT_BACKEND = backend

    running = True
    imgui_app_params = imgui_app_params_helper.imgui_app_params
    was_window_position_saved = False

    backend.init_backend()
    all_monitors_work_areas = backend.get_all_monitors_work_areas()
    yet_unknown_real_window_size_after_auto_size = None
    window_bounds = imgui_app_params_helper.app_window_bounds_initial(
        all_monitors_work_areas, yet_unknown_real_window_size_after_auto_size)
    backend.init_backend_window(
         window_title = imgui_app_params.app_window_title,
         window_bounds = window_bounds,
         fullscreen = imgui_app_params.app_window_full_screen,
         fullscreen_monitor_idx = imgui_app_params.app_window_monitor_index,
         gl_multisamples= imgui_app_params.gl_multisamples
    )


    auto_size_app_window = AutoSizeAppWindow(imgui_app_params_helper, backend)

    imgui.create_context()

    if imgui_app_params.use_power_save is not None:
        _power_save_instance().use_power_save = imgui_app_params.use_power_save

    backend.init_imgui_gl_renderer()

    _gl_provider_sentinel.create_sentinel()

    def cleanup():
        backend.destroy_window()
        if not was_window_position_saved:
            _ImguiAppParamsHelper.write_last_run_window_bounds(backend.get_window_bounds())

        _gl_provider_sentinel.destroy_sentinel()
        backend.destroy_imgui_gl_renderer()
        ctx = imgui.get_current_context()
        if ctx is not None:
            imgui.destroy_context(ctx)

        backend.destroy_gl_context()

        # when using jupyter  notebook, with SDL, on MacOS, the window sometimes does not close
        # attempt to convince it a little more with a second call to close + sleep
        backend.destroy_window()
        time.sleep(0.2)

        backend.quit()



    frame_idx = 0
    while running:

        frame_idx += 1
        if imgui_app_params_helper.want_autosize() and frame_idx == 2:
            # The window was resized
            # We should now recenter the window if needed
            # and ensure it fits on the monitor
            real_window_size_after_auto_size = backend.get_window_size()
            window_bounds = imgui_app_params_helper.app_window_bounds_initial(
                all_monitors_work_areas, real_window_size_after_auto_size)
            backend.set_window_bounds(window_bounds)
            backend.move_window_if_out_of_screen_bounds()

        if not imgui_app_params_helper.want_autosize() and not imgui_app_params.app_window_full_screen and frame_idx == 2:
            # The window was not resized
            # However, we forcefully set its position once again, since some backends ignore
            # it position at window creation time (SDL)
            real_window_size_after_auto_size = backend.get_window_size()
            window_bounds = imgui_app_params_helper.app_window_bounds_initial(
                all_monitors_work_areas, real_window_size_after_auto_size)
            backend.set_window_position(window_bounds.window_position)

        if _power_save_instance().use_power_save:
            backend.wait_for_event_power_save()

        backend.poll_events()
        if backend.should_window_close():
            break

        imgui.new_frame()
        _power_save_instance().on_new_frame()

        if imgui_app_params.app_shall_raise_window:
            backend.raise_window()
            imgui_app_params.app_shall_raise_window = False

        def gui_handler():
            nonlocal running, auto_size_app_window, was_window_position_saved
            if imgui_app_params.provide_default_window:
                auto_size_app_window.begin()

            try:
                gui_function()
            except Exception as e:
                # Early handling of user exceptions
                logging.error(f"\n\n\nPanic! imgui_runner.run: exception in user code! \n    {e}\n\n")
                traceback.print_exception(e, e, e.__traceback__)

                # It is very likely that there is an error in your gui function which was just executed
                # Look at the back trace
                breakpoint()
                # rethrow the exception: this will be caught just few lines below,
                # where some cleanup will be done before rethrowing
                raise

            if imgui_app_params.provide_default_window:
                auto_size_app_window.end()

            if imgui_app_params.app_shall_exit:
                running = False
                _ImguiAppParamsHelper.write_last_run_window_bounds(backend.get_window_bounds())
                was_window_position_saved = True
                backend.destroy_window()
        try:
            gui_handler()
        except Exception as e:
            # Late handling of user exceptions: cleanup and rethrow
            logging.error(f"Panic! Will try to do some cleanup...")
            was_window_position_saved = True
            backend.destroy_window()
            cleanup()
            logging.error(f"Panic! Cleanup ok. Will rethrow the exception...")
            raise


        bg = imgui_app_params.gl_clear_color
        gl.glClearColor(bg[0], bg[1], bg[2], bg[3])
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        imgui.render()
        backend.render_gl_draw_data(imgui.get_draw_data())
        backend.swap_window()

    cleanup()



def get_backend_app_window() -> Any:
    """
    Returns the current backend window.
        - For Sdl, it will be a SDL_Window
        - For Glfw, it will be a GLFWwindow
    """
    return _CURRENT_BACKEND.app_window()


def get_backend_gl_context() -> Any:
    """
    Returns the current backend OpenGl context
        - For Sdl, it will be a SDL_GlContext
        - For Glfw, it is None
    """
    return _CURRENT_BACKEND.gl_context
