#!/usr/bin/env python3

import os
import subprocess
import datetime
import sys

# global options
SKIP_IF_PRESENT = False
USE_POWERSAVE = True
ACTIVATE_ALL_WARNINGS = True
ONLY_ECHO_COMMAND = False
LINK_STATIC_LIBRARIES = True
ARCH_X64_OR_X86 = "x64" # can be "x64" or "x86"

# Directory global variables
CURRENT_DIR=os.getcwd()
THIS_DIR=os.path.dirname(os.path.realpath(__file__))
REPO_DIR=THIS_DIR + "/.."
EXTERNAL_DIR=REPO_DIR + "/external"
CURRENT_DIR_IS_REPO_DIR = False
if (os.path.realpath(CURRENT_DIR) == os.path.realpath(REPO_DIR)):
    CURRENT_DIR_IS_REPO_DIR = True
if (os.path.realpath(CURRENT_DIR) == os.path.realpath(THIS_DIR)):
    CURRENT_DIR_IS_REPO_DIR = True
if (os.path.realpath(CURRENT_DIR) == os.path.realpath(EXTERNAL_DIR)):
    CURRENT_DIR_IS_REPO_DIR = True


def has_program(program_name):
    paths = os.environ['PATH'].split(":")
    for path in paths:
        prog_path = f"{path}/{program_name}"
        if os.path.exists(prog_path) and os.path.isfile(prog_path):
            return True
    return False

HAS_EMSCRIPTEN = has_program("emcmake")


def print_current_options():
    print()
    print("Current options:")
    print("================")
    print(f"* USE_POWERSAVE (Use imgui power save version): {USE_POWERSAVE}")
    print(f"* SKIP_IF_PRESENT (do not reinstall present third parties): {SKIP_IF_PRESENT}")
    print(f"* ACTIVATE_ALL_WARNINGS: {ACTIVATE_ALL_WARNINGS} (activate many warning, as errors)")
    print(f"* LINK_STATIC_LIBRARIES: {LINK_STATIC_LIBRARIES} (build libs, i.e OpenCV, as static libs)")
    print(f"* ONLY_ECHO_COMMAND: {ONLY_ECHO_COMMAND} (if true, only display required shell commands)")
    print(f"* ARCH_X64_OR_X86: {ARCH_X64_OR_X86}")
    doc_ems = "" if HAS_EMSCRIPTEN else "(run `source emsdk_env.sh` before running this script)"
    print(f"* HAS_EMSCRIPTEN: {HAS_EMSCRIPTEN} {doc_ems}")
    print()

def run(cmd):
    if ONLY_ECHO_COMMAND:
        print("cd " + os.getcwd())
        print(cmd)
    else:
        print("#####################################################")
        print("Running shell command: \n    " + cmd)
        print("in directory " + os.getcwd())
        print("#####################################################")
        subprocess.check_call(cmd, shell=True)


def optional_triplet_name():
    if os.name == 'nt':
        if LINK_STATIC_LIBRARIES:
            return "x86-windows-static-md" if ARCH_X64_OR_X86 == "x86" else "x64-windows-static-md"
        else:
            return "x86-windows" if ARCH_X64_OR_X86 == "x86" else "x64-windows"
    return ""


def install_vcpkg_thirdparties():
    """
    Install vcpkg + opencv and sdl (for desktop builds) into external/vcpkg
    Will create a folder external/vcpkg/ where vcpkg will be installed, with OpenCV and SDL.
    You can skip this part if you already have OpenCV and SDL someewhere else on your system.
    """
    vcpkg_exe = ".\\vcpkg" if os.name == "nt" else "./vcpkg"

    print("# install_vcpkg_thirdparties")
    os.chdir(EXTERNAL_DIR)
    if os.path.isdir("vcpkg") and SKIP_IF_PRESENT:
        print("install_vcpkg_thirdparties => already present!")
        return

    if not os.path.isdir("vcpkg"):
        print("install_vcpkg_thirdparties => Running...")
        run("git clone https://github.com/Microsoft/vcpkg.git")

    os.chdir("vcpkg")

    if os.name == 'nt':
        run(".\\bootstrap-vcpkg.bat")
    else:
        run("./bootstrap-vcpkg.sh")

    if os.name == 'nt':
        packages = ["sdl2", "opencv4[core,jpeg,png]" ]
        triplet = optional_triplet_name()
        for package in packages:
            cmd = f"{vcpkg_exe} install {package}:{triplet}"
            run(cmd)
    else:
        run(f"{vcpkg_exe} install sdl2 opencv4")

    os.chdir(REPO_DIR)


