import logging
import time

from .backend_any import BackendAny
from sdl2 import *
from .power_save import power_save_instance
from imgui.integrations.sdl2 import SDL2Renderer
from .gui_types import WindowPosition, WindowSize, WindowBounds
import ctypes
from . import _DEBUG_IMGUI_RUNNER
from .debug_utils import log

if _DEBUG_IMGUI_RUNNER:
    pass
    from .debug_utils import verbose_function
    #SDL_CreateWindow = verbose_function(dump_args=True, dump_args_at_exit=True, dump_return=True)(SDL_CreateWindow)
    #SDL_SetWindowPosition = verbose_function(dump_args=True, dump_args_at_exit=False, dump_return=True)(SDL_SetWindowPosition)
    #SDL_SetWindowSize = verbose_function(dump_args=True, dump_args_at_exit=False, dump_return=True)(SDL_SetWindowSize)
    #SDL_GetWindowPosition = verbose_function(dump_args=True, dump_args_at_exit=True, dump_return=True)(SDL_GetWindowPosition)



class BackendSdl(BackendAny):
    _should_window_close: bool  = False

    def __init__(self):
        BackendAny.__init__(self)

    def get_nb_monitors(self):
        r = SDL_GetNumVideoDisplays()
        return r

    def get_one_monitor_work_area(self, monitor_index: int) -> WindowBounds:
        monitor_index_c = c_int()
        monitor_index_c.value = monitor_index
        usable_bounds = SDL_Rect()
        r = SDL_GetDisplayUsableBounds(monitor_index_c, usable_bounds)
        if r != 0:
            log(logging.ERROR, f"Error when calling SDL_GetDisplayUsableBounds")

        monitor_bounds = WindowBounds()
        monitor_bounds.window_position = (usable_bounds.x, usable_bounds.y)
        monitor_bounds.window_size = (usable_bounds.w, usable_bounds.h)
        return monitor_bounds

    def init_backend(self):
        if SDL_Init(SDL_INIT_EVERYTHING) < 0:
            log(logging.ERROR, "Error:SDL_Init failed! SDL Error: " + SDL_GetError().decode("utf-8"))
            exit(1)

    def init_backend_window(
            self,
            window_title: str,
            window_bounds: WindowBounds,
            fullscreen: bool,
            fullscreen_monitor_idx: int,
            gl_multisamples: int
        ):
        self.full_screen = fullscreen
        self.full_screen_monitor_idx = fullscreen_monitor_idx

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1)

        if gl_multisamples > 0:
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, gl_multisamples)

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)

        SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, b"1")
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, b"1")

        if fullscreen:
            # Create a full screen window
            nb_monitors = self.get_nb_monitors()
            assert 0 <= fullscreen_monitor_idx < nb_monitors
            monitor_bounds = self.get_one_monitor_work_area(fullscreen_monitor_idx)
            if window_bounds.window_size is None:
                video_size = (monitor_bounds.window_size[0], monitor_bounds.window_size[1])
            else:
                video_size = window_bounds.window_size

            window_flags = (SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI)
            if window_bounds.window_size is None:
                window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP
            else:
                window_flags |= SDL_WINDOW_FULLSCREEN
            window = SDL_CreateWindow(
                window_title.encode('utf-8'),
                monitor_bounds.window_position[0], monitor_bounds.window_position[1],
                video_size[0], video_size[1],
                window_flags)
        else:
            window_flags = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)
            window = SDL_CreateWindow(
                window_title.encode('utf-8'),
                window_bounds.window_position[0], window_bounds.window_position[1],
                window_bounds.window_size[0], window_bounds.window_size[1],
                window_flags)

        if window is None:
            log(logging.ERROR, "Error: SDL_CreateWindow failed! SDL Error: " + SDL_GetError().decode("utf-8"))
            exit(1)

        gl_context = SDL_GL_CreateContext(window)
        if gl_context is None:
            log(logging.ERROR, "Error: SDL_GL_CreateContext failed! SDL Error: " + SDL_GetError().decode("utf-8"))
            exit(1)

        SDL_GL_MakeCurrent(window, gl_context)

        swap_interval_result = SDL_GL_SetSwapInterval(1)  # Enable vsync
        if swap_interval_result < 0:
            log(logging.WARNING, f"Warning, SDL_GL_SetSwapInterval returned {SDL_GL_SetSwapInterval}")

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
        current_window_position = self.get_window_position()
        if window_position != current_window_position:
            SDL_SetWindowPosition(self.app_window, window_position[0], window_position[1])
            obtained_window_position = self.get_window_position()
            if obtained_window_position != window_position:
                log(logging.WARNING, f"BackendSdl: set_window_position: fail {current_window_position=} {window_position=} {obtained_window_position=}")

    def get_window_size(self):
        win_size = (c_int(), c_int())
        SDL_GetWindowSize(self.app_window, win_size[0], win_size[1])
        return win_size[0].value, win_size[1].value

    def set_window_size(self, window_size: WindowSize):
        r = SDL_SetWindowSize(self.app_window, window_size[0], window_size[1])
        if not((r is None) or (r == 0)):
            log(logging.WARNING, "Error during SDL_SetWindowSize")

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
