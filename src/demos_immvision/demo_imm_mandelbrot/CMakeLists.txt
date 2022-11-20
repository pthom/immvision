set(CMAKE_CXX_STANDARD 20)
if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/hello_imgui_add_app.cmake)
hello_imgui_add_app(demo_imm_mandelbrot demo_imm_mandelbrot.main.cpp)
target_link_libraries(demo_imm_mandelbrot PRIVATE immvision)
# target_include_directories(demo_imm_mandelbrot PRIVATE ${CMAKE_SOURCE_DIR}/external/fplus)