def imgui_download():
    """
    Download imgui into external/imgui/
    Two versions are avaible: 
    * standard version: docking branch of the standard repo (https://github.com/ocornut/imgui.git)
    * power save version: see PR https://github.com/ocornut/imgui/pull/4076
      This PR was adapted to imgui docking version of March 2022 here: 
      https://github.com/pthom/imgui/tree/docking_powersave
    """
    print("# imgui_download")
    os.chdir(EXTERNAL_DIR)
    if os.path.isdir("imgui") and SKIP_IF_PRESENT:
        print("imgui_download => already present!")
        return

    if (os.path.isdir("imgui")):
        run("rm -rf imgui")
    if USE_POWERSAVE:
        run("git clone https://github.com/pthom/imgui.git")
        os.chdir("imgui")
        run("git checkout docking_powersave")
    else:
        run("git clone https://github.com/ocornut/imgui.git")
        os.chdir("imgui")
        run("git checkout docking")
    os.chdir(REPO_DIR)


def hello_imgui_download():
    """
    Download hello_imgui into external/hello_imgui (optional)
    hello_imgui is not required, and is only used to build the demos 
    and the standalone image debug viewer.
    """
    print("# hello_imgui_download")
    os.chdir(EXTERNAL_DIR)
    if os.path.isdir("hello_imgui") and SKIP_IF_PRESENT:
        print("hello_imgui_download => already present!")
        return
    run("git clone https://github.com/pthom/hello_imgui.git")


