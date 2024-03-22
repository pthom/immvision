file(GLOB_RECURSE sources *.h *.cpp)
find_package(OpenCV REQUIRED)

set(target_name immvision)
add_library(${target_name} ${sources})
if (IMMVISION_SERIALIZE_JSON)
    target_compile_options(${target_name} PUBLIC -DIMMVISION_SERIALIZE_JSON)
endif()
target_include_directories(${target_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    )

