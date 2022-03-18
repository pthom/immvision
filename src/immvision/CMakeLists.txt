if (APPLE)
    file(GLOB_RECURSE sources *.h *.cpp *.c *.mm)
else()
    file(GLOB_RECURSE sources *.h *.cpp *.c)
endif()


find_package(OpenCV CONFIG REQUIRED)

set(target_name immvision)
add_library(${target_name} ${sources})
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${target_name} PUBLIC
    imgui
    immvision_gl_loader
    opencv_core opencv_imgproc
    )
