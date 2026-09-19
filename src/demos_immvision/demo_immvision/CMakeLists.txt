if (IMMVISION_BUILD_OPENCV_DEMOS)
    set(CMAKE_CXX_STANDARD 20)
    hello_imgui_add_app(demo_immvision demo_immvision.main.cpp)
    # cv::Mat interop is an application choice: define IMMVISION_HAS_OPENCV and link OpenCV here
    target_compile_definitions(demo_immvision PRIVATE IMMVISION_HAS_OPENCV)
    target_link_libraries(demo_immvision PRIVATE immvision opencv_core opencv_imgproc opencv_imgcodecs)
else()
    message(STATUS "Skipping demo_immvision (requires OpenCV: set IMMVISION_BUILD_OPENCV_DEMOS)")
endif()
