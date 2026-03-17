file(GLOB_RECURSE sources *.h *.cpp)

set(target_name immvision)
add_library(${target_name} ${sources})
if (IMMVISION_SERIALIZE_JSON)
    target_compile_options(${target_name} PUBLIC -DIMMVISION_SERIALIZE_JSON)
endif()
target_include_directories(${target_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

# OpenCV is optional: when found, enable cv::Mat interop via IMMVISION_HAS_OPENCV
if (IMMVISION_HAS_OPENCV)
    target_compile_definitions(${target_name} PUBLIC IMMVISION_HAS_OPENCV)
    target_link_libraries(${target_name} PUBLIC opencv_core)
endif()

# stb_image and stb_image_write: when hello_imgui is available it already provides
# the implementations; in standalone mode, immvision compiles its own.
if(NOT TARGET hello_imgui)
    target_compile_definitions(${target_name} PRIVATE
        IMMVISION_PROVIDE_STB_IMAGE
        IMMVISION_PROVIDE_STB_IMAGE_WRITE)
endif()

target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    )
