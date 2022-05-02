import time

from .any_backend import AnyBackend
from sdl2 import *
from typing import Optional, Tuple
from .power_save import power_save_instance
from imgui.integrations.sdl2 import SDL2Renderer
from .gui_types import WindowPosition, WindowSize, WindowBounds
from .imgui_app_params import ImguiAppParams, _ImguiAppParamsHelper
import ctypes


class SdlBackend(AnyBackend):
    _should_window_close: bool  = False

    def __init__(self, imgui_app_params_helper: _ImguiAppParamsHelper):
        AnyBackend.__init__(self, imgui_app_params_helper)

    def get_nb_monitors(self):
        r = SDL_GetNumVideoDisplays()
        return r

    def get_monitor_work_area_from_index(self, monitor_index: int) -> WindowBounds:
        monitor_index_c = c_int()
        monitor_index_c.value = monitor_index
        usable_bounds = SDL_Rect()
        r = SDL_GetDisplayUsableBounds(monitor_index_c, usable_bounds)
        if r != 0:
            print("Error when calling SDL_GetDisplayUsableBounds")

        monitor_bounds = WindowBounds()
        monitor_bounds.window_position = (usable_bounds.x, usable_bounds.y)
        monitor_bounds.window_size = (usable_bounds.w, usable_bounds.h)
        return monitor_bounds

    def init_backend(self):
        if SDL_Init(SDL_INIT_EVERYTHING) < 0:
            print("Error:SDL_Init failed! SDL Error: " + SDL_GetError().decode("utf-8"))
            exit(1)

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1)

        if self.imgui_app_params.gl_multisamples > 0:
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, self.imgui_app_params.gl_multisamples)

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)

        SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, b"1")
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, b"1")

        window_flags = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)

        window_bounds = self.initial_window_bounds()
        window_position = self.initial_window_position(window_bounds)

        window = SDL_CreateWindow(
            self.imgui_app_params.app_window_title.encode('utf-8'),
            window_position[0], window_position[1],
            window_bounds.window_size[0], window_bounds.window_size[1],
            window_flags)
        if window is None:
            print("Error: SDL_CreateWindow failed! SDL Error: " + SDL_GetError().decode("utf-8"))
            exit(1)

        gl_context = SDL_GL_CreateContext(window)
        if gl_context is None:
            print("Error: SDL_GL_CreateContext failed! SDL Error: " + SDL_GetError().decode("utf-8"))
            exit(1)

        SDL_GL_MakeCurrent(window, gl_context)

        swap_interval_result = SDL_GL_SetSwapInterval(1)  # Enable vsync
        if swap_interval_result < 0:
            print(f"Warning, SDL_GL_SetSwapInterval returned {SDL_GL_SetSwapInterval}")

        self.raise_window()
        self.gl_context = gl_context
        self.app_window = window

    def init_imgui_gl_renderer(self):
        self.imgui_gl_renderer = SDL2Renderer(self.app_window)

    def destroy_imgui_gl_renderer(self):
        SDL_HideWindow(self.app_window)
        time.sleep(0.5)
        self.imgui_gl_renderer.shutdown()

    def is_window_hidden(self) -> bool:
        window_flags = SDL_GetWindowFlags(self.app_window)
        window_is_hidden = window_flags & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED) > 0
        return window_is_hidden

    def wait_for_event_power_save(self):
        waiting_time_s = power_save_instance().get_event_waiting_timeout(self.is_window_hidden())
        if waiting_time_s > 0:
            waiting_time_ms = int(1000.0 * waiting_time_s)
            SDL_WaitEventTimeout(None, waiting_time_ms)

    def get_window_position(self) -> WindowPosition:
        win_pos = (c_int(), c_int())
        SDL_GetWindowPosition(self.app_window, win_pos[0], win_pos[1])
        return win_pos[0].value, win_pos[1].value

    def set_window_position(self, window_position: WindowPosition):
        SDL_SetWindowPosition(self.app_window, window_position[0], window_position[1])

    def get_window_size(self):
        win_size = (c_int(), c_int())
        SDL_GetWindowSize(self.app_window, win_size[0], win_size[1])
        return win_size[0].value, win_size[1].value

    def set_window_size(self, window_size: WindowSize):
        r = SDL_SetWindowSize(self.app_window, window_size[0], window_size[1])
        if not((r is None) or (r == 0)):
            print("Error during SDL_SetWindowSize")

    def poll_events(self):
        was_window_closed = False
        event = SDL_Event()
        while SDL_PollEvent(ctypes.byref(event)) != 0:
            if event.type == SDL_QUIT:
                was_window_closed = True
                break
            self.imgui_gl_renderer.process_event(event)
        self.imgui_gl_renderer.process_inputs()
        self._should_window_close = was_window_closed

    def should_window_close(self) -> bool:
        return self._should_window_close

    def render_gl_draw_data(self, draw_data):
        self.imgui_gl_renderer.render(draw_data)

    def destroy_window(self):
        SDL_DestroyWindow(self.app_window)

    def swap_window(self):
        SDL_GL_SwapWindow(self.app_window)

    def destroy_gl_context(self):
        SDL_GL_DeleteContext(self.gl_context)

    def quit(self):
        SDL_Quit()

    def raise_window(self):
        # Despite those efforts, the app does not come to the front under MacOS
        SDL_ShowWindow(self.app_window)
        SDL_FlashWindow(self.app_window, SDL_FLASH_UNTIL_FOCUSED)
        SDL_RaiseWindow(self.app_window)
