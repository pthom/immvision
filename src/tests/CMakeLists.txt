add_executable(test_immvision_internals test_immvision_internals.cpp)
target_link_libraries(test_immvision_internals PRIVATE immvision)

# immvision transitively depends on imgui which may pull in hello_imgui and stb;
# link hello_imgui if available to resolve all symbols.
if(TARGET hello_imgui)
    target_link_libraries(test_immvision_internals PRIVATE hello_imgui)
endif()
add_test(NAME test_immvision_internals COMMAND test_immvision_internals)

# OpenCV interop test: the *test* defines IMMVISION_HAS_OPENCV and links OpenCV,
# the immvision library does not.
if (IMMVISION_BUILD_OPENCV_DEMOS)
    add_executable(test_immvision_opencv_interop test_immvision_opencv_interop.cpp)
    target_compile_definitions(test_immvision_opencv_interop PRIVATE IMMVISION_HAS_OPENCV)
    target_link_libraries(test_immvision_opencv_interop PRIVATE immvision opencv_core)
    if(TARGET hello_imgui)
        target_link_libraries(test_immvision_opencv_interop PRIVATE hello_imgui)
    endif()
    if (WIN32 AND IMMVISION_OPENCV_WORLD_DLL)
        # precompiled OpenCV on Windows: the test needs opencv_world.dll next to its exe
        add_custom_command(TARGET test_immvision_opencv_interop POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IMMVISION_OPENCV_WORLD_DLL} $<TARGET_FILE_DIR:test_immvision_opencv_interop>)
    endif()
    add_test(NAME test_immvision_opencv_interop COMMAND test_immvision_opencv_interop)
endif()
