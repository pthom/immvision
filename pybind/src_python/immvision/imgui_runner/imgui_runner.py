from immvision import _gl_provider_sentinel
from ._imgui_app_params_helper import _ImguiAppParamsHelper, ImguiAppParams
from .power_save import _power_save_instance
from .auto_size_app_window import AutoSizeAppWindow
from .backend_any import BackendAny
from .gui_types import GuiFunction, WindowBounds


import imgui
import imgui.internal

import OpenGL.GL as gl
from typing import Any, List
import traceback
import logging
import time


_CURRENT_BACKEND: BackendAny = None


class _ImGuiRunner:
    """A class that runs an imgui application with a given backend

    Features:
    - PowerSave: automatic lowering of the FPS when not used
    - AutoSize: auto size the app window to its inner widgets
    - App Window options: full screen, maximized, monitor index, etc
    - Restore last run Window Position and / or size (optional)
    - Provide a full screen empty background inside the ImGui window (optional)
    """

    backend: BackendAny = None
    imgui_app_params_helper: _ImguiAppParamsHelper = None
    imgui_app_params: ImguiAppParams
    running:bool = True
    auto_size_app_window: AutoSizeAppWindow = None
    was_window_position_saved: bool = False
    gui_function: GuiFunction = None
    all_monitors_work_areas: List[WindowBounds] = None

    def __init__(self, gui_function: GuiFunction, backend: BackendAny, imgui_app_params_helper: _ImguiAppParamsHelper = None):
        self.gui_function = gui_function

        self.backend = backend
        global _CURRENT_BACKEND
        _CURRENT_BACKEND = self.backend

        self.imgui_app_params_helper = imgui_app_params_helper
        self.imgui_app_params = self.imgui_app_params_helper.imgui_app_params
        if self.imgui_app_params.use_power_save is not None:
            _power_save_instance().use_power_save = self.imgui_app_params.use_power_save

        self.running = True
        self.was_window_position_saved = False

        self.init_backend_window()
        self.auto_size_app_window = AutoSizeAppWindow(self.imgui_app_params_helper, self.backend)
        imgui.create_context()

        self._init_backend_gl()


    def init_backend_window(self):
        # has to be done before imgui.create_context()
        self.backend.init_backend()
        self.all_monitors_work_areas = self.backend.get_all_monitors_work_areas()

        yet_unknown_real_window_size_after_auto_size = None
        window_bounds = self.imgui_app_params_helper.app_window_bounds_initial(
            self.all_monitors_work_areas, yet_unknown_real_window_size_after_auto_size)
        self.backend.init_backend_window(
             window_title = self.imgui_app_params.app_window_title,
             window_bounds = window_bounds,
             fullscreen = self.imgui_app_params.app_window_full_screen,
             fullscreen_monitor_idx = self.imgui_app_params.app_window_monitor_index,
             gl_multisamples= self.imgui_app_params.gl_multisamples
        )

    def _init_backend_gl(self):
        # has to be done after imgui.create_context()
        self.backend.init_imgui_gl_renderer()
        _gl_provider_sentinel.create_sentinel()

    def _force_window_position_or_size(self):
        # This is done at the second frame, once we know the size of all the widgets
        if self.imgui_app_params_helper.want_autosize():
            # The window was resized by AutoSizeApp
            # We should now recenter the window if needed
            # and ensure it fits on the monitor
            real_window_size_after_auto_size = self.backend.get_window_size()
            window_bounds = self.imgui_app_params_helper.app_window_bounds_initial(
                self.all_monitors_work_areas, real_window_size_after_auto_size)
            self.backend.set_window_bounds(window_bounds)
            self.backend.move_window_if_out_of_screen_bounds()

        if not self.imgui_app_params_helper.want_autosize() and not self.imgui_app_params.app_window_full_screen:
            # The window was not resized
            # However, we forcefully set its position once again, since some backends ignore
            # it position at window creation time (SDL)
            real_window_size_after_auto_size = self.backend.get_window_size()
            window_bounds = self.imgui_app_params_helper.app_window_bounds_initial(
                self.all_monitors_work_areas, real_window_size_after_auto_size)
            self.backend.set_window_position(window_bounds.window_position)

    def _gui_handler(self):
        """
        Wraps self.gui_function :
            - adds a background window
            - nicely handle user exceptions
            - handle exit
        """
        if self.imgui_app_params.provide_default_window:
            self.auto_size_app_window.begin()

        try:
            self.gui_function()
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

        if self.imgui_app_params.provide_default_window:
            self.auto_size_app_window.end()

        if self.imgui_app_params.app_shall_exit:
            self.running = False
            _ImguiAppParamsHelper.write_last_run_window_bounds(self.backend.get_window_bounds())
            self.was_window_position_saved = True
            self.backend.destroy_window()

    def run(self):
        """run the app
        """
        frame_idx = 0
        while self.running:

            frame_idx += 1
            if frame_idx == 2:
                self._force_window_position_or_size()

            if _power_save_instance().use_power_save:
                self.backend.wait_for_event_power_save()

            self.backend.poll_events()
            if self.backend.should_window_close():
                break

            imgui.new_frame()
            _power_save_instance().on_new_frame()

            if self.imgui_app_params.app_shall_raise_window:
                self.backend.raise_window()
                self.imgui_app_params.app_shall_raise_window = False

            try:
                self._gui_handler()
            except Exception as e:
                # Late handling of user exceptions: cleanup and rethrow
                logging.error(f"Panic! Will try to do some cleanup...")
                self.was_window_position_saved = True
                self.backend.destroy_window()
                self._cleanup()
                logging.error(f"Panic! Cleanup ok. Will rethrow the exception...")
                raise

            bg = self.imgui_app_params.gl_clear_color
            gl.glClearColor(bg[0], bg[1], bg[2], bg[3])
            gl.glClear(gl.GL_COLOR_BUFFER_BIT)

            imgui.render()
            self.backend.render_gl_draw_data(imgui.get_draw_data())
            self.backend.swap_window()

        self._cleanup()

    def _cleanup(self):
        self.backend.destroy_window()
        if not self.was_window_position_saved:
            _ImguiAppParamsHelper.write_last_run_window_bounds(self.backend.get_window_bounds())

        _gl_provider_sentinel.destroy_sentinel()
        self.backend.destroy_imgui_gl_renderer()
        ctx = imgui.get_current_context()
        if ctx is not None:
            imgui.destroy_context(ctx)

        self.backend.destroy_gl_context()

        # when using jupyter  notebook, with SDL, on MacOS, the window sometimes does not close
        # attempt to convince it a little more with a second call to close + sleep
        self.backend.destroy_window()
        time.sleep(0.2)

        self.backend.quit()


def get_backend_app_window() -> Any:
    """
    Returns the current self.backend window.
        - For Sdl, it will be a SDL_Window
        - For Glfw, it will be a GLFWwindow
    """
    return _CURRENT_BACKEND.app_window()


def get_backend_gl_context() -> Any:
    """
    Returns the current self.backend OpenGl context
        - For Sdl, it will be a SDL_GlContext
        - For Glfw, it is None
    """
    return _CURRENT_BACKEND.gl_context
