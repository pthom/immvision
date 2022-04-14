include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/hello_imgui_add_app.cmake)
hello_imgui_add_app(inspector_demo inspector_demo.cpp)
target_link_libraries(inspector_demo PRIVATE immvision)
# target_link_libraries(inspector_demo PRIVATE immvision_all_in_one)
