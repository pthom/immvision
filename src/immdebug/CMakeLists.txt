file(GLOB_RECURSE sources *.h *.cpp)

set(target_name immdebug)
add_library(${target_name} ${sources})
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if (NOT IMMVISION_FIX_MISSING_OPENCV_CONFIG)
    find_package(OpenCV CONFIG REQUIRED)
endif()
target_link_libraries(${target_name} opencv_core)
