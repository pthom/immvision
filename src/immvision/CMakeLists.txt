file(GLOB_RECURSE sources *.h *.cpp)
find_package(OpenCV REQUIRED)

set(target_name immvision)
add_library(${target_name} ${sources})
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs
    )

add_custom_command(
    TARGET immvision
    PRE_BUILD
    COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/../../pybind/code_parser/code_autogenerator.py
)
