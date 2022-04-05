include(${CMAKE_SOURCE_DIR}/external/hello_imgui/hello_imgui_cmake/hello_imgui_add_app.cmake)
hello_imgui_add_app(inspector_demo inspector_demo.cpp)
target_link_libraries(inspector_demo PRIVATE immvision)
