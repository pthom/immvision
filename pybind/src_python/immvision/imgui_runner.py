from . import _gl_provider_sentinel
from sdl2 import *
import ctypes
import OpenGL.GL as gl

import imgui

from imgui.integrations.sdl2 import SDL2Renderer
import imgui.internal

from dataclasses import dataclass
import typing
from typing import Callable, Tuple


@dataclass
class ImguiAppParams:
    # Title and Size
    app_window_title: str = "ImGui Application"
    # app_window_size: Application window size: if you do not change it,
    # the app window size will try to match the combined widgets size.
    app_window_size: Tuple[int, int] = (0, 0)
    # Provide a default full *imgui* window inside the app
    provide_default_window: bool = True
    # Make the App window almost full screen?
    windowed_full_screen: bool = False
    # Change this bool to True when you want to exit
    app_shall_exit: bool = False
    # Use power save: if this is true, you can increase manually the idle FPS by calling
    # imgui_runner.power_save.set_max_wait_before_next_frame( 1 / desired_fps)
    use_power_save: bool = True
    # Anti-aliasing number of samples
    # - if the call to SDL_GL_CreateContext fails, try reducing the value to zero
    # - if you want better antialiasing, you can also increase it
    gl_multisamples = 4


class _PowerSave:
    USE_POWER_SAVE = True
    _idle_wait_time = 1. / 4. # 4 fps if idle
    _max_wait_before_next_frame_seconds: float = _idle_wait_time

    def on_new_frame(self):
        self._max_wait_before_next_frame_seconds = _PowerSave._idle_wait_time

    def set_max_wait_before_next_frame(self, time_seconds):
        assert(time_seconds >= 0)
        self._max_wait_before_next_frame_seconds = time_seconds

    def get_event_waiting_time(self):
        return self._max_wait_before_next_frame_seconds


power_save =  _PowerSave()


def get_screen_size():
    margin_x = 30
    margin_y = 50
    display_index = 0

    display_mode = SDL_DisplayMode()
    r = SDL_GetCurrentDisplayMode(display_index, display_mode)
    if not((r is None) or (r == 0)):
        raise RuntimeError("Could not query SDL_GetCurrentDisplayMode")
    return display_mode.w - margin_x, display_mode.h -margin_y


class AutoSizeAppWindow:
    _imgui_app_params: ImguiAppParams = None
    _last_seen_imgui_window_size = None
    _computed_sdl_window_size = None
    _sdl_window: SDL_Window = None
    _flag_is_measuring_size: bool = False

    def __init__(self, imgui_app_params: ImguiAppParams, sdl_window: SDL_Window):
        self._imgui_app_params = imgui_app_params
        self._sdl_window = sdl_window

    def _want_autosize(self):
        return self._imgui_app_params.app_window_size[0] == 0 or self._imgui_app_params.app_window_size[1] == 0

    def _move_sdl_window_if_out_of_screen_bounds(self):
        win_pos = (c_int(), c_int())
        SDL_GetWindowPosition(self._sdl_window, win_pos[0], win_pos[1])
        win_size = (c_int(), c_int())
        SDL_GetWindowSize(self._sdl_window, win_size[0], win_size[1])
        screen_size = get_screen_size()

        for dim in range(2):
            if win_pos[dim].value < 0:
                win_pos[dim].value = 0
            if win_pos[dim].value + win_size[dim].value > screen_size[dim]:
                win_pos[dim].value = int(screen_size[dim] / 2. - win_size[dim].value / 2.)
                if win_pos[dim].value < 0:
                    win_pos[dim].value = 0

        SDL_SetWindowPosition(self._sdl_window, win_pos[0], win_pos[1])



    def _force_sdl_window_size(self):
        user_widgets_size = imgui.get_item_rect_size()
        widgets_margin = 30
        screen_size = get_screen_size()

        self._computed_sdl_window_size = (
            min(user_widgets_size[0] + widgets_margin, screen_size[0]),
            min(user_widgets_size[1] + widgets_margin, screen_size[0]))

        r = SDL_SetWindowSize(
            self._sdl_window,
            int(self._computed_sdl_window_size[0]),
            int(self._computed_sdl_window_size[1])
        )

        if not((r is None) or (r == 0)):
            print("Error during SDL_SetWindowSize")

        self._move_sdl_window_if_out_of_screen_bounds()

    @staticmethod
    def _begin_full_size_imgui_window():
        imgui.set_next_window_position(0, 0)
        view_port = imgui.get_main_viewport()
        imgui.set_next_window_position(view_port.pos.x, view_port.pos.y)
        viewport_size = (float(view_port.size[0]), float(view_port.size[1]))
        imgui.set_next_window_size(viewport_size[0], viewport_size[1])
        imgui.begin("Default window", False, imgui.WINDOW_NO_TITLE_BAR | imgui.WINDOW_HORIZONTAL_SCROLLING_BAR)

    def begin(self):
        """
        Reproduces imgui.begin behavior for an imgui window that occupies the whole app window
        and that will try to resize the SDL app window to its content size
        """
        AutoSizeAppWindow._begin_full_size_imgui_window()
        if self._want_autosize() and self._computed_sdl_window_size is None:
            self._flag_is_measuring_size = True
            imgui.begin_group()
        else:
            self._flag_is_measuring_size = False

    def end(self):
        if self._flag_is_measuring_size:
            imgui.end_group()
            self._force_sdl_window_size()
        imgui.end()


