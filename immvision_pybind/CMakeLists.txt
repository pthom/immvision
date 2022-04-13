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

set(target_name _core)
pybind11_add_module(${target_name} MODULE src/main.cpp)

#
# Define specific settings for immvision:
# do not use targets from the main project, since pip install will run from this subfolder!
#
# 1. Add sources
# 1.1 Sources / immvision
set(immvision_src_DIRS ${CMAKE_CURRENT_LIST_DIR}/../src/immvision)
file(GLOB_RECURSE sources_immvision ${immvision_src_DIRS}/*.h ${immvision_src_DIRS}/*.cpp)
target_sources(${target_name} PRIVATE ${sources_immvision})
target_include_directories(${target_name} PRIVATE ${immvision_src_DIRS}/..)
# 1.2 Sources / glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/src/glad CACHE INTERNAL "Directory of glad")
set(glad_files ${GLAD_DIR}/src/glad.c ${GLAD_DIR}/include/glad/glad.h ${GLAD_DIR}/include/KHR/khrplatform.h)
target_sources(${target_name} PRIVATE ${glad_files})
target_include_directories(${target_name} PRIVATE ${GLAD_DIR}/include)
#
# 2. Add include dirs
#
# 2.1 Find OpenCV include dir
find_package(OpenCV)
if (NOT OpenCV_FOUND)
  message(WARN "find_package(OpenCV) failed, using default (and probably bad) location:
          set(OpenCV_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../external/vcpkg/installed/x64-osx/include) ")
  set(OpenCV_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../external/vcpkg/installed/x64-osx/include)
endif()
# 2.1 Other include dir
set(imgui_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../external/imgui")
target_include_directories(${target_name} PRIVATE ${OpenCV_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS})
#
# 3. Set OpenGL loader
#
set(IMMVISION_CUSTOM_GL_INCLUDE "${GLAD_DIR}/include/glad/glad.h")
target_compile_definitions(${target_name} PRIVATE
    IMMVISION_CUSTOM_GL_INCLUDE=${IMMVISION_CUSTOM_GL_INCLUDE})


target_compile_definitions(${target_name} PRIVATE VERSION_INFO=${PROJECT_VERSION})
install(TARGETS ${target_name} DESTINATION .)
