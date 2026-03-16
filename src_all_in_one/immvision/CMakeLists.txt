find_package(OpenCV QUIET)

set(target_name immvision_all_in_one)
add_library(${target_name} immvision.cpp immvision.h)
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if (IMMVISION_HAS_OPENCV)
    target_compile_definitions(${target_name} PUBLIC IMMVISION_HAS_OPENCV)
    target_link_libraries(${target_name} PUBLIC opencv_core)
endif()

# stb_image and stb_image_write: standalone mode compiles its own implementations
if(NOT TARGET hello_imgui)
    target_compile_definitions(${target_name} PRIVATE
        IMMVISION_PROVIDE_STB_IMAGE
        IMMVISION_PROVIDE_STB_IMAGE_WRITE)
endif()

target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    )