def run(gui_function: Callable, imgui_app_params: ImguiAppParams = None):
    if imgui_app_params is None:
        imgui_app_params = ImguiAppParams()

    imgui_app_params.app_shall_exit = False
    running = True

    window, gl_context = _impl_pysdl2_init(imgui_app_params)
    auto_size_app_window = AutoSizeAppWindow(imgui_app_params, window)
    imgui.create_context()
    impl = SDL2Renderer(window)

    power_save.USE_POWER_SAVE = imgui_app_params.use_power_save
    event = SDL_Event()

    while running:

        if power_save.USE_POWER_SAVE:
            _impl_sdl2_wait_for_event_power_save(window)

        while SDL_PollEvent(ctypes.byref(event)) != 0:
            if event.type == SDL_QUIT:
                running = False
                break
            impl.process_event(event)
        impl.process_inputs()

        imgui.new_frame()
        power_save.on_new_frame()

        def gui_handler():
            nonlocal running, auto_size_app_window
            if imgui_app_params.provide_default_window:
                auto_size_app_window.begin()
            gui_function()
            if imgui_app_params.provide_default_window:
                auto_size_app_window.end()

            if imgui_app_params.app_shall_exit:
                running = False
                SDL_DestroyWindow(window)
        gui_handler()

        gl.glClearColor(1., 1., 1., 1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        imgui.render()
        impl.render(imgui.get_draw_data())
        SDL_GL_SwapWindow(window)

    impl.shutdown()
    ctx = imgui.get_current_context()
    if ctx is not None:
        imgui.destroy_context(ctx)
    SDL_GL_DeleteContext(gl_context)
    SDL_DestroyWindow(window)
    SDL_Quit()


def _impl_pysdl2_init(imgui_app_params: ImguiAppParams):
    if SDL_Init(SDL_INIT_EVERYTHING) < 0:
        print("Error:SDL_Init failed! SDL Error: " + SDL_GetError().decode("utf-8"))
        exit(1)

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1)

    if imgui_app_params.gl_multisamples > 0:
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, imgui_app_params.gl_multisamples)

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)

    SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, b"1")
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, b"1")

    window_flags = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)

    app_window_size = imgui_app_params.app_window_size
    if app_window_size[0] == 0 or app_window_size[1] == 0:
        app_window_size = (150, 150)
    window = SDL_CreateWindow(
        imgui_app_params.app_window_title.encode('utf-8'),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        app_window_size[0], app_window_size[1],
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
    return window, gl_context


def _impl_sdl2_wait_for_event_power_save(window):
    window_flags = SDL_GetWindowFlags(window)
    window_is_hidden = window_flags & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED) > 0
    waiting_time = power_save._idle_wait_time if window_is_hidden else power_save._max_wait_before_next_frame_seconds
    if waiting_time > 0:
        waiting_time_ms = int(1000.0 * waiting_time)
        SDL_WaitEventTimeout(None, waiting_time_ms)


def _test_gui_function(params: ImguiAppParams):
    imgui.text("Hello there!")
    if imgui.button("Exit"):
        params.app_shall_exit = True



if __name__ == "__main__":
    params = ImguiAppParams()

    def my_gui():
        _test_gui_function(params)

    params.gui_function = my_gui
    run(params)

