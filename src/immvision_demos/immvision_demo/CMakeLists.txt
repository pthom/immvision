set(CMAKE_CXX_STANDARD 20)
hello_imgui_add_app(immvision_demo immvision_demo.cpp)
target_link_libraries(immvision_demo PRIVATE immvision)
