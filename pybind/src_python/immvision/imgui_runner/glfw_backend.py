from .any_backend import AnyBackend
from sdl2 import *
from typing import Optional, Tuple, Any
from .power_save import power_save_instance
from imgui.integrations.sdl2 import SDL2Renderer
from .imgui_app_params import get_app_window_size_initial
import ctypes


class SdlBackend(AnyBackend):
    def __init__(self, imgui_app_params):
        AnyBackend.__init__(self, imgui_app_params)

    def get_screen_size(self):
        margin_x = 30
        margin_y = 50
        display_index = 0

        display_mode = SDL_DisplayMode()
        r = SDL_GetCurrentDisplayMode(display_index, display_mode)
        if not((r is None) or (r == 0)):
            raise RuntimeError("Could not query SDL_GetCurrentDisplayMode")
        return display_mode.w - margin_x, display_mode.h - margin_y

    def init_backend(
            self,
            window_position: Optional[Tuple[int, int]],
            window_size: Tuple[int, int]
        ):
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

        if window_position is None:
            sdl_window_position = (SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)
        else:
            sdl_window_position = (window_position[0], window_position[1])

        window = SDL_CreateWindow(
            self.imgui_app_params.app_window_title.encode('utf-8'),
            sdl_window_position[0], sdl_window_position[1],
            window_size[0], window_size[1],
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

        SDL_RaiseWindow(window)
        self.gl_context = gl_context
        self.app_window = window

    def init_gl_renderer(self):
        self.gl_renderer = SDL2Renderer(self.app_window)

    def wait_for_event_power_save(self):
        window_flags = SDL_GetWindowFlags(self.app_window)
        window_is_hidden = window_flags & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED) > 0
        waiting_time = power_save_instance()._idle_wait_time if window_is_hidden else power_save_instance().get_event_waiting_time()
        if waiting_time > 0:
            waiting_time_ms = int(1000.0 * waiting_time)
            SDL_WaitEventTimeout(None, waiting_time_ms)

    def get_window_position(self) -> Tuple[int, int]:
        win_pos = (c_int(), c_int())
        SDL_GetWindowPosition(self.app_window, win_pos[0], win_pos[1])
        return win_pos[0].value, win_pos[1].value

    def set_window_position(self, window_position: Tuple[int, int]):
        SDL_SetWindowPosition(self.app_window, window_position[0], window_position[1])

    def get_window_size(self):
        win_size = (c_int(), c_int())
        SDL_GetWindowSize(self.app_window, win_size[0], win_size[1])
        return win_size[0].value, win_size[1].value

    def set_window_size(self, window_size:Tuple[int, int]):
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
            self.gl_renderer.process_event(event)
        self.gl_renderer.process_inputs()
        return was_window_closed

    def render_gl_draw_data(self, draw_data):
        self.gl_renderer.render(draw_data)

    def shutdown_gl_renderer(self):
        self.gl_renderer.shutdown()

    def destroy_window(self):
        SDL_DestroyWindow(self.app_window)

    def swap_window(self):
        SDL_GL_SwapWindow(self.app_window)

    def delete_gl_context(self):
        SDL_GL_DeleteContext(self.gl_context)

    def destroy_window(self):
        SDL_DestroyWindow(self.app_window)

    def quit(self):
        SDL_Quit()
