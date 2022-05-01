include("${CMAKE_CURRENT_LIST_DIR}/../cmake/activate_warnings.cmake")

if (IMMVISION_ACTIVATE_ALL_WARNINGS)
    immvision_activate_warnings()
endif()

add_subdirectory(immvision)

# ---- Create auto-generated sources in src_all_in_one ----
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_FOUND)
    add_custom_target(
        generate_src_all_in_one
        COMMAND
        "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/scripts/generate_src_all_in_one/generate_src_all_in_one.py"
        BYPRODUCTS
        "${PROJECT_SOURCE_DIR}/src_all_in_one/immvision/immvision.cpp"
        "${PROJECT_SOURCE_DIR}/src_all_in_one/immvision/immvision.h"
    )
    add_dependencies(generate_src_all_in_one immvision )
    add_dependencies(immvision_all_in_one generate_src_all_in_one)
else()
    message(STATUS "Not adding the 'generate_src_all_in_one' target (requires Python3)")
endif()
