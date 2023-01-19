cmake_minimum_required(VERSION 3.21)
project(immvision
    VERSION 0.8.0
    DESCRIPTION "immvision: immediate image debugger and insights"
    HOMEPAGE_URL "https://github.com/pthom/immvision/"
    )

# OpenCV is required: if IMMVISION_FETCH_OPENCV is ON,
# we will fetch a build a very minimal version of OpenCV in the build directory
# This version is extremely minimal: no highgui, no video capture, etc.
#  Feel free to use your own version of OpenCV in C++)
option(IMMVISION_FETCH_OPENCV OFF)

option(IMMVISION_BUILD_ALL_IN_ONE OFF)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_SOURCE_DIR}/find_opencv.cmake)
immvision_find_opencv()
find_package(OpenCV REQUIRED)

set(IMMVISION_VERSION ${PROJECT_VERSION})
add_compile_definitions(IMMVISION_VERSION="${PROJECT_VERSION}")

if(PROJECT_IS_TOP_LEVEL)
    option(IMMVISION_BUILD_DEMOS "Build immvision demos" ON)
    option(IMMVISION_ACTIVATE_ALL_WARNINGS "Activate all warnings (as error!)" OFF)
else()
    option(IMMVISION_BUILD_DEMOS "Build immvision demos" OFF)
endif()


if (IMMVISION_BUILD_DEMOS)
    # Download hello_imgui if not available as a target
    if(NOT TARGET hello_imgui)
        set(HELLOIMGUI_WITH_SDL OFF CACHE BOOL "" FORCE)
        set(HELLOIMGUI_WITH_GLFW ON CACHE BOOL "" FORCE)
        include(FetchContent)
        FetchContent_Declare(
            hello_imgui
            GIT_REPOSITORY https://github.com/pthom/hello_imgui.git
            # Enter the desired git tag below
            # GIT_TAG 0.8.0
        )
        FetchContent_MakeAvailable(hello_imgui)
    endif()

    # Make cmake function `hello_imgui_add_app` available
    list(APPEND CMAKE_MODULE_PATH ${HELLOIMGUI_CMAKE_PATH})
    include(hello_imgui_add_app)
endif()

add_subdirectory(src)

if (IMMVISION_BUILD_ALL_IN_ONE)
    add_subdirectory(src_all_in_one)
endif()
