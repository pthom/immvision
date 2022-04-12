find_package(OpenCV REQUIRED)

set(target_name immvision_all_in_one)
add_library(${target_name} immvision.cpp immvision.h)
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs
    )
