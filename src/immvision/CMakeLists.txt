file(GLOB_RECURSE sources *.h *.cpp)
if (NOT IMMVISION_FIX_MISSING_OPENCV_CONFIG)
    find_package(OpenCV CONFIG REQUIRED)
endif()

set(target_name immvision)
add_library(${target_name} ${sources})
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    opencv_core opencv_imgproc opencv_highgui
    )

