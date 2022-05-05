set(CMAKE_CXX_STANDARD 20)
if (UNIX AND NOT APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/hello_imgui_add_app.cmake)
hello_imgui_add_app(mandelbrot_demo_cpp mandelbrot_demo_cpp.cpp)
target_link_libraries(mandelbrot_demo_cpp PRIVATE immvision)
target_include_directories(mandelbrot_demo_cpp PRIVATE ${CMAKE_SOURCE_DIR}/external/fplus)
