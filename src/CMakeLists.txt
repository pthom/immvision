include("${CMAKE_CURRENT_LIST_DIR}/../cmake/activate_warnings.cmake")

option(IMMVISION_ACTIVATE_ALL_WARNINGS "Activate all warnings (as error!)" OFF)
if (IMMVISION_ACTIVATE_ALL_WARNINGS)
    immvision_activate_warnings()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if (UNIX AND NOT APPLE)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()


add_subdirectory(immvision_gl_loader)
add_subdirectory(immvision)
add_subdirectory(immdebug)
add_subdirectory(immdebug_viewer)
if (IMMVISION_BUILD_DEMOS)
    add_subdirectory(immvision_demos)
endif()
