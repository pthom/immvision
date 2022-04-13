cmake_minimum_required(VERSION "3.0")
project(immvision)

set(CMAKE_CXX_STANDARD 17)

option(IMMVISION_BUILD_IMGUI "Build imgui" ON)
option(IMMVISION_BUILD_DEMOS "Build immvision demos" ON)


# Advanced option: use powersave mode for imgui: see PR https://github.com/ocornut/imgui/pull/4076
# This PR was adapted to imgui docking version of March 2022 here: https://github.com/pthom/imgui/tree/docking_powersave
# only use if your version of imgui has this mode!
option(IMMVISION_USE_POWERSAVE "use powersave mode for imgui" OFF)

option(IMMVISION_ACTIVATE_ALL_WARNINGS "Activate all warnings (as error!)" OFF)
option(IMMVISION_BUILD_PYTHON_BINDINGS "Build Python bindings for immvision" OFF)

# For conan, add binary dir to module search path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(src_all_in_one)

if (IMMVISION_BUILD_PYTHON_BINDINGS)
#    message(FATAL_ERROR "PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}")
    add_subdirectory(immvision_pybind)
endif()
