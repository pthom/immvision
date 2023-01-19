include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/hello_imgui_add_app.cmake)
hello_imgui_add_app(demo_inspector demo_inspector.main.cpp)
target_link_libraries(demo_inspector PRIVATE immvision)
# target_link_libraries(demo_inspector PRIVATE immvision_all_in_one)
