# Note: this file is *included* (not add_subidrectory) via the main CMakeList (parent folder)
# CMAKE_CURRENT_LIST_DIR is thus ".." i.e the main repo dir
set(THIS_DIR ${PROJECT_SOURCE_DIR}/pybind)

#
# Find pydinb11 location via python (probably in a virtual env)
#
if (NOT DEFINED PYTHON_EXECUTABLE)
  if (EXISTS ${PROJECT_SOURCE_DIR}/venv/bin/python)
    set(PYTHON_EXECUTABLE "${PROJECT_SOURCE_DIR}/venv/bin/python")
  elseif(EXISTS ${PROJECT_SOURCE_DIR}/venv_docker/bin/python)
    set(PYTHON_EXECUTABLE "${PROJECT_SOURCE_DIR}/venv_docker/bin/python")
  else()
    set(PYTHON_EXECUTABLE "python")
  endif()
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/external/pybind11)

#
# Main target: cpp_imvision python module
#
file(GLOB_RECURSE sources_immvision_pybind ${THIS_DIR}/pybind_src/*.cpp ${THIS_DIR}/pybind_src/*.h)
file(GLOB_RECURSE sources_immvision ${PROJECT_SOURCE_DIR}/src/immvision/*.h ${PROJECT_SOURCE_DIR}/src/immvision/*.cpp)
pybind11_add_module(cpp_immvision MODULE ${sources_immvision_pybind} ${sources_immvision})
target_include_directories(cpp_immvision PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Compile definitions
target_compile_definitions(cpp_immvision PRIVATE
    VERSION_INFO=${PROJECT_VERSION}
    IMMVISION_COMPILATION_TIMESTAMP="${IMMVISION_COMPILATION_TIMESTAMP}"
    IMMVISION_BUILDING_PYBIND
)

#
# Apple: no link at all
#
if (APPLE)
  set(IMMVISION_NOLINK_APPLE ON)
  target_compile_definitions(cpp_immvision PRIVATE IMMVISION_NOLINK_APPLE)
endif()

#
# Link with OpenCV
#
find_package(OpenCV)
if (NOT OpenCV_FOUND)
  set(default_opencv_include_dir ${PROJECT_SOURCE_DIR}/external/vcpkg/installed/x64-osx/include)
  message(WARN "find_package(OpenCV) failed, using default (and probably bad) location:
          set(OpenCV_INCLUDE_DIRS ${default_opencv_include_dir}) ")
  set(OpenCV_INCLUDE_DIRS ${default_opencv_include_dir})
else()
  # Hack for broken vcpkg naming
  set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} ${opencv_INCLUDE_DIRS})
endif()
target_include_directories(cpp_immvision PRIVATE ${OpenCV_INCLUDE_DIRS})
# Link
if (NOT IMMVISION_NOLINK_APPLE)
    target_link_libraries(cpp_immvision PRIVATE opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
endif()

#
# Include path for imgui
#
set(imgui_source_dir ${PROJECT_SOURCE_DIR}/external/imgui)
file(GLOB imgui_sources ${imgui_source_dir}/*.h ${imgui_source_dir}/*.cpp)
target_include_directories(cpp_immvision PRIVATE ${imgui_source_dir})


if (NOT IMMVISION_NOLINK_APPLE)
  # Methode sans rien linker
  #  ==> undefined symbol: _ZN5ImGui11PopStyleVarEi

  # Methode avec link statique:
  # => les import semblent fonctionner, mais est ce
  # => ensuite segfault dans GImGui
  add_library(imgui_shared_pybind STATIC ${imgui_sources})
  target_compile_options(imgui_shared_pybind PRIVATE -fPIC)
  target_include_directories(imgui_shared_pybind PUBLIC ${imgui_source_dir})
  target_link_libraries(cpp_immvision PRIVATE imgui_shared_pybind)


  #
  # Methode avec lib shared pybind:
  #  ==> undefined symbol: GImGui (pb classique, solution connue cf main.cpp)
  #  ==> puis undefined symbol: _ZN5ImGui11PopStyleVarEi (ImGui::PopStyleVar(int))
  #pybind11_add_module(imgui_shared_pybind SHARED ${imgui_sources})
  #target_include_directories(imgui_shared_pybind PUBLIC ${imgui_source_dir})
  #target_link_libraries(cpp_immvision PRIVATE imgui_shared_pybind)
  #install(TARGETS imgui_shared_pybind DESTINATION .)



  # Methode avec lib shared classique:
  # => libimgui_shared_pybind.so: cannot open shared object file: No such file or directory
  # bien que present dans package dir...
  # => import marche avec LD_LIBRARY_PATH=/home/pascal/dvp/immvision/venv/lib/python3.8/site-packages/immvision python
  # => mais ensuite segfault dans GImGui
  #add_library(imgui_shared_pybind SHARED ${imgui_sources})
  #target_include_directories(imgui_shared_pybind PUBLIC ${imgui_source_dir})
  #target_link_libraries(cpp_immvision PRIVATE imgui_shared_pybind)
  #install(TARGETS imgui_shared_pybind DESTINATION .)



  # Methode / Hack link pip imgui on docker
  # ==> Semi-ok, avec LD_LIBRARY_PATH avant appel python
  # LD_LIBRARY_PATH=/dvp/sources/immvision_pybind/venv_docker/lib/python3.10/site-packages/imgui
  #set(imgui_docker_lib_dir "/dvp/sources/immvision_pybind/venv_docker/lib/python3.10/site-packages/imgui/")
  #target_link_directories(cpp_immvision PRIVATE ${imgui_docker_lib_dir})
  #target_link_libraries(cpp_immvision PRIVATE core internal)
  #install(TARGETS imgui_shared_pybind DESTINATION .)
endif(NOT IMMVISION_NOLINK_APPLE)


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
if (NOT IMMVISION_NOLINK_APPLE)
  install(TARGETS imgui_shared_pybind DESTINATION .)
  add_custom_command(
          TARGET imgui_shared_pybind
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy
          $<TARGET_FILE:imgui_shared_pybind>
          ${THIS_DIR}/pybind_src/immvision/$<TARGET_FILE_NAME:imgui_shared_pybind>
  )
endif()

#
# immvision_debug_pybind
#
add_executable(pybind_debug_helper ${THIS_DIR}/pybind_debug_helper/pybind_debug_helper.cpp)
target_link_libraries(pybind_debug_helper PRIVATE pybind11::embed)
target_include_directories(pybind_debug_helper PRIVATE ${pybind11_INCLUDE_DIRS})
