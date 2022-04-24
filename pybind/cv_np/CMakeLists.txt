cmake_minimum_required(VERSION 3.4)
project(cv_np LANGUAGES CXX)

find_package(OpenCV REQUIRED)

#
# Library cv_np_lib that you can link to other projects
#
file(GLOB sources *.h *.cpp)
pybind11_add_module(cv_np_lib OBJECT ${sources})
target_link_libraries(cv_np_lib PUBLIC opencv_core)
target_include_directories(cv_np_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)


#
# Standalone module cv_np that you can import into python
#
pybind11_add_module(cv_np  main/cv_np_main.cpp)
target_link_libraries(cv_np PRIVATE cv_np_lib)

# Copy the .so or .pyd module into this dir, to facilitate the tests
add_custom_command(
    TARGET cv_np POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cv_np> ${CMAKE_CURRENT_LIST_DIR}
)
