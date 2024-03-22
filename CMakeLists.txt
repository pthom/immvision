cmake_minimum_required(VERSION 3.21)
project(immvision
    VERSION 0.8.0
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
if(PROJECT_IS_TOP_LEVEL)
    option(IMMVISION_BUILD_DEMOS "Build immvision demos" ON)
else()
    option(IMMVISION_BUILD_DEMOS "Build immvision demos" OFF)
endif()
#------------------------------------------------------------------------------

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

#------------------------------------------------------------------------------
# IMMVISION_OPENCV_CONAN
# For conan, add binary dir to module search path
# this way, if you run
#     conan install ../conanfile_opencv.txt --build=missing
# in the build dir, you can use OpenCV provided by conan
option(IMMVISION_OPENCV_CONAN "Use conan to get opencv" ON)
if (IMMVISION_OPENCV_CONAN)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
endif()
#------------------------------------------------------------------------------

###############################################################################
# Build process
###############################################################################
set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_opencv.cmake)
immvision_find_opencv()
find_package(OpenCV REQUIRED)  # Will try to find OpenCV (if IMMVISION_FETCH_OPENCV, will also download and build it)

set(IMMVISION_VERSION ${PROJECT_VERSION})
add_compile_definitions(IMMVISION_VERSION="${PROJECT_VERSION}")

include(cmake/fetch_hello_imgui.cmake)

add_subdirectory(src)

if (IMMVISION_BUILD_ALL_IN_ONE)
    add_subdirectory(src_all_in_one)
endif()
