#!/usr/bin/env python3

import os
import subprocess
import datetime

CURRENT_DIR=os.getcwd()
THIS_DIR=os.path.dirname(os.path.realpath(__file__))
REPO_DIR=THIS_DIR + "/.."
EXTERNAL_DIR=REPO_DIR + "/external"
CURRENT_DIR_IS_REPO_DIR = (os.path.realpath(CURRENT_DIR) == os.path.realpath(REPO_DIR))


os.chdir(REPO_DIR)


def run(cmd):
    subprocess.check_call(cmd, shell=True)


def install_vcpkg_thirdparties(skip_if_present):
    os.chdir(EXTERNAL_DIR)
    if os.path.isdir("vcpkg") and skip_if_present:
        print("install_vcpkg_thirdparties => already present!")
        return

    print("install_vcpkg_thirdparties => Running...")
    run("git clone https://github.com/Microsoft/vcpkg.git")
    os.chdir("vcpkg")

    if os.name == 'nt':
        run("bootstrap-vcpkg.bat")
    else:
        run("./bootstrap-vcpkg.sh")

    if os.name == 'nt':
        run("vcpkg.exe install sdl2:x86-windows opencv4:x86-windows")
        run("vcpkg.exe install sdl2:x64-windows opencv4:x64-windows")
    else:
        run("./vcpkg install sdl2 opencv4")

    os.chdir(REPO_DIR)


def imgui_download(use_powersave: bool, skip_if_present: bool):
    os.chdir(EXTERNAL_DIR)
    if os.path.isdir("imgui") and skip_if_present:
        print("imgui_download => already present!")
        return

    print("imgui_download")
    if (os.path.isdir("imgui")):
        run("rm -rf imgui")
    if use_powersave:
        run("git clone https://github.com/pthom/imgui.git")
        os.chdir("imgui")
        run("git checkout docking_powersave")
    else:
        run("git clone https://github.com/ocornut/imgui.git")
        os.chdir("imgui")
        run("git checkout docking")
    os.chdir(REPO_DIR)


def hello_imgui_download(skip_if_present: bool):
    os.chdir(EXTERNAL_DIR)
    if os.path.isdir("hello_imgui") and skip_if_present:
        print("hello_imgui_download => already present!")
        return
    run("git clone https://github.com/pthom/hello_imgui.git")


