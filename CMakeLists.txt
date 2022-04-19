cmake_minimum_required(VERSION 3.15)
project(immvision
    VERSION 0.5.0
    DESCRIPTION "immvision: immediate image debugger and insights"
    HOMEPAGE_URL "https://github.com/pthom/immvision/"
    )

set(CMAKE_CXX_STANDARD 17)

option(IMMVISION_BUILD_PYTHON_BINDINGS "Build Python bindings for immvision" OFF)
if(SKBUILD)
    set(IMMVISION_BUILD_PYTHON_BINDINGS ON CACHE BOOL "")
endif()

if (IMMVISION_BUILD_PYTHON_BINDINGS)
    include("pybind/pybind_cmake.cmake")
endif()

if (NOT SKBUILD)
    option(IMMVISION_BUILD_IMGUI "Build imgui" ON)
    option(IMMVISION_BUILD_DEMOS "Build immvision demos" ON)


    # Advanced option: use powersave mode for imgui: see PR https://github.com/ocornut/imgui/pull/4076
    # This PR was adapted to imgui docking version of March 2022 here: https://github.com/pthom/imgui/tree/docking_powersave
    # only use if your version of imgui has this mode!
    option(IMMVISION_USE_POWERSAVE "use powersave mode for imgui" OFF)

    option(IMMVISION_ACTIVATE_ALL_WARNINGS "Activate all warnings (as error!)" OFF)

    # For conan, add binary dir to module search path
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

    add_subdirectory(external)
    add_subdirectory(src)
    add_subdirectory(src_all_in_one)

endif()