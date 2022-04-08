option(IMMVISION_DEMO_USE_GLFW "Demos using GLFW" OFF)
option(IMMVISION_DEMO_USE_SDL "Demos using SDL" ON)
option(IMMVISION_DEMO_USE_POWERSAVE "Demos using power saving mode" OFF)

# KK Remove ?
set(IMMVISION_DEMO_USE_SDL ON CACHE BOOL "" FORCE)
#set(IMMVISION_DEMO_USE_GLFW OFF CACHE BOOL "" FORCE)


set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../../external/imgui)

set(sources
    ${imgui_dir}/backends/imgui_impl_opengl3.h
    ${imgui_dir}/backends/imgui_impl_opengl3.cpp
    immvision_simple_runner.h
    )
if (IMMVISION_DEMO_USE_GLFW)
    set(sources ${sources}
        ${imgui_dir}/backends/imgui_impl_glfw.h
        ${imgui_dir}/backends/imgui_impl_glfw.cpp
        immvision_simple_runner_glfw.cpp
        )
endif()
if (IMMVISION_DEMO_USE_SDL)
    set(sources ${sources}
        ${imgui_dir}/backends/imgui_impl_sdl.h
        ${imgui_dir}/backends/imgui_impl_sdl.cpp
        immvision_simple_runner_sdl.cpp
        )
endif()

add_library(immvision_simple_runner ${sources})
target_link_libraries(immvision_simple_runner
    PUBLIC imgui
    PRIVATE immvision_gl_loader)

if (IMMVISION_DEMO_USE_SDL)
    find_package(SDL2 REQUIRED)
    target_include_directories(immvision_simple_runner
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/..
        PRIVATE
        ${imgui_dir}/backends
        )
    if (APPLE)
        target_link_libraries(immvision_simple_runner PUBLIC SDL2::SDL2)
    else()
        if (NOT EMSCRIPTEN)
            target_link_libraries(immvision_simple_runner PUBLIC SDL2 SDL2main)
         else()
            target_link_libraries(immvision_simple_runner PUBLIC SDL2)
        endif()
    endif()
    target_include_directories(immvision_simple_runner  PUBLIC ${SDL2_INCLUDE_DIRS})
endif()

if (IMMVISION_DEMO_USE_GLFW)
    find_package(glfw3 CONFIG REQUIRED)
    target_include_directories(immvision_simple_runner
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/..
        PRIVATE
        ${imgui_dir}/backends
        )
    target_link_libraries(immvision_simple_runner PUBLIC glfw)
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(immvision_simple_runner PRIVATE dl)
endif()

if (IMMVISION_USE_POWERSAVE)
    target_compile_definitions(immvision_simple_runner PRIVATE IMMVISION_USE_POWERSAVE)
endif()