def opencv_build_emscripten():
    """
    Download and build opencv for emscripten (into external/opencv and external/external/opencv_build_emscripten)
    (only used for emscripten builds, where OpenCV is not available through vcpkg)
    Will clone opencv code into external/opencv, then build it for emscripten into external/opencv_build_emscripten
    """
    print("# opencv_build_emscripten")
    os.chdir(EXTERNAL_DIR)


    # Git clone
    if not os.path.isdir("opencv"):
        run("git clone https://github.com/opencv/opencv.git")
    else:
        if not SKIP_IF_PRESENT:
            run("rm -rf opencv")
            run("git clone https://github.com/opencv/opencv.git")

    # build
    if not os.path.isdir("opencv_build_emscripten"):
        os.makedirs("opencv_build_emscripten")
    os.chdir("opencv_build_emscripten")

    cmd = """emcmake cmake ../opencv \
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
    run(cmd)

    run("make -j")
    os.chdir(REPO_DIR)


def cmake_desktop_build_vcpkg():
    """
    Run cmake with correct flags for a desktop build with vcpkg
    * CMAKE_TOOLCHAIN_FILE will use the vcpkg toolchain
    * IMMVISION_USE_POWERSAVE can optionally activate the power save mode
    Run this script from your desired build dir.
    """
    print("# cmake_desktop_build_vcpkg")
    if CURRENT_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(CURRENT_DIR)
    cmd = f"cmake {REPO_DIR} -DCMAKE_TOOLCHAIN_FILE={EXTERNAL_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"

    triplet = optional_triplet_name()
    if len(triplet) > 0:
        cmd = cmd + f" -DVCPKG_TARGET_TRIPLET={triplet}"

    if USE_POWERSAVE:
        cmd = cmd + " -DIMMVISION_USE_POWERSAVE=ON"

    if ACTIVATE_ALL_WARNINGS:
        cmd = cmd + " -DIMMVISION_ACTIVATE_ALL_WARNINGS=ON"

    cmd = cmd + " -B ."

    if os.name == 'nt':
        generator = "Visual Studio 16 2019"
        arch = "win32" if ARCH_X64_OR_X86 == "x86" else "x64"
        # cmd = cmd + f" -G \"{generator}\""
        cmd = cmd + f" -A {arch}"

    run(cmd)


def cmake_emscripten_build():
    """
    Run cmake with correct flags for a build with emscripten.
    OpenCV_DIR will point to external/opencv_build_emscripten. 
    Run this script from your desired build dir.
    """
    print("# cmake_emscripten_build")
    if CURRENT_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(CURRENT_DIR)
    cmd = f"emcmake cmake {REPO_DIR} -DOpenCV_DIR={EXTERNAL_DIR}/opencv_build_emscripten"
    run(cmd)


def build_emscripten_with_timestamp():
    """
    Helper to build the emscripten version
    This will update src/immvision_demos/inspector_demo/datestr.h and then run `make`
    (this date is displayed in the javascript console at startup to help diagnose issues) 
    You can safely run `make` manually instead.
    """
    print("# build_emscripten_with_timestamp")
    if CURRENT_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(CURRENT_DIR)
    now = datetime.datetime.now()
    datestr_file = REPO_DIR + "/src/immvision_demos/inspector_demo/datestr.h"
    with open(datestr_file, "w") as f:
        f.write(f"#define datestr \"{now}\"")
    run("make -j")


def get_all_function_categories():
    function_list_third_parties = {
        "name": "Install third parties",
        "functions": [
            install_vcpkg_thirdparties,
            imgui_download,
            hello_imgui_download
        ]
    }

    function_list_build = {
        "name": "Build for desktop",
        "functions": [
            cmake_desktop_build_vcpkg,
        ]
    }

    function_list_emscripten = {
        "name": "Build for emscripten",
        "functions": [
            opencv_build_emscripten,
            cmake_emscripten_build,
            build_emscripten_with_timestamp
        ]
    }
    all_function_categories = [function_list_third_parties, function_list_build]
    if HAS_EMSCRIPTEN:
        all_function_categories.append(function_list_emscripten)
    return all_function_categories

def get_all_functions():
    all_functions = []
    for c in get_all_function_categories():
        all_functions = all_functions + (c["functions"])
    return all_functions


def run_interactive():
    print_current_options()

    print("Choose a command:\n")
    all_function_list = []

    print("0. List command line options")
    print()

    i = 0
    for category in get_all_function_categories():
        print(f"{category['name']}")
        print("=================================================")
        for fn in category['functions']:
            doc = fn.__doc__
            doc_lines = doc.split("\n")[1:]
            title = doc_lines[:1][0].strip()
            doc_detail_lines = doc_lines[1:]
            def indent_doc_line(line):
                return "   " + line.strip()
            doc_detail_lines = list(map(indent_doc_line, doc_detail_lines))
            doc_detail = "\n".join(doc_detail_lines)  
            print(f"{i + 1}. {title}")
            print(f"{doc_detail}")
            all_function_list.append(fn)
            i = i + 1
    choice = input("Enter the number corresponding to your choice: ")
    try:
        choice_nb = int(choice) - 1
        if choice == "0":
            help_available_functions()
        elif choice_nb < 0 or choice_nb >= len(all_function_list):
            print(f"Enter a number between 1 and {len(all_function_list)}")
            return
        else:
            fn_to_run = all_function_list[choice_nb]
            fn_to_run()
    except ValueError:
        print("Enter a number!")

def help_available_functions():
    print("Available functions:")
    for fn in get_all_functions():
        print(f"{sys.argv[0]} --{fn.__name__}")
        doc = fn.__doc__
        doc_lines = doc.split("\n")[1:]
        title = doc_lines[:1][0].strip()
        print(f"    {title}")

def run_argv_function(arg1):
    if not arg1[:2] == "--":
        return False
    function_name = arg1[2:]
    found_function = False
    for fn in get_all_functions():
        if fn.__name__ == function_name:
            found_function = True
            fn()
    if not found_function:
        print("Bad argument!")
        help_available_functions()
        return False
    else:
        return True


if __name__ == "__main__":
    if len(sys.argv) == 1:
        run_interactive()
    if len(sys.argv) == 2:
        arg1 = sys.argv[1]
        if arg1 == "-h":
            help_available_functions()
        elif not run_argv_function(arg1):
            help_available_functions()
