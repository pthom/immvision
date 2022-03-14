set(IMMVISION_BUILD_IMGUI_SDL_OPENGL3 ON)

if (IMMVISION_BUILD_IMGUI_SDL_OPENGL3)
    add_subdirectory(imgui_sdl_opengl3_runner)
    add_subdirectory(immvision_demo)
endif()
