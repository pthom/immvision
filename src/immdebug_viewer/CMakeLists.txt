set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
fetch_hello_imgui() # see cmake/fetch_hello_imgui.cmake
hello_imgui_add_app(immdebug_viewer immdebug_viewer.cpp single_instance_app.cpp single_instance_app.h)
target_link_libraries(immdebug_viewer PRIVATE immdebug immvision)
if (UNIX AND NOT APPLE)
    target_link_libraries(immdebug_viewer PRIVATE pthread)
endif()
