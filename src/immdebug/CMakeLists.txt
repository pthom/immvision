file(GLOB_RECURSE sources *.h *.cpp)

set(target_name immdebug)
add_library(${target_name} ${sources})
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_package(OpenCV REQUIRED)
target_link_libraries(${target_name} PUBLIC opencv_core)
