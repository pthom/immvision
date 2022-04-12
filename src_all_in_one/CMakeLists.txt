include("${CMAKE_CURRENT_LIST_DIR}/../cmake/activate_warnings.cmake")

if (IMMVISION_ACTIVATE_ALL_WARNINGS)
    immvision_activate_warnings()
endif()

add_subdirectory(immvision)
