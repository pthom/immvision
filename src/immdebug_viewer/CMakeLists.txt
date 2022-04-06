set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_executable(immdebug_viewer immdebug_viewer.cpp single_instance_app.cpp single_instance_app.h)
target_link_libraries(immdebug_viewer PRIVATE immdebug immvision immvision_simple_runner)
