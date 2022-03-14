cmake_minimum_required(VERSION "3.0")
project(immvision)

set(CMAKE_CXX_STANDARD 17)

option(IMMVISION_BUILD_IMGUI "Build imgui" ON)
option(IMMVISION_BUILD_DEMOS "Build immvision demos" ON)

find_package(OpenCV CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(external)
