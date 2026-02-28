###############################################################################
# Get OpenCV package
###############################################################################
# For an overview of how OpenCV is obtained for each platform, see:
#   docs/book/devel_docs/build_opencv_immvision.md (in imgui_bundle)

# Capture this directory at include-time (not inside a macro, where
# CMAKE_CURRENT_LIST_DIR would resolve to the *caller's* directory).
set(_find_opencv_dir "${CMAKE_CURRENT_LIST_DIR}")

###############################################################################
# Note about pip and wheel builds:
#
# - opencv-python (or its companions opencv-contrib-python, opencv-python-headless
#   and opencv-contrib-python-headless) is the pip package providing opencv:
#   when installed it comes with a dll, or an .so file (cv2.abi3.so)
#
# - when building wheels for macos and linux, and when fetching OpenCV from source,
#  a static minimal version of opencv will be linked into _imgui_bundle.so.
#  Thus there is no risk of doubly defined functions:
#     imgui_bundle will search into the linked functions inside _imgui_bundle.so,
#     and opencv-python and co will search inside cv2.abi3.so.
###############################################################################


macro(immvision_download_opencv_official_package_win)
    # Download a precompiled version of opencv4.6.0
    # This is the official release from OpenCV for windows, under the form of a "opencv_world.dll"
    # The zip package we download is just a transcription of the exe provided by OpenCV, with the advantage that
    # it does not require any user click.
    # Mainly used for wheel builds, but can be used by windows users for their pip build from source
    message("FIND OPENCV use immvision_download_opencv_official_package_win")

    include(FetchContent)
    Set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Declare(
        opencv_official_package_win
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        URL https://github.com/pthom/imgui_bundle/releases/download/v1.3.0/opencv-4.10.0-windows.zip
        URL_MD5 309471e093251b78a25bef1f8c255ce6
    )
    FetchContent_MakeAvailable(opencv_official_package_win)
    set(opencv_official_package_win_dir ${CMAKE_BINARY_DIR}/_deps/opencv_official_package_win-src/opencv-4.10.0-windows/build)
    message(WARNING "opencv_official_package_win_dir=${opencv_official_package_win_dir}")
    set(OpenCV_DIR ${opencv_official_package_win_dir})
    # set(OpenCV_STATIC ON)
endmacro()

function(check_emscripten_emcc_version)
    execute_process(
        COMMAND emcc --version
        OUTPUT_VARIABLE EMCC_VERSION_OUTPUT
    )
    string(REGEX MATCH "emcc.* ([0-9]+\\.[0-9]+\\.[0-9]+)" _ ${EMCC_VERSION_OUTPUT})
    set(EMCC_VERSION ${CMAKE_MATCH_1})
    set(REQUIRED_EMCC_VERSION "3.1.57")
    if(EMCC_VERSION VERSION_LESS REQUIRED_EMCC_VERSION)
        message(FATAL_ERROR "emscripten / emcc version must be at least ${REQUIRED_EMCC_VERSION}, but found ${EMCC_VERSION}")
    endif()
endfunction()

macro(immvision_download_emscripten_precompiled_opencv_4_9_0)
    # Download a precompiled version of opencv4.9.0 for emscripten
    # (see imgui_bundle/devel_docs/devdoc_parts/emscripten_build.adoc for info on how it was built)
    message("Download a precompiled version of opencv4.9.0 for emscripten")
    check_emscripten_emcc_version()

    # Check that we are using emcc version 3.1.57 or higher
    execute_process(
        COMMAND emcc --version
        OUTPUT_VARIABLE emcc_version
    )

    include(FetchContent)
    Set(FETCHCONTENT_QUIET FALSE)
    option(IMMVISION_OPENCV_EMSCRIPTEN_WITH_PTHREAD "" OFF)
    if (IMGUI_BUNDLE_BUILD_PYODIDE)
        FetchContent_Declare(
            opencv_package_emscripten
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            URL https://github.com/pthom/imgui_bundle/releases/download/v1.6.2/opencv_4.11_wasmexcept_pthread_fpic_emscripten_minimalist_install.tgz
            URL_MD5 c3ff4526ec6638a786f21438f5e1b1cb
        )
    elseif(IMMVISION_OPENCV_EMSCRIPTEN_WITH_PTHREAD)
        FetchContent_Declare(
            opencv_package_emscripten
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            URL https://github.com/pthom/imgui_bundle/releases/download/v1.5.2/opencv_4.9_pthread_fpic_emscripten_minimalist_install.tgz
            URL_MD5 3b970fa37bca8ac3dc977a7a6c33b9b7
        )
    else()
        FetchContent_Declare(
            opencv_package_emscripten
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            URL https://github.com/pthom/imgui_bundle/releases/download/v1.3.0/opencv_4.9_no_pthread_emscripten_install.tgz
            URL_MD5 2a9e65542123548bec1b80ee2cb6056e
        )
    endif()
    FetchContent_MakeAvailable(opencv_package_emscripten)
    set(OpenCV_DIR  ${CMAKE_BINARY_DIR}/_deps/opencv_package_emscripten-src/lib/cmake/opencv4)
