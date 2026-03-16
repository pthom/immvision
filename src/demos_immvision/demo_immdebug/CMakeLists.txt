add_executable(demo_immdebug demo_immdebug.main.cpp)
target_link_libraries(demo_immdebug PRIVATE immdebug immvision)
# Also link to the hello_imgui (just to get stb_image.h implem from it)
target_link_libraries(demo_immdebug PRIVATE hello_imgui)
