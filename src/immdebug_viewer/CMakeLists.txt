set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_executable(immdebug_viewer immdebug_viewer.cpp single_instance_app.cpp single_instance_app.h)

find_package(Boost COMPONENTS system REQUIRED)
target_link_libraries(immdebug_viewer PRIVATE immdebug immvision immvision_simple_runner)

# ./vcpkg install boost-interprocess
find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(immdebug_viewer PRIVATE Boost::boost)
