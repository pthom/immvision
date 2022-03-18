set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../../external/imgui)
add_library(imgui_sdl_opengl3
    ${imgui_dir}/backends/imgui_impl_opengl3.h
    ${imgui_dir}/backends/imgui_impl_opengl3.cpp
    ${imgui_dir}/backends/imgui_impl_sdl.h
    ${imgui_dir}/backends/imgui_impl_sdl.cpp
    imgui_sdl_opengl3_runner.cpp
    imgui_sdl_opengl3_runner.h
)

find_package(SDL2 CONFIG REQUIRED)
target_include_directories(imgui_sdl_opengl3 
    PUBLIC 
    ${imgui_dir}/backends
    ${CMAKE_CURRENT_LIST_DIR}
    )
target_link_libraries(imgui_sdl_opengl3 PUBLIC SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static)
target_link_libraries(imgui_sdl_opengl3 PUBLIC imgui)

target_link_libraries(imgui_sdl_opengl3 PUBLIC immvision_gl_loader)
