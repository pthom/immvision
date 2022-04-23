# Note: this file is *included* (not add_subidrectory) via the main CMakeList (parent folder)
# CMAKE_CURRENT_LIST_DIR is thus ".." i.e the main repo dir
set(THIS_DIR ${PROJECT_SOURCE_DIR}/pybind)

if(DEFINED ENV{OpenCV_DIR})
  set(OpenCV_DIR $ENV{OpenCV_DIR})
  message(WARNING "Got OpenCV_DIR from enviroment: ${OpenCV_DIR}")
endif()

if (WIN32)
  add_compile_options(/FI ${THIS_DIR}/py_imconfig.h)
else()
  add_compile_options(-include ${THIS_DIR}/py_imconfig.h)
endif()


# Find pydinb11 location via python (probably in a virtual env)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/pybind11)

#
# Add OpenCv via conan, if not found on the machine
#
set(opencv_install_help "
Could not find OpenCV. Please install it first.

1. Either via a global installation
Under ubuntu, you can install it with apt:
    sudo apt-get install libopencv-dev

2. Or via a local installation, using conan (https://conan.io/):
First, install conan:
    pip install conan
    conan profile new default --detect

If (and only if) you are using gcc, it is also recommended to run:
    conan profile update settings.compiler.libcxx=libstdc++11 default

Then, install OpenCV for this package via:
    mkdir -p /tmp/foo
    cd /tmp/foo
    # For linux, run:
    conan install ${THIS_DIR}/conanfile_linux.txt --build=missing
    # For other platforms, run:
    conan install ${THIS_DIR}/conanfile.txt --build=missing
")

find_package(OpenCV) # test if opencv can be found
if (NOT OpenCV_FOUND)
  set(IMMVISION_PYBIND_USE_CONAN ON)
  message(WARNING "Did not find a global OpenCV installation. Will try to install it via conan")
endif()
if (IMMVISION_PYBIND_USE_CONAN)
  set(conan_folder ${CMAKE_CURRENT_BINARY_DIR}/conan_third)
  file(MAKE_DIRECTORY ${conan_folder})
  if(UNIX AND NOT APPLE)
    set(conanfile "conanfile_linux.txt")
  else()
    set(conanfile "conanfile.txt")
  endif()

  execute_process(COMMAND
      conan install ${PROJECT_SOURCE_DIR}/pybind/${conanfile}
      WORKING_DIRECTORY ${conan_folder}
      RESULT_VARIABLE conan_install_result
      )
  if (NOT ${conan_install_result} EQUAL "0")
    message(WARNING "conan_install_result=${conan_install_result}")
    message(FATAL_ERROR ${opencv_install_help})
  endif()
  # For conan, add binary dir to module search path
  list(APPEND CMAKE_MODULE_PATH ${conan_folder})
endif()

#
# Main target: cpp_imvision python module
#
file(GLOB_RECURSE sources_immvision_pybind ${THIS_DIR}/pybind_src/*.cpp ${THIS_DIR}/pybind_src/*.h)
file(GLOB_RECURSE sources_immvision ${PROJECT_SOURCE_DIR}/src/immvision/*.h ${PROJECT_SOURCE_DIR}/src/immvision/*.cpp)
pybind11_add_module(cpp_immvision MODULE ${sources_immvision_pybind} ${sources_immvision})
target_include_directories(cpp_immvision PRIVATE ${PROJECT_SOURCE_DIR}/src)
set(cv_np_shared_cast_dir ${THIS_DIR}/cv_np_shared_cast)
target_sources(cpp_immvision PRIVATE
    ${cv_np_shared_cast_dir}/cv_np_shared_cast.cpp
    ${cv_np_shared_cast_dir}/cv_np_shared_cast.h
    ${cv_np_shared_cast_dir}/cv_np_shared_test_helper.cpp
    )
target_include_directories(cpp_immvision PRIVATE ${cv_np_shared_cast_dir})

# Compile definitions
target_compile_definitions(cpp_immvision PRIVATE
    VERSION_INFO=${PROJECT_VERSION}
    IMMVISION_COMPILATION_TIMESTAMP="${IMMVISION_COMPILATION_TIMESTAMP}"
    IMMVISION_BUILDING_PYBIND
)

#
#
#

#
# Link with OpenCV
#
find_package(OpenCV)
if (NOT OpenCV_FOUND)
  message(FATAL_ERROR ${opencv_install_help})
endif()
target_link_libraries(cpp_immvision PRIVATE opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

#
# Include path for imgui
#
set(imgui_source_dir ${PROJECT_SOURCE_DIR}/external/imgui)
file(GLOB imgui_sources ${imgui_source_dir}/*.h ${imgui_source_dir}/*.cpp)
target_include_directories(cpp_immvision PRIVATE ${imgui_source_dir})


#
# Link with imgui: here be dragons
#
# We are here linking with a static version of imgui, but the package
# will communicate with the dynamic libraries of pyimgui
# (imgui/core.cpython-39-darwin.so and internal.cpython-39-darwin.so).
# These dynamic libraries include imgui as well.
# This has several serious implications:
#   1. We *do not need* to create the GImGui instance inside this package (since we link statically with imgui)
#      but we *do need* to transfer ImGui::GetCurrentContext() from pyimgui to this package
#      (see SetImGuiContextFrom_pyimgui_Context())
#   2. We need to be absolutely certain that pyimgui and this package use the exact same version of ImGui
#      (imgui versions are not ABI stable).
#      For this reason, we use a fork of pyimgui which is using our own version of ImGui
#   3. We need to be certain that pyimgui and this package use the same imgui configuration
#      (see py_imconfig.h)
set(link_with_imgui ON)
if (link_with_imgui)
  # If we do not link ==> undefined symbol: _ZN5ImGui11PopStyleVarEi !!!

  # Link with a static library
  add_library(imgui_pybind STATIC ${imgui_sources})
  target_compile_options(imgui_pybind PRIVATE -fPIC)
  target_include_directories(imgui_pybind PUBLIC ${imgui_source_dir})
  target_link_libraries(cpp_immvision PRIVATE imgui_pybind)

  # Link with a shared pybind library  ==> undefined symbol: _ZN5ImGui11PopStyleVarEi (ImGui::PopStyleVar(int)) !!!
  #  pybind11_add_module(imgui_pybind SHARED ${imgui_sources})
  #  target_include_directories(imgui_pybind PUBLIC ${imgui_source_dir})
  #  target_link_libraries(cpp_immvision PRIVATE imgui_pybind)
  #  target_compile_definitions(cpp_immvision PRIVATE IMMVISION_CREATE_GIMGUI_POINTER)
  #  install(TARGETS imgui_pybind DESTINATION .)
endif()


#
# Install and deploy to source dir (for pip editable mode)
#
install(TARGETS cpp_immvision DESTINATION .)
add_custom_command(
        TARGET cpp_immvision
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:cpp_immvision>
        ${THIS_DIR}/pybind_src/immvision/$<TARGET_FILE_NAME:cpp_immvision>
)

# Note: no install for imgui_pybind since it is a static library
#install(TARGETS imgui_pybind DESTINATION .)
#add_custom_command(
#        TARGET imgui_pybind
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        $<TARGET_FILE:imgui_pybind>
#        ${THIS_DIR}/pybind_src/immvision/$<TARGET_FILE_NAME:imgui_pybind>
#    )

#
# immvision_debug_pybind
#
add_executable(pybind_debug_helper ${THIS_DIR}/pybind_debug_helper/pybind_debug_helper.cpp)
target_link_libraries(pybind_debug_helper PRIVATE pybind11::embed)
target_include_directories(pybind_debug_helper PRIVATE ${pybind11_INCLUDE_DIRS})
add_dependencies(pybind_debug_helper cpp_immvision)
