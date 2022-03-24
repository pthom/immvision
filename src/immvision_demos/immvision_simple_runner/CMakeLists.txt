set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../../external/imgui)
add_library(immvision_simple_runner
    ${imgui_dir}/backends/imgui_impl_opengl3.h
    ${imgui_dir}/backends/imgui_impl_opengl3.cpp
    ${imgui_dir}/backends/imgui_impl_sdl.h
    ${imgui_dir}/backends/imgui_impl_sdl.cpp
    immvision_simple_runner.cpp
    immvision_simple_runner.h
)

find_package(SDL2 CONFIG REQUIRED)
target_include_directories(immvision_simple_runner
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
    PRIVATE
    ${imgui_dir}/backends
    )
target_link_libraries(immvision_simple_runner PUBLIC SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static)
target_link_libraries(immvision_simple_runner PUBLIC imgui)
target_link_libraries(immvision_simple_runner PUBLIC immvision_gl_loader)
