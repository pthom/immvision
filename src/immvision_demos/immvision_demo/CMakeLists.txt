include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/hello_imgui_add_app.cmake)
hello_imgui_add_app(immvision_demo immvision_demo.cpp)
target_link_libraries(immvision_demo PRIVATE immvision)
