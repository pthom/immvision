cmake_minimum_required(VERSION 3.15...3.22)

project(immvision VERSION "0.0.1")

set(CMAKE_CXX_STANDARD 17)

if(SKBUILD OR IMMVISION_BUILD_PYTHON_BINDINGS)
  # Scikit-Build does not add your site-packages to the search path
  # automatically, so we need to add it _or_ the pybind11 specific directory
  # here.
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE _tmp_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
  list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
endif()

# Now we can find pybind11
find_package(pybind11 CONFIG REQUIRED)

set(IMMVISION_PYBIND_BIN_MODULE_NAME cpp_immvision)
file(GLOB_RECURSE sources_immvision_pybind src/*.cpp src/*.h)
pybind11_add_module(${IMMVISION_PYBIND_BIN_MODULE_NAME} MODULE ${sources_immvision_pybind})

#
# Define specific settings for immvision:
# do not use targets from the main project, since pip install will run from this subfolder!
#
# 1. Add sources
set(immvision_src_DIRS ${CMAKE_CURRENT_LIST_DIR}/../src/immvision)
file(GLOB_RECURSE sources_immvision ${immvision_src_DIRS}/*.h ${immvision_src_DIRS}/*.cpp)
target_sources(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${sources_immvision})
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${immvision_src_DIRS}/..)

# 2. Add include dirs
set(default_include_dir ${CMAKE_CURRENT_LIST_DIR}/../external/vcpkg/installed/x64-osx/include)
# 2.1 Find OpenCV include dir
find_package(OpenCV)
if (NOT OpenCV_FOUND)
  message(WARN "find_package(OpenCV) failed, using default (and probably bad) location:
          set(OpenCV_INCLUDE_DIRS ${default_include_dir}) ")
  set(OpenCV_INCLUDE_DIRS ${default_include_dir})
else()
  # Hack for broken vcpkg naming
  set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} ${opencv_INCLUDE_DIRS})
endif()

# 2.2 imgui include dir
set(imgui_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../external/imgui")
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS})

# 3. Compile definitions and install
target_compile_definitions(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE
    VERSION_INFO=${PROJECT_VERSION}
    IMMVISION_PYBIND_BIN_MODULE_NAME=${IMMVISION_PYBIND_BIN_MODULE_NAME}
    IMMVISION_BUILDING_PYBIND
    )
install(TARGETS ${IMMVISION_PYBIND_BIN_MODULE_NAME} DESTINATION .)

#
# Post build: deploy library to ${CMAKE_BINARY_DIR}/_pybind/,
# so that we can use it as a python package name immvision
message(WARNING "CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(
    TARGET ${IMMVISION_PYBIND_BIN_MODULE_NAME}
    POST_BUILD
    COMMAND
    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/src/postbuild_deploy.py
      ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}
    )


#
# immvision_debug_pybind
#
find_package(pybind11 CONFIG REQUIRED)
add_executable(pybind_debug_helper debug_helper/pybind_debug_helper.cpp)
target_link_libraries(pybind_debug_helper PRIVATE pybind11::embed)
target_include_directories(pybind_debug_helper PRIVATE ${pybind11_INCLUDE_DIRS})
