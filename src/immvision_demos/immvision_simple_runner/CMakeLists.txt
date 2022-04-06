option(IMMVISION_DEMO_USE_GLFW "Demos using GLFW" ON)
option(IMMVISION_DEMO_USE_SDL "Demos using SDL" OFF)
option(IMMVISION_DEMO_USE_POWERSAVE "Demos using power saving mode" OFF)

message(WARNING "Forced powersave in src/immvision_demos/immvision_simple_runner/CMakeLists.txt")
set(IMMVISION_DEMO_USE_SDL ON CACHE BOOL "" FORCE)
#set(IMMVISION_DEMO_USE_GLFW OFF CACHE BOOL "" FORCE)
set(IMMVISION_DEMO_USE_POWERSAVE ON CACHE BOOL "" FORCE)


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
    find_package(SDL2 CONFIG REQUIRED)
    target_include_directories(immvision_simple_runner
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/..
        PRIVATE
        ${imgui_dir}/backends
        )
    target_link_libraries(immvision_simple_runner PUBLIC SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static)
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

if (IMMVISION_USE_POWERSAVE)
    target_compile_definitions(immvision_simple_runner PRIVATE IMMVISION_USE_POWERSAVE)
endif()
