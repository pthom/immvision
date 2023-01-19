set(IMMVISION_BUILD_IMGUI_SDL_OPENGL3 ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

fetch_hello_imgui() # see cmake/fetch_hello_imgui.cmake
add_subdirectory(demo_inspector)
add_subdirectory(demo_immdebug)
add_subdirectory(demo_immvision)

FILE(COPY ${CMAKE_CURRENT_LIST_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
