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
    # Download a precompiled OpenCV 4.10.0 x64 package (opencv_world.dll).
    # Used as a fallback for local Windows x64 `pip install` when no system OpenCV is found.
    # On ARM64, this is skipped (see immvision_find_opencv).
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
    set(OpenCV_DIR ${opencv_official_package_win_dir} CACHE PATH "" FORCE)
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
    set(OpenCV_DIR  ${CMAKE_BINARY_DIR}/_deps/opencv_package_emscripten-src/lib/cmake/opencv4  CACHE PATH "" FORCE)
endmacro()



macro(immvision_fetch_opencv_from_source)
    # Fetch, build and install a very-minimalist OpenCV via ci/build_opencv.sh
    # (one script used by both CMake and CI — see docs/book/devel_docs/build_opencv_immvision.md)
    set(opencv_install_dir "${CMAKE_BINARY_DIR}/_deps/opencv_fetch-install")

    # Skip if already built (OpenCVConfig.cmake exists in install dir)
    file(GLOB_RECURSE _existing_opencv_config "${opencv_install_dir}/OpenCVConfig.cmake")
    if (_existing_opencv_config)
        list(GET _existing_opencv_config 0 _opencv_config)
        get_filename_component(_opencv_dir "${_opencv_config}" DIRECTORY)
        set(OpenCV_DIR "${_opencv_dir}" CACHE PATH "" FORCE)
        message(STATUS "OpenCV already built — reusing ${OpenCV_DIR}")
        set(OpenCV_STATIC ON CACHE BOOL "" FORCE)
    else()
        # Locate the build script (same directory as this cmake file)
        set(_build_opencv_script "${_find_opencv_dir}/build_opencv.sh")

        # Build script args
        set(_script_args "")
        if (IMMVISION_FETCH_OPENCV_FULL)
            list(APPEND _script_args "--full")
        endif()

        # Forward architecture flags to the OpenCV build.
        if (WIN32)
            # On Windows, forward the VS platform (e.g. -A x64) so the inner cmake
            # targets the same architecture as the parent. Without this, the VS generator
            # defaults to the host arch (ARM64 on ARM64 machines).
            # Note: we do NOT try to override CMAKE_SYSTEM_PROCESSOR — CMake's project()
            # resets it from the host regardless of -D overrides. The SIMD mismatch
            # (ARM64 host detected, x64 compiler used) is handled by -DWITH_SIMD=OFF
            # in build_opencv.sh instead.
            if (CMAKE_VS_PLATFORM_NAME)
                list(APPEND _script_args "-A" "${CMAKE_VS_PLATFORM_NAME}")
            endif()
        else()
            # On Unix, forward processor flags (needed for macOS cross-compilation).
            foreach(flag CMAKE_HOST_SYSTEM_PROCESSOR CMAKE_SYSTEM_PROCESSOR)
                if (DEFINED ${flag} AND NOT "${${flag}}" STREQUAL "")
                    list(APPEND _script_args "-D${flag}=${${flag}}")
                endif()
            endforeach()
        endif()
        foreach(flag CMAKE_OSX_DEPLOYMENT_TARGET CMAKE_OSX_ARCHITECTURES)
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

        # Use _immvision_bash if set by caller (Windows bash detection), else plain "bash"
        if(NOT _immvision_bash)
            set(_immvision_bash bash)
        endif()

        execute_process(
            COMMAND "${_immvision_bash}" "${_build_opencv_script}" "${opencv_install_dir}" ${_script_args}
            RESULT_VARIABLE result
            COMMAND_ECHO STDOUT
        )
        if (NOT result EQUAL 0)
            message(FATAL_ERROR "build_opencv.sh failed (exit code ${result})")
        else()
            message(STATUS "build_opencv.sh succeeded")
        endif()

        # Point find_package to the freshly built OpenCV.
        # The install layout varies by platform and OpenCV config, so search for it.
        set(OpenCV_STATIC ON CACHE BOOL "" FORCE)
        file(GLOB_RECURSE _opencv_configs "${opencv_install_dir}/OpenCVConfig.cmake")
        if (_opencv_configs)
            list(GET _opencv_configs 0 _opencv_config)
            get_filename_component(_opencv_dir "${_opencv_config}" DIRECTORY)
            set(OpenCV_DIR "${_opencv_dir}" CACHE PATH "" FORCE)
            message(STATUS "Found OpenCVConfig.cmake at ${OpenCV_DIR}")
        else()
            message(FATAL_ERROR "
            immvision_fetch_opencv_from_source: can't find OpenCVConfig.cmake under ${opencv_install_dir}
            ")
        endif()
    endif()
endmacro()


macro(immvision_fetch_opencv)
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
                # On ARM64, skip the precompiled x64 package — go straight to source build.
                # The precompiled package is x64-only and its deep directory tree triggers
                # Windows MAX_PATH cleanup failures in scikit-build-core.
                set(_skip_precompiled OFF)
                if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
                    set(_skip_precompiled ON)
                endif()
                if(NOT _skip_precompiled)
                    immvision_download_opencv_official_package_win() # will download prebuilt package
                    find_package(OpenCV QUIET)
                endif()
                if(NOT OpenCV_FOUND)
                    # Precompiled package incompatible or skipped, try source build.
                    # Find bash: on Windows, Git/bin/bash.exe isn't on PATH by default,
                    # but git.exe (in Git/cmd/) is. Derive bash location from git.
                    find_program(_bash_exe bash)
                    if(NOT _bash_exe)
                        find_program(_git_exe git)
                        if(_git_exe)
                            get_filename_component(_git_dir "${_git_exe}" DIRECTORY)
                            get_filename_component(_git_root "${_git_dir}" DIRECTORY)
                            find_program(_bash_exe bash HINTS "${_git_root}/bin")
                        endif()
                    endif()
                    if(_bash_exe)
                        message("Building OpenCV from source (bash=${_bash_exe})...")
                        set(_immvision_bash "${_bash_exe}")
                        immvision_fetch_opencv_from_source()
                    else()
                        message("Cannot build OpenCV from source: bash not found.")
                    endif()
                endif()
            elseif(fetch_opencv)
                immvision_fetch_opencv_from_source()  # Will fetch, build and install OpenCV if IMMVISION_FETCH_OPENCV
            endif()
        endif()
    endif()

    find_package(OpenCV)
    if (NOT OpenCV_FOUND)
        message("
        ----------------------------------------------------------------------------------
        immvision_fetch_opencv failed!
        Please install OpenCV manually (or set OpenCV_DIR to the OpenCVConfig.cmake directory) and re-run CMake.
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


