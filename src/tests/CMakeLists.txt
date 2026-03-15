add_executable(test_immvision_internals test_immvision_internals.cpp)
target_link_libraries(test_immvision_internals PRIVATE immvision)

# immvision transitively depends on imgui which may pull in hello_imgui and stb;
# link hello_imgui if available to resolve all symbols.
if(TARGET hello_imgui)
    target_link_libraries(test_immvision_internals PRIVATE hello_imgui)
endif()