def opencv_build_emscripten():
    os.chdir(EXTERNAL_DIR)

    # Git clone
    if not os.path.isdir("opencv"):
        run("git clone https://github.com/opencv/opencv.git")

    if not os.path.isdir("opencv_build_emscripten"):
        os.makedirs("opencv_build_emscripten")
    os.chdir("opencv_build_emscripten")

    cmd = """
    source ~/emsdk/emsdk_env.sh && \
    emcmake cmake ../opencv \
    -DENABLE_PIC=FALSE -DCMAKE_BUILD_TYPE=Release -DCPU_BASELINE='' \
    -DCPU_DISPATCH='' -DCV_TRACE=OFF \
     \
    -DBUILD_SHARED_LIBS=OFF -DWITH_1394=OFF -DWITH_ADE=OFF \
    -DWITH_VTK=OFF -DWITH_EIGEN=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF \
    -DWITH_GTK=OFF -DWITH_GTK_2_X=OFF -DWITH_IPP=OFF -DWITH_JASPER=OFF     \
    -DWITH_JPEG=ON -DWITH_WEBP=OFF -DWITH_OPENEXR=OFF -DWITH_OPENGL=OFF \
    -DWITH_OPENVX=OFF -DWITH_OPENNI=OFF -DWITH_OPENNI2=OFF -DWITH_PNG=ON \
    -DWITH_TBB=OFF -DWITH_TIFF=OFF -DWITH_V4L=OFF -DWITH_OPENCL=OFF \
    -DWITH_OPENCL_SVM=OFF -DWITH_OPENCLAMDFFT=OFF -DWITH_OPENCLAMDBLAS=OFF \
    -DWITH_GPHOTO2=OFF -DWITH_LAPACK=OFF -DWITH_ITT=OFF -DWITH_QUIRC=ON \
    -DBUILD_ZLIB=ON \
    -DBUILD_opencv_apps=OFF -DBUILD_opencv_calib3d=ON -DBUILD_opencv_dnn=ON \
    -DBUILD_opencv_features2d=ON -DBUILD_opencv_flann=ON -DBUILD_opencv_gapi=OFF \
    -DBUILD_opencv_ml=OFF -DBUILD_opencv_photo=ON -DBUILD_opencv_imgcodecs=ON \
    -DBUILD_opencv_shape=OFF -DBUILD_opencv_videoio=OFF -DBUILD_opencv_videostab=OFF \
    -DBUILD_opencv_highgui=ON -DBUILD_opencv_superres=OFF -DBUILD_opencv_stitching=OFF \
    -DBUILD_opencv_java=OFF -DBUILD_opencv_js=ON -DBUILD_opencv_python2=OFF \
    -DBUILD_opencv_python3=OFF -DBUILD_EXAMPLES=ON -DBUILD_PACKAGE=OFF \
    \
    -DBUILD_TESTS=ON -DBUILD_PERF_TESTS=ON -DBUILD_DOCS=OFF -DWITH_PTHREADS_PF=OFF \
    -DCV_ENABLE_INTRINSICS=OFF -DBUILD_WASM_INTRIN_TESTS=OFF \
    \
    -DCMAKE_C_FLAGS='-s USE_PTHREADS=0 ' -DCMAKE_CXX_FLAGS='-s USE_PTHREADS=0 '
    """
    print(cmd)
    run(cmd)

    run("make -j")
    os.chdir(REPO_DIR)


def cmake_desktop_build(use_power_save: bool):
    if CURRENT_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(CURRENT_DIR)
    cmd = f"cmake {REPO_DIR} -DCMAKE_TOOLCHAIN_FILE={EXTERNAL_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    if use_power_save:
        cmd = cmd + " -DIMMVISION_USE_POWERSAVE=ON"
    run(cmd)


def cmake_emscripten_build():
    if CURRENT_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(CURRENT_DIR)
    cmd = f"source ~/emsdk/emsdk_env.sh && emcmake cmake {REPO_DIR} -DOpenCV_DIR={EXTERNAL_DIR}/opencv_build_emscripten"
    run(cmd)


def build_emscripten_with_timestamp():
    if CURRENT_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(CURRENT_DIR)
    now = datetime.datetime.now()
    datestr_file = REPO_DIR + "/src/immvision_demos/inspector_demo/datestr.h"
    with open(datestr_file, "w") as f:
        f.write(f"#define datestr \"{now}\"")
    run("make -j")



if __name__ == "__main__":
    print("Choose a command:")
    print("""
    1.1 Desktop builds: Install vcpkg + opencv and sdl
    1.2 Emscripten build: Dowload and build opencv
    2.1 Download imgui official docking version
    2.2 Download imgui power save version
    3   Download hello_imgui (needed for the viewer)
    4.1 Build / Desktop: run cmake using vcpkg toolchain
    4.2 Build / Desktop using power save: run cmake using vcpkg toolchain
    4.3 Emscripten build: run cmake for emscripten
    4.4 Emscripten: udpate datestr.h & build
    """)
    choice = input("Enter the corresponding number: ")
    if choice == "1.1":
        install_vcpkg_thirdparties(True)
    if choice == "1.2":
        opencv_build_emscripten()
    if choice == "2.1":
        imgui_download(False, True)
    if choice == "2.2":
        imgui_download(True, True)
    if choice == "3":
        hello_imgui_download(True)
    if choice == "4.1":
        cmake_desktop_build(False)
    if choice == "4.2":
        cmake_desktop_build(True)
    if choice == "4.3":
        cmake_emscripten_build()
    if choice == "4.4":
        build_emscripten_with_timestamp()
