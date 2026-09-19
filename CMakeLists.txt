cmake_minimum_required(VERSION 3.21)
project(immvision
    VERSION 1.92.601
    DESCRIPTION "immvision: immediate image debugger and insights"
    HOMEPAGE_URL "https://github.com/pthom/immvision/"
    )


###############################################################################
# Build Options
###############################################################################

#------------------------------------------------------------------------------
# Note about OpenCV
# ImmVision does not depend on OpenCV, and never links it.
# Applications which want cv::Mat interop should define IMMVISION_HAS_OPENCV
# and link OpenCV themselves (the conversions are header-only), e.g.
#     find_package(OpenCV REQUIRED)
#     target_compile_definitions(my_app PRIVATE IMMVISION_HAS_OPENCV)
#     target_link_libraries(my_app PRIVATE opencv_core)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# IMMVISION_BUILD_OPENCV_DEMOS
# if ON, build the demos and tests that require OpenCV (OpenCV must then be findable,
# either because it is installed, or via IMMVISION_FETCH_OPENCV)
option(IMMVISION_BUILD_OPENCV_DEMOS "Build immvision demos and tests that require OpenCV" OFF)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# IMMVISION_FETCH_OPENCV
# Convenience only: if ON, and if OpenCV is not found, fetch and build a very minimal version
# of OpenCV in the build directory, so that a later find_package(OpenCV) succeeds.
# (extremely minimal: no highgui, no video capture, etc.)
# It does not change how ImmVision is built.
#
# For a complete version of OpenCV, it is preferable to use your own version
option(IMMVISION_FETCH_OPENCV "Fetch and build a minimal OpenCV if not found" OFF)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# IMMVISION_FETCH_OPENCV_FULL
# if ON, the fetched build of OpenCV will be less minimal, and use OpenCV defaults
#
# For a complete version of OpenCV use your own version
option(IMMVISION_FETCH_OPENCV_FULL "Fetched OpenCV uses OpenCV defaults instead of a minimal build" OFF)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# IMMVISION_SERIALIZE_JSON
# if ON, the ImageParams class will be able to serialize to JSON
# This requires nlohmann/json
option(IMMVISION_SERIALIZE_JSON OFF)
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# IMMVISION_BUILD_DEMOS
# if ON, build demos (this will fetch hello_imgui at configure time)
option(IMMVISION_BUILD_DEMOS "Build immvision demos" ${PROJECT_IS_TOP_LEVEL})
#------------------------------------------------------------------------------

option(IMMVISION_BUILD_TESTS "Build immvision tests" ${PROJECT_IS_TOP_LEVEL})


#------------------------------------------------------------------------------
# IMMVISION_BUILD_VIEWER
# if ON, build immdebug viewer (this will fetch hello_imgui at configure time)
if(PROJECT_IS_TOP_LEVEL)
    option(IMMVISION_BUILD_VIEWER "Build immdebug_viewer" ON)
else()
    option(IMMVISION_BUILD_VIEWER "Build immdebug_viewer" OFF)
endif()
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# IMMVISION_ACTIVATE_ALL_WARNINGS
# if ON, all warnings are considered as errors
option(IMMVISION_ACTIVATE_ALL_WARNINGS "Activate all warnings (as error!)" OFF)
#------------------------------------------------------------------------------


###############################################################################
# Build process
###############################################################################
set(CMAKE_CXX_STANDARD 17)

# OpenCV (not used by the immvision library itself, see note in the options above)
if (IMMVISION_FETCH_OPENCV)
    include(cmake/fetch_opencv.cmake)
    immvision_fetch_opencv()  # only fetches if find_package(OpenCV) fails
endif()
if (IMMVISION_BUILD_OPENCV_DEMOS)
    find_package(OpenCV QUIET)
    if (NOT OpenCV_FOUND)
        message(FATAL_ERROR "IMMVISION_BUILD_OPENCV_DEMOS requires OpenCV: install it, set OpenCV_DIR, or pass -DIMMVISION_FETCH_OPENCV=ON")
    endif()
endif()


set(IMMVISION_VERSION ${PROJECT_VERSION})
add_compile_definitions(IMMVISION_VERSION="${PROJECT_VERSION}")

include(cmake/fetch_hello_imgui.cmake)

if (IMMVISION_BUILD_TESTS)
    enable_testing()
endif()
add_subdirectory(src)
