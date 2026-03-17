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
# IMMVISION_FETCH_OPENCV
# if ON, we will fetch a build a very minimal version of OpenCV in the build directory
# This version is extremely minimal: no highgui, no video capture, no img_proc, etc.
#
# For a complete version of OpenCV, it is preferable to use your own version
option(IMMVISION_FETCH_OPENCV OFF)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# IMMVISION_FETCH_OPENCV_FULL
# if ON, the fetched build of OpenCV will be less minimal, and use OpenCV defaults
#
# For a complete version of OpenCV use your own version
option(IMMVISION_FETCH_OPENCV_FULL OFF)
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
# IMMVISION_BUILD_ALL_IN_ONE
# if ON, an all in one library will be built from amalgamated sources at src_all_in_one/
option(IMMVISION_BUILD_ALL_IN_ONE OFF)
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

# OpenCV
# =======
# Immvision will provide cv::Mat interop if OpenCV is found.
# - OpenCV is optional
# - if IMMVISION_FETCH_OPENCV is ON, and OpenCV is not found, we will fetch and build a minimal version of OpenCV in the build directory.
find_package(OpenCV QUIET)
if (IMMVISION_FETCH_OPENCV)
    if (NOT OpenCV_FOUND)
        message(STATUS "OpenCV not found, fetching minimal OpenCV...")
        include(cmake/fetch_opencv.cmake)
        immvision_fetch_opencv()
    endif()
endif()
if (OpenCV_FOUND)
    set(IMMVISION_HAS_OPENCV ON CACHE BOOL "" FORCE)
    message(STATUS "OpenCV found (${OpenCV_VERSION}) — cv::Mat interop enabled")
else()
    set(IMMVISION_HAS_OPENCV OFF CACHE BOOL "" FORCE)
    message(STATUS "OpenCV not found — building without cv::Mat interop")
endif()


set(IMMVISION_VERSION ${PROJECT_VERSION})
add_compile_definitions(IMMVISION_VERSION="${PROJECT_VERSION}")

include(cmake/fetch_hello_imgui.cmake)

add_subdirectory(src)

if (IMMVISION_BUILD_ALL_IN_ONE)
    add_subdirectory(src_all_in_one)
endif()
