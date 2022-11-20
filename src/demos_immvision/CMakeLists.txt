set(IMMVISION_BUILD_IMGUI_SDL_OPENGL3 ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(demo_imm_inspector)
add_subdirectory(demo_imm_mandelbrot)
add_subdirectory(demo_immdebug)
add_subdirectory(demo_immvision)

FILE(COPY ${CMAKE_CURRENT_LIST_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
