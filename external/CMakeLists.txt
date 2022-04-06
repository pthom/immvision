# imgui is built by default
# You can use your own (but make sure that a target named imgui is available)
if (IMMVISION_BUILD_IMGUI)
    set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/imgui)
    add_library(imgui
        ${imgui_dir}/imconfig.h
        ${imgui_dir}/imgui.cpp
        ${imgui_dir}/imgui.h
        ${imgui_dir}/imgui_demo.cpp
        ${imgui_dir}/imgui_draw.cpp
        ${imgui_dir}/imgui_internal.h
        ${imgui_dir}/imgui_tables.cpp
        ${imgui_dir}/imgui_widgets.cpp
        )
    target_include_directories(imgui PUBLIC ${imgui_dir})
endif()

if (IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/hello_imgui)
    set(HELLOIMGUI_USE_SDL_OPENGL3 ON CACHE BOOL "" FORCE)
    set(HELLOIMGUI_BUILD_IMGUI OFF CACHE BOOL "" FORCE)
    set(HELLOIMGUI_IMGUI_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/imgui" CACHE STRING "Source dir of imgui" FORCE)
    set(HELLOIMGUI_USE_POWERSAVE ${IMMVISION_USE_POWERSAVE} CACHE BOOL "" FORCE)
    add_subdirectory(hello_imgui)
endif()