file(GLOB_RECURSE sources *.h *.cpp)
find_package(OpenCV REQUIRED)

set(target_name immvision)
add_library(${target_name} ${sources})
if (IMMVISION_SERIALIZE_JSON)
    target_compile_options(${target_name} PUBLIC -DIMMVISION_SERIALIZE_JSON)
endif()
target_compile_definitions(${target_name} PUBLIC IMMVISION_HAS_OPENCV)
target_include_directories(${target_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

# stb_image_write: when hello_imgui is available it already provides the implementation;
# in standalone mode, immvision compiles its own.
if(NOT TARGET hello_imgui)
    target_compile_definitions(${target_name} PRIVATE IMMVISION_PROVIDE_STB_IMAGE_WRITE)
endif()

target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    opencv_core
    opencv_imgproc
    opencv_imgcodecs  # immvision no longer uses imgcodecs, but downstream demos may
    )
