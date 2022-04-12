#!/usr/bin/env bash

# exit when any command fails
set -e

# Specify the include folders for OpenCV and Dear ImGui
OPENCV_INCLUDE_FOLDER="../../external/vcpkg/installed/x64-osx/include"
IMGUI_INCLUDE_FOLDER="../../external/imgui"

# Specify your OpenGL Loader via IMMVISION_CUSTOM_GL_INCLUDE (i.e how do you include OpenGL?)
# See src/immvision_gl_loader/immvision_gl_loader.h for the different way to customize this
# Note: the build parameter `-I $GLAD_INCLUDE_PATH` is needed only if you are using glad as the OpenGL loader.
GLAD_INCLUDE_PATH="../../src/immvision_gl_loader/glad/include"
IMMVISION_CUSTOM_GL_INCLUDE="$GLAD_INCLUDE_PATH/glad/glad.h"

# Set the c++ standard version: can be c++14, c++17 or c++20
CXX_STANDARD="c++14"

cmd="
        g++ --std=$CXX_STANDARD -c immvision.cpp -o immvision.o \
            -Wall \
            \
            -I $IMGUI_INCLUDE_FOLDER \
            -I $OPENCV_INCLUDE_FOLDER \
            -I $GLAD_INCLUDE_PATH \
            -I $IMGUI_INCLUDE_FOLDER \
            \
            -DIMMVISION_CUSTOM_GL_INCLUDE=$IMMVISION_CUSTOM_GL_INCLUDE
        """

echo $cmd
$cmd
echo
echo "immvision.o was produced"
