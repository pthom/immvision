cmake_minimum_required(VERSION "3.0")
project(immvision)

set(CMAKE_CXX_STANDARD 17)

option(IMMVISION_BUILD_IMGUI "Build imgui" ON)
option(IMMVISION_BUILD_DEMOS "Build immvision demos" ON)


# Advanced option: use powersave mode for imgui: see PR https://github.com/ocornut/imgui/pull/4076
# This PR was adapted to imgui docking version of March 2022 here: https://github.com/pthom/imgui/tree/docking_powersave
# only use if your version of imgui has this mode!
option(IMMVISION_USE_POWERSAVE "use powersave mode for imgui" OFF)

# For conan, add binary dir to module search path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(external)
add_subdirectory(src)