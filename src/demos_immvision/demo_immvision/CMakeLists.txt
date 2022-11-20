set(CMAKE_CXX_STANDARD 20)
hello_imgui_add_app(demo_immvision demo_immvision.main.cpp)
target_link_libraries(demo_immvision PRIVATE immvision)
