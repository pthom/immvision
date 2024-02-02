if (IOS OR ANDROID OR EMSCRIPTEN)
    set(IMMVISION_USE_GLES3 ON)
    # set(IMMVISION_USE_GLES2 ON)
else()
    set(IMMVISION_USE_GLAD ON)
endif()

if (IMMVISION_USE_GLAD)
    set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad CACHE INTERNAL "Directory of glad")
    set(GLAD_FILES
        ${GLAD_DIR}/src/glad.c
        ${GLAD_DIR}/include/glad/glad.h
        ${GLAD_DIR}/include/KHR/khrplatform.h)
    # source_group_by_path_all(${GLAD_DIR} ${GLAD_FILES})
    add_library(glad_imm ${GLAD_FILES})
    target_include_directories(glad_imm PUBLIC $<BUILD_INTERFACE:${GLAD_DIR}/include>)
    get_target_property(library_type glad_imm TYPE)
    if (library_type STREQUAL SHARED_LIBRARY)
        # If glad_imm is a shared library, define the macro GLAD_API_EXPORT on the command line.
        target_compile_definitions(glad_imm PRIVATE GLAD_GLAPI_EXPORT)
        target_compile_definitions(glad_imm PUBLIC GLAD_GLAPI_EXPORT PRIVATE GLAD_GLAPI_EXPORT_BUILD)
    endif()
endif()


add_library(immvision_gl_loader immvision_gl_loader.cpp immvision_gl_loader.h)
target_include_directories(immvision_gl_loader PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

if (NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    target_link_libraries(immvision_gl_loader PUBLIC OpenGL::GL)
endif()

if (IMMVISION_USE_GLAD)
    target_link_libraries(immvision_gl_loader PUBLIC glad_imm)
    target_compile_definitions(immvision_gl_loader PUBLIC IMMVISION_USE_GLAD)
elseif(IMMVISION_USE_GLES3)
    target_compile_definitions(immvision_gl_loader PUBLIC IMMVISION_USE_GLES3)
elseif(IMMVISION_USE_GLES2)
    target_compile_definitions(immvision_gl_loader PUBLIC IMMVISION_USE_GLES2)
else()
    message(FATAL_ERROR "Cannot determine GL loader")
endif()
