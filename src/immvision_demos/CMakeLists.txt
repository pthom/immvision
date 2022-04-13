set(IMMVISION_BUILD_IMGUI_SDL_OPENGL3 ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(immdebug_demo)

if (IMMVISION_BUILD_IMGUI_SDL_OPENGL3)
    add_subdirectory(immvision_demo)
    add_subdirectory(inspector_demo)
endif()
FILE(COPY ${CMAKE_CURRENT_LIST_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
