cmake_minimum_required(VERSION 3.15...3.22)

project(immvision VERSION "0.0.1")

set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED PYTHON_EXECUTABLE)
  set(PYTHON_EXECUTABLE "python")
endif()

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

#
# Main target: cpp_imvision python module
#
set(IMMVISION_PYBIND_BIN_MODULE_NAME cpp_immvision)
file(GLOB_RECURSE sources_immvision_pybind src_pybind/*.cpp src_pybind/*.h)
pybind11_add_module(${IMMVISION_PYBIND_BIN_MODULE_NAME} MODULE ${sources_immvision_pybind})
# Define specific settings for immvision (do not use targets from the main project, since pip install will run from this subfolder!)
# 1. Add sources
if (IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../src/immvision")
  set(immvision_src_DIRS "${CMAKE_CURRENT_LIST_DIR}/../src/immvision")
else()
  # When building the pip package, we are in a bare copy of this folder
  # and ".." does not point inside the repository.
  # So, we copying imgui and immvision to src_copy_cpp/
  set(immvision_src_DIRS "${CMAKE_CURRENT_LIST_DIR}/src_copy_cpp/immvision")
  message(WARNING "Trying to use immvision_src_DIRS=${immvision_src_DIRS}")
endif()
if (NOT IS_DIRECTORY ${immvision_src_DIRS})
  message(FATAL_ERROR "Cannot set immvision_src_DIRS")
endif()
file(GLOB_RECURSE sources_immvision ${immvision_src_DIRS}/*.h ${immvision_src_DIRS}/*.cpp)
target_sources(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${sources_immvision})
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${immvision_src_DIRS}/..)

#
# Compile definitions and install
target_compile_definitions(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE
    VERSION_INFO=${PROJECT_VERSION}
    IMMVISION_PYBIND_BIN_MODULE_NAME=${IMMVISION_PYBIND_BIN_MODULE_NAME}
    IMMVISION_BUILDING_PYBIND
    )
install(TARGETS ${IMMVISION_PYBIND_BIN_MODULE_NAME} DESTINATION .)


#
# Link with OpenCV
#
find_package(OpenCV)
if (NOT OpenCV_FOUND)
  set(default_opencv_include_dir ${CMAKE_CURRENT_LIST_DIR}/../external/vcpkg/installed/x64-osx/include)
  message(WARN "find_package(OpenCV) failed, using default (and probably bad) location:
          set(OpenCV_INCLUDE_DIRS ${default_include_dir}) ")
  set(OpenCV_INCLUDE_DIRS ${default_include_dir})
else()
  # Hack for broken vcpkg naming
  set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} ${opencv_INCLUDE_DIRS})
endif()
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
# Link
target_link_libraries(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)


#
# Link with imgui
#
# 2.1 imgui source dir
if (IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../external/imgui")
  set(imgui_source_dir "${CMAKE_CURRENT_LIST_DIR}/../external/imgui")
else()
  # When building the pip package, sometimes we are in a bare copy of this folder
  # and ".." does not point inside the repository.
  # So, the pip.yml Github workflow starts by copying imgui and immvision to src_copy_cpp/
  set(imgui_source_dir "${CMAKE_CURRENT_LIST_DIR}/src_copy_cpp/imgui")
  message(WARNING "Trying to use imgui_source_dir=${imgui_source_dir}")
endif()
if (NOT IS_DIRECTORY ${imgui_source_dir})
  message(FATAL_ERROR "Cannot set imgui_source_dir")
endif()
file(GLOB imgui_sources ${imgui_source_dir}/*.h ${imgui_source_dir}/*.cpp)
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${imgui_source_dir})



# Methode sans rien linker
#  ==> undefined symbol: _ZN5ImGui11PopStyleVarEi

# Methode avec link statique:
# => les import semblent fonctionner, mais c'est inquietant ?
add_library(imgui_shared_pybind STATIC ${imgui_sources})
target_compile_options(imgui_shared_pybind PRIVATE -fPIC)
target_include_directories(imgui_shared_pybind PUBLIC ${imgui_source_dir})
target_link_libraries(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE imgui_shared_pybind)
install(TARGETS imgui_shared_pybind DESTINATION .)


# Methode avec lib shared classique:
#add_library(imgui_shared_pybind SHARED ${imgui_sources})
#target_include_directories(imgui_shared_pybind PUBLIC ${imgui_source_dir})
#target_link_libraries(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE imgui_shared_pybind)
#install(TARGETS imgui_shared_pybind DESTINATION .)




# Methode avec lib shared pybind:
#  ==> undefined symbol: _ZN5ImGui11PopStyleVarEi
#pybind11_add_module(imgui_shared_pybind SHARED ${imgui_sources})
#target_include_directories(imgui_shared_pybind PUBLIC ${imgui_source_dir})
#target_link_libraries(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE imgui_shared_pybind)
#install(TARGETS imgui_shared_pybind DESTINATION .)


# Methode / Hack link pip imgui on docker
# ==> Semi-ok, avec LD_LIBRARY_PATH avant appel python
# LD_LIBRARY_PATH=/dvp/sources/immvision_pybind/venv_docker/lib/python3.10/site-packages/imgui
#set(imgui_docker_lib_dir "/dvp/sources/immvision_pybind/venv_docker/lib/python3.10/site-packages/imgui/")
#target_link_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${imgui_docker_lib_dir})
#target_link_libraries(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE core internal)
#install(TARGETS imgui_shared_pybind DESTINATION .)



#
# Post build: deploy library to ${CMAKE_BINARY_DIR}/_pybind/,
# so that we can use it as a python package name immvision
add_custom_command(
    TARGET ${IMMVISION_PYBIND_BIN_MODULE_NAME}
    POST_BUILD
    COMMAND
    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/CMakeUtilities.py
      ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} "post_build_deploy"
    )

#
# immvision_debug_pybind
#
find_package(pybind11 CONFIG REQUIRED)
add_executable(pybind_debug_helper debug_helper/pybind_debug_helper.cpp)
target_link_libraries(pybind_debug_helper PRIVATE pybind11::embed)
target_include_directories(pybind_debug_helper PRIVATE ${pybind11_INCLUDE_DIRS})
