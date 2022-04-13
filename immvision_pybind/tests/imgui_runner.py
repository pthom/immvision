# -*- coding: utf-8 -*-
from sdl2 import *
import ctypes
import OpenGL.GL as gl

import imgui
from imgui.integrations.sdl2 import SDL2Renderer
from dataclasses import dataclass
import typing


WINDOWS_TASKBAR_HEIGHT = 60
WINDOWS_TITLE_HEIGHT = 32
WINDOWED_FULL_SCREEN_X_MARGIN = 20


@dataclass
class ImguiAppParams:
    # Main function that will display the gui
    gui_function: typing.Callable = None
    # Title and Size
    app_window_title: str = "ImGui Application"
    app_window_size: (int, int) = (1280, 720)
    # Provide a default full *imgui* window inside the app
    provide_default_window: bool = True
    # Make the App window almost full screen?
    windowed_full_screen: bool = False
    # Change this bool to True when you want to exit
    app_shall_exit: bool = False


def run(imgui_app_params: ImguiAppParams):
    window, gl_context = _impl_pysdl2_init(imgui_app_params)
    imgui.create_context()
    impl = SDL2Renderer(window)

    running = True
    event = SDL_Event()
    while running:
        while SDL_PollEvent(ctypes.byref(event)) != 0:
            if event.type == SDL_QUIT:
                running = False
                break
            impl.process_event(event)
        impl.process_inputs()

        imgui.new_frame()

        def gui_handler():
            nonlocal running
            if imgui_app_params.provide_default_window:
                imgui.set_next_window_position(0, 0)
                imgui.set_next_window_size(imgui_app_params.app_window_size[0], imgui_app_params.app_window_size[1])
                imgui.begin("Default window")

            imgui_app_params.gui_function()
            if imgui_app_params.provide_default_window:
                imgui.end()

            if imgui_app_params.app_shall_exit:
                running = False
        gui_handler()

        gl.glClearColor(1., 1., 1., 1)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        imgui.render()
        impl.render(imgui.get_draw_data())
        SDL_GL_SwapWindow(window)

    impl.shutdown()
    SDL_GL_DeleteContext(gl_context)
    SDL_DestroyWindow(window)
    SDL_Quit()


def _impl_pysdl2_init(imgui_app_params: ImguiAppParams):
    if SDL_Init(SDL_INIT_EVERYTHING) < 0:
        print("Error: SDL could not initialize! SDL Error: " + SDL_GetError().decode("utf-8"))
        exit(1)

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)

    SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, b"1")
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, b"1")

    window = SDL_CreateWindow(imgui_app_params.app_window_title.encode('utf-8'),
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              imgui_app_params.app_window_size[0], imgui_app_params.app_window_size[1],
                              SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE)

    if window is None:
        print("Error: Window could not be created! SDL Error: " + SDL_GetError().decode("utf-8"))
        exit(1)

    gl_context = SDL_GL_CreateContext(window)
    if gl_context is None:
        print("Error: Cannot create OpenGL Context! SDL Error: " + SDL_GetError().decode("utf-8"))
        exit(1)

    SDL_GL_MakeCurrent(window, gl_context)
    if SDL_GL_SetSwapInterval(1) < 0:
        print("Warning: Unable to set VSync! SDL Error: " + SDL_GetError().decode("utf-8"))
        exit(1)

    return window, gl_context


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