endmacro()



macro(immvision_fetch_opencv_from_source)
    # Fetch, build and install a very-minimalist OpenCV via ci/build_opencv.sh
    # (one script used by both CMake and CI — see docs/book/devel_docs/build_opencv_immvision.md)
    set(opencv_install_dir "${CMAKE_BINARY_DIR}/_deps/opencv_fetch-install")

    # Locate the build script (same directory as this cmake file)
    set(_build_opencv_script "${_find_opencv_dir}/build_opencv.sh")

    # Build script args
    set(_script_args "")
    if (IMMVISION_FETCH_OPENCV_FULL)
        list(APPEND _script_args "--full")
    endif()

    # Forward architecture flags (needed for macOS cross-compilation)
    # Note: we cannot compile universal2 wheels at this time, because CMake confuses itself
    # The incoming CMAKE_OSX_ARCHITECTURES is "x86_64;arm64"
    # However, it is difficult to pass it as is to the cmake args for OpenCV,
    # since CMake happily splits it in two...
    foreach(flag CMAKE_HOST_SYSTEM_PROCESSOR CMAKE_SYSTEM_PROCESSOR CMAKE_OSX_DEPLOYMENT_TARGET CMAKE_OSX_ARCHITECTURES)
        if (DEFINED ${flag} AND NOT "${${flag}}" STREQUAL "")
            list(APPEND _script_args "-D${flag}=${${flag}}")
        endif()
    endforeach()

    # Forward IMMVISION_OPENCV_GIT_REPO cmake variable to environment (script reads env)
    if(DEFINED IMMVISION_OPENCV_GIT_REPO AND NOT DEFINED ENV{IMMVISION_OPENCV_GIT_REPO})
        set(ENV{IMMVISION_OPENCV_GIT_REPO} "${IMMVISION_OPENCV_GIT_REPO}")
    endif()

    message("
    -----------------------------------------------------------------------
    immvision_fetch_opencv_from_source:
        script=${_build_opencv_script}
        install_dir=${opencv_install_dir}
        extra_args=${_script_args}
    -----------------------------------------------------------------------
    ")

    execute_process(
        COMMAND bash "${_build_opencv_script}" "${opencv_install_dir}" ${_script_args}
        RESULT_VARIABLE result
    )
    if (NOT result EQUAL 0)
        message(FATAL_ERROR "build_opencv.sh failed")
    endif()

    # Point find_package to the install
    if (WIN32)
        set(OpenCV_DIR ${opencv_install_dir})
        set(OpenCV_STATIC ON CACHE BOOL "" FORCE)
    else()
        if (IS_DIRECTORY ${opencv_install_dir}/lib/cmake/opencv4)
            set(OpenCV_DIR ${opencv_install_dir}/lib/cmake/opencv4)
        elseif(IS_DIRECTORY ${opencv_install_dir}/lib64/cmake/opencv4)
            set(OpenCV_DIR ${opencv_install_dir}/lib64/cmake/opencv4)
        endif()
    endif()
    if (NOT EXISTS ${OpenCV_DIR}/OpenCVConfig.cmake)
        message(FATAL_ERROR "
        immvision_fetch_opencv_from_source: can't find OpenCVConfig.cmake
        ")
    endif()
endmacro()


macro(immvision_find_opencv)
    # Forward OpenCV_STATIC from environment (used by CI pre-built static OpenCV on Windows)
    if(DEFINED ENV{OpenCV_STATIC})
        set(OpenCV_STATIC "$ENV{OpenCV_STATIC}" CACHE BOOL "" FORCE)
    endif()

    if (EMSCRIPTEN AND IMMVISION_FETCH_OPENCV)
        find_package(OpenCV QUIET)
        if (NOT OpenCV_FOUND)
            immvision_download_emscripten_precompiled_opencv_4_9_0()
        endif()
    else()
        find_package(OpenCV QUIET)
        if (NOT OpenCV_FOUND)
            if ("$ENV{IMMVISION_FETCH_OPENCV}" OR IMMVISION_FETCH_OPENCV)
                set(fetch_opencv ON)
            endif()
            if(WIN32 AND fetch_opencv)
                immvision_download_opencv_official_package_win() # will download prebuilt package
                # oldie_immvision_forward_opencv_env_variables()
            elseif(fetch_opencv)
                immvision_fetch_opencv_from_source()  # Will fetch, build and install OpenCV if IMMVISION_FETCH_OPENCV
            endif()
        endif()
    endif()

    find_package(OpenCV)
    if (NOT OpenCV_FOUND)
        message("
        ----------------------------------------------------------------------------------
        immvision requires OpenCV

        If you want immvision to be built, install OpenCV before running cmake.
        Tip: you can run
            cmake -DIMMVISION_FETCH_OPENCV=ON ..
        in order to automatically download and build a (very) minimal version of OpenCV.
        ----------------------------------------------------------------------------------
        ")
    else()
        message("found OpenCV OpenCV_DIR=${OpenCV_DIR} ")
    endif()

    # Under windows install dll opencv_worldxxx.dll to package
    if (WIN32 AND OpenCV_FOUND)
        set(immvision_OpenCV_DLL_PATH "${OpenCV_LIB_PATH}/../bin")
        message("immvision_OpenCV_DLL_PATH=${immvision_OpenCV_DLL_PATH}")

        file(GLOB immvision_opencv_world_dll ${immvision_OpenCV_DLL_PATH}/opencv_world*.dll)

        if (SKBUILD)
            # Remove OpenCV debug dlls (their name end with d.dll):
            #   When building the pip package, we do not want to deploy the debug dlls
            #   (they are not needed by the python code, and they are huge)
            message(STATUS "immvision_find_opencv: removing OpenCV debug dlls")
            list(FILTER immvision_opencv_world_dll EXCLUDE REGEX ".*d.dll")
        endif()

        if (immvision_opencv_world_dll)
            if (IMGUI_BUNDLE_BUILD_PYTHON)
                if (SKBUILD)
                    # specific for imgui_bundle pip package
                    install(FILES ${immvision_opencv_world_dll} DESTINATION imgui_bundle)
                else()
                    # Not sure we should install in other cases
                    install(FILES ${immvision_opencv_world_dll} DESTINATION .)
                endif()
            endif()
            set(IMMVISION_OPENCV_WORLD_DLL ${immvision_opencv_world_dll} CACHE STRING "opencv_word dll needed by cpp programs" FORCE)
        endif()
    endif()
endmacro()


macro(oldie_immvision_forward_opencv_env_variables)
    # Forward environment variable to standard variables that are used by OpenCVConfig.cmake
    # This is useful when building the pip package for which only env variables are available
    # (this is deprecated, but may prove useful when building for windows Arm64)
    if(DEFINED ENV{OpenCV_DIR})
        set(OpenCV_DIR "$ENV{OpenCV_DIR}")
        message("immvision_forward_opencv_env_variables: Forwarding env OpenCV_DIR=${OpenCV_DIR}")
    endif()

    if(DEFINED ENV{OpenCV_STATIC})
        set(OpenCV_STATIC "$ENV{OpenCV_STATIC}")
        message("immvision_forward_opencv_env_variables: Forwarding env OpenCV_STATIC=${OpenCV_STATIC}")
    endif()

    # I know I know, one should not hack CMAKE_GENERATOR_PLATFORM when not doing cross platform builds
    # But when you want to build for windows ARM64, this is the only option given by OpenCVConfig.cmake...
    if(DEFINED ENV{CMAKE_GENERATOR_PLATFORM})
        set(CMAKE_GENERATOR_PLATFORM "$ENV{CMAKE_GENERATOR_PLATFORM}")
        message("immvision_forward_opencv_env_variables: Forwarding env CMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}")
    endif()
endmacro()

