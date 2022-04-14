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

set(IMMVISION_PYBIND_BIN_MODULE_NAME _core_immvision_pybind) # was initially _core (can it be changed?)
file(GLOB_RECURSE sources_immvision_pybind src/*.cpp src/*.h)
pybind11_add_module(${IMMVISION_PYBIND_BIN_MODULE_NAME} MODULE ${sources_immvision_pybind})

#
# Define specific settings for immvision:
# do not use targets from the main project, since pip install will run from this subfolder!
#
# 1. Add sources
# 1.1 Sources / immvision
set(immvision_src_DIRS ${CMAKE_CURRENT_LIST_DIR}/../src/immvision)
file(GLOB_RECURSE sources_immvision ${immvision_src_DIRS}/*.h ${immvision_src_DIRS}/*.cpp)
target_sources(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${sources_immvision})
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${immvision_src_DIRS}/..)
# 1.2 Sources / glad  ## KK Remove!!!
#set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/src/glad CACHE INTERNAL "Directory of glad")
#file(GLOB_RECURSE sources_glad ${GLAD_DIR}/*.h ${GLAD_DIR}/*.c)
#target_sources(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${sources_glad})
#target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${GLAD_DIR}/include)
#
# 2. Add include dirs
#
set(default_include_dir ${CMAKE_CURRENT_LIST_DIR}/../external/vcpkg/installed/x64-osx/include)
# 2.1 Find OpenCV include dir
find_package(OpenCV)
if (NOT OpenCV_FOUND)
  message(WARN "find_package(OpenCV) failed, using default (and probably bad) location:
          set(OpenCV_INCLUDE_DIRS ${default_include_dir}) ")
  set(OpenCV_INCLUDE_DIRS ${default_include_dir})
endif()
# 2.2 SDL2 include dir     ## KK Remove!!!
#find_package(SDL2 CONFIG)
#if (NOT SDL2_FOUND)
#  message(WARN "find_package(SDL2) failed, using default (and probably bad) location:
#          set(SDL2_INCLUDE_DIRS ${default_include_dir} ${default_include_dir}/SDL2) ")
#  set(SDL2_INCLUDE_DIRS ${default_include_dir} ${default_include_dir}/SDL2)
#endif()
#target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME}  PRIVATE ${SDL2_INCLUDE_DIRS})
# 2.3 Numpy include dir
set(numpy_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../external/numpy/numpy/core/include)
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME}  PRIVATE ${numpy_INCLUDE_DIRS})

# 2.3 imgui include dir
set(imgui_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../external/imgui")
target_include_directories(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS})
#
# 3. Set OpenGL loader // ## KK Remove!!!
#
set(IMMVISION_CUSTOM_GL_INCLUDE "${GLAD_DIR}/include/glad/glad.h")
target_compile_definitions(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE
    IMMVISION_CUSTOM_GL_INCLUDE=${IMMVISION_CUSTOM_GL_INCLUDE})


target_compile_definitions(${IMMVISION_PYBIND_BIN_MODULE_NAME} PRIVATE
    VERSION_INFO=${PROJECT_VERSION}
    IMMVISION_PYBIND_BIN_MODULE_NAME=${IMMVISION_PYBIND_BIN_MODULE_NAME}
    IMMVISION_PYBIND
    )
install(TARGETS ${IMMVISION_PYBIND_BIN_MODULE_NAME} DESTINATION .)
