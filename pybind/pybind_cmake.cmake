# Note: this file is *included* (not add_subidrectory) via the main CMakeList (parent folder)
# CMAKE_CURRENT_LIST_DIR is thus ".." i.e the main repo dir
set(THIS_DIR ${PROJECT_SOURCE_DIR}/pybind)
set(PY_MAHI_GUI_DIR ${THIS_DIR}/py-mahi-gui)

include(${THIS_DIR}/find_opencv_with_help.cmake)

find_opencv_with_help()

add_subdirectory(${THIS_DIR}/py-mahi-gui)

if (NOT DEFINED PYTHON_EXECUTABLE)
  if (WIN32 AND EXISTS ${CMAKE_SOURCE_DIR}/venv/Scripts/python.exe)
    set(PYTHON_EXECUTABLE ${CMAKE_SOURCE_DIR}/venv/Scripts/python.exe)
  elseif(EXISTS ${CMAKE_SOURCE_DIR}/venv/bin/python)
    set(PYTHON_EXECUTABLE ${CMAKE_SOURCE_DIR}/venv/bin/python)
  else()
    message(FATAL_ERROR "Please set PYTHON_EXECUTABLE")
  endif()
endif()


#
# Main target: cpp_imvision python module
#
file(GLOB_RECURSE sources_immvision_pybind ${THIS_DIR}/src_cpp/*.cpp ${THIS_DIR}/src_cpp/*.h)
file(GLOB_RECURSE sources_immvision ${PROJECT_SOURCE_DIR}/src/immvision/*.h ${PROJECT_SOURCE_DIR}/src/immvision/*.cpp)
pybind11_add_module(_cpp_immvision MODULE ${sources_immvision_pybind} ${sources_immvision})
target_include_directories(_cpp_immvision PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(_cpp_immvision PUBLIC IMMVISION_BUILD_PYTHON_BINDINGS)
# add cv_np
add_subdirectory(${THIS_DIR}/cvnp)
target_link_libraries(_cpp_immvision PRIVATE cvnp)
# Compile definitions
target_compile_definitions(_cpp_immvision PRIVATE
    VERSION_INFO=${PROJECT_VERSION}
    IMMVISION_COMPILATION_TIMESTAMP="${IMMVISION_COMPILATION_TIMESTAMP}"
    IMMVISION_BUILDING_PYBIND
)

#
# pre-build: autogenerate code
#
add_custom_target(
    autogenerate_immvision
    COMMAND ${PYTHON_EXECUTABLE} ${THIS_DIR}/code_parser/autogenerate_immvision.py
    DEPENDS ${THIS_DIR}/../src/immvision/image.h
    COMMENT "autogenerate python bindings infos"

    # We can't use BYPRODUCTS, since
    # "The Makefile Generators will remove BYPRODUCTS and other GENERATED files during make clean."
    # (cf. https://cmake.org/cmake/help/latest/command/add_custom_target.html)
    #
    #    BYPRODUCTS
    #      ${THIS_DIR}/src_cpp/pydef_image.cpp
    #      ${THIS_DIR}/src_python/immvision/__init__.py
    #      ${THIS_DIR}/src_python/immvision/_cpp_immvision.pyi
    #      ${THIS_DIR}/../src/immvision/internal/misc/immvision_to_string.cpp
    #      ${THIS_DIR}/../src/immvision/internal/misc/immvision_to_string.h
)
if (TARGET immvision)
  add_dependencies(autogenerate_immvision immvision)
endif()
add_dependencies(_cpp_immvision autogenerate_immvision)


#
# Link with OpenCV
#
target_link_libraries(_cpp_immvision PRIVATE opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

#
# Include path for imgui
#
#set(imgui_source_dir ${PY_MAHI_GUI_DIR}/thirdparty/mahi-gui/3rdparty/imgui)
#target_include_directories(_cpp_immvision PRIVATE ${imgui_source_dir})
target_link_libraries(_cpp_immvision PRIVATE mahi::gui)

# Install
install(TARGETS _cpp_immvision DESTINATION .)
install(FILES ${THIS_DIR}/src_python/immvision/_cpp_immvision.pyi DESTINATION .)

# deploy python lib to source dir (for pip editable mode)
add_custom_command(
        TARGET _cpp_immvision
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:_cpp_immvision>
        ${THIS_DIR}/src_python/immvision/$<TARGET_FILE_NAME:_cpp_immvision>
)
# copy debug_utils.py from ./src_python/immvision to ./src_python/immvision/imgui_runner/
FILE(COPY_FILE
    ${THIS_DIR}/src_python/immvision/debug_utils.py
    ${THIS_DIR}/src_python/immvision/imgui_runner/debug_utils.py
    ONLY_IF_DIFFERENT)


# Note: no install for imgui_pybind since it is a static library
#install(TARGETS imgui_pybind DESTINATION .)
#add_custom_command(
#        TARGET imgui_pybind
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        $<TARGET_FILE:imgui_pybind>
#        ${THIS_DIR}/src_python/immvision/$<TARGET_FILE_NAME:imgui_pybind>
#    )

#
# immvision_debug_pybind
#
add_executable(pybind_native_debug ${THIS_DIR}/pybind_native_debug/pybind_native_debug.cpp)
target_link_libraries(pybind_native_debug PRIVATE pybind11::embed)
target_include_directories(pybind_native_debug PRIVATE ${pybind11_INCLUDE_DIRS})
add_dependencies(pybind_native_debug _cpp_immvision)
