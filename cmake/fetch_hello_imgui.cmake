macro(fetch_hello_imgui)
    # Download hello_imgui if not available as a target
    if(NOT TARGET hello_imgui)
        set(HELLOIMGUI_WITH_SDL OFF CACHE BOOL "" FORCE)
        set(HELLOIMGUI_WITH_GLFW ON CACHE BOOL "" FORCE)
        include(FetchContent)
        FetchContent_Declare(
            hello_imgui
            GIT_REPOSITORY https://github.com/pthom/hello_imgui.git
            # Enter the desired git tag below
            # GIT_TAG 0.8.0
        )
        FetchContent_MakeAvailable(hello_imgui)

        # Make cmake function `hello_imgui_add_app` available
        list(APPEND CMAKE_MODULE_PATH ${HELLOIMGUI_CMAKE_PATH})
        include(hello_imgui_add_app)
    endif()
endmacro()
