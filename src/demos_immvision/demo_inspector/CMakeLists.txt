if (IMMVISION_BUILD_OPENCV_DEMOS)
    include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/hello_imgui_add_app.cmake)
    hello_imgui_add_app(demo_inspector demo_inspector.main.cpp)
    # cv::Mat interop is an application choice: define IMMVISION_HAS_OPENCV and link OpenCV here
    target_compile_definitions(demo_inspector PRIVATE IMMVISION_HAS_OPENCV)
    target_link_libraries(demo_inspector PRIVATE immvision opencv_core opencv_imgproc opencv_imgcodecs)
else()
    message(STATUS "Skipping demo_inspector (requires OpenCV: set IMMVISION_BUILD_OPENCV_DEMOS)")
endif()
