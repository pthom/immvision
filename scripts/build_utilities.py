#!/usr/bin/env python3

CMAKE_BUILD_TYPE="Release"

import os
import subprocess
import datetime
import sys
import dataclasses
import typing

@dataclasses.dataclass
class Option:
    Value: bool
    Description: str


@dataclasses.dataclass
class Options:
    ONLY_ECHO_COMMAND:Option = Option(True, "Only echo shell command, but do nothing")

    USE_VCPKG: Option = Option(False, "Use vcpkg package manager")
    USE_CONAN: Option = Option(False, "Use Conan package manager")

    USE_POWERSAVE: Option = Option(True, "Use imgui powersave version")
    ACTIVATE_ALL_WARNINGS:Option = Option(True, "Activate all warnings, as errors")

    WINDOWS_VCPKG_STATIC: Option = Option(True, "Let vcpkg build static libraries under windows")
    BUILD_32BITS: Option = Option(False, "Build 32 bits version (only possible under windows)")

    SHOW_ADVANCED_FUNCTIONS:Option = Option(False, "Show advanced options (used while developing this library)")


OPTIONS = Options()

# Directory global variables
INVOKE_DIR=os.getcwd()
THIS_DIR=os.path.dirname(os.path.realpath(__file__))
REPO_DIR=THIS_DIR + "/.."
EXTERNAL_DIR=REPO_DIR + "/external"
INVOKE_DIR_IS_REPO_DIR = False
if (os.path.realpath(INVOKE_DIR) == os.path.realpath(REPO_DIR)):
    INVOKE_DIR_IS_REPO_DIR = True
if (os.path.realpath(INVOKE_DIR) == os.path.realpath(THIS_DIR)):
    INVOKE_DIR_IS_REPO_DIR = True
if (os.path.realpath(INVOKE_DIR) == os.path.realpath(EXTERNAL_DIR)):
    INVOKE_DIR_IS_REPO_DIR = True


def has_program(program_name):
    paths = os.environ['PATH'].split(":")
    for path in paths:
        prog_path = f"{path}/{program_name}"
        if os.path.exists(prog_path) and os.path.isfile(prog_path):
            return True
    return False


HAS_EMSCRIPTEN = has_program("emcmake")

RUN_LAST_DIRECTORY = "NotSet!!!"


def run(cmd):
    global RUN_LAST_DIRECTORY
    if OPTIONS.ONLY_ECHO_COMMAND.Value:
        if os.getcwd() != RUN_LAST_DIRECTORY:
            print("cd " + os.getcwd())
            RUN_LAST_DIRECTORY = os.getcwd()
        print(cmd)
    else:
        print("#####################################################")
        print("# Running shell command:")
        if os.getcwd() != RUN_LAST_DIRECTORY:
            print("cd " + os.getcwd())
        print(cmd)
        print("#####################################################")
        subprocess.check_call(cmd, shell=True)


######################################################################
# cmake and build
######################################################################
def run_cmake():
    """
    Run cmake with correct flags:
    * -DIMMVISION_USE_POWERSAVE=ON|OFF can optionally activate the power save mode
    * -DIMMVISION_ACTIVATE_ALL_WARNINGS==ON|OFF
    * -A : arch if building for Win32
    Notes:
    * If using vcpkg: CMAKE_TOOLCHAIN_FILE will be set
    * If using conan: `conan install` will be launched
    Run this script from your desired build dir.
    """
    print("# run_cmake")
    if INVOKE_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(INVOKE_DIR)

    if OPTIONS.USE_CONAN.Value:
        print("# Install dependencies via conan")
        run(f"conan install {REPO_DIR}")

    cmake_cmd = f"cmake {REPO_DIR}"

    if OPTIONS.USE_VCPKG.Value:
        cmake_cmd = cmake_cmd + f" -DCMAKE_TOOLCHAIN_FILE={EXTERNAL_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        triplet = _vcpkg_optional_triplet_name()
        if len(triplet) > 0:
            cmake_cmd = cmake_cmd + f" -DVCPKG_TARGET_TRIPLET={triplet}"

    if OPTIONS.USE_POWERSAVE.Value:
        cmake_cmd = cmake_cmd + " -DIMMVISION_USE_POWERSAVE=ON"

    if OPTIONS.ACTIVATE_ALL_WARNINGS.Value:
        cmake_cmd = cmake_cmd + " -DIMMVISION_ACTIVATE_ALL_WARNINGS=ON"

    cmake_cmd = cmake_cmd + f" -DCMAKE_BUILD_TYPE={CMAKE_BUILD_TYPE}"
    cmake_cmd = cmake_cmd + " -B ."

    if os.name == 'nt':
        arch = "win32" if OPTIONS.BUILD_32BITS.Value else "x64"
        cmake_cmd = cmake_cmd + f" -A {arch}"

    run(cmake_cmd)


######################################################################
# imgui and hello_imgui
######################################################################
def _do_clone_repo(git_repo, folder, branch):
    os.chdir(EXTERNAL_DIR)
    if not os.path.isdir(folder):
        run(f"git clone {git_repo}")
    os.chdir(folder)
    run(f"git checkout {branch}")
    run(f"git pull")


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
    git_repo = "https://github.com/pthom/imgui.git" if OPTIONS.USE_POWERSAVE.Value else "https://github.com/ocornut/imgui.git"
    branch = "docking_powersave" if OPTIONS.USE_POWERSAVE.Value else "master"
    os.chdir(EXTERNAL_DIR)

    # Check if we changed to/from powersave mode
    if os.path.isdir("imgui"):
        os.chdir("imgui")
        cmd_result = subprocess.check_output("git branch", shell=True).decode("utf-8")
        cmd_result_lines = cmd_result.split("\n")
        need_reset_repo = False
        if OPTIONS.USE_POWERSAVE.Value and "* docking_powersave" not in cmd_result_lines:
            need_reset_repo = True
        if not OPTIONS.USE_POWERSAVE.Value and "* master" not in cmd_result_lines:
            need_reset_repo = True
        if need_reset_repo:
            os.chdir(EXTERNAL_DIR)
            print("Need to re-download imgui (changed powersave mode)")
            print("Please remove external/imgui manually")
            sys.exit(1)

    _do_clone_repo(git_repo, "imgui", branch)


def hello_imgui_download():
    """
    Download hello_imgui into external/hello_imgui (optional)
    hello_imgui is not required, and is only used to build the demos
    and the standalone image debug viewer.
    """
    print("# hello_imgui_download")
    _do_clone_repo("https://github.com/pthom/hello_imgui.git", "hello_imgui", "master")


def all_imgui_downloads():
    """
    Download imgui and hello_imgui into external/
    """
    imgui_download()
    hello_imgui_download()

######################################################################
# vcpkg
######################################################################
def _vcpkg_optional_triplet_name():
    if os.name == 'nt':
        if OPTIONS.WINDOWS_VCPKG_STATIC.Value:
            return "x86-windows-static-md" if OPTIONS.BUILD_32BITS.Value else "x64-windows-static-md"
        else:
            return "x86-windows" if OPTIONS.BUILD_32BITS.Value else "x64-windows"
    return ""


def vcpkg_install_thirdparties():
    """
    Install vcpkg + opencv and sdl (for desktop builds) into external/vcpkg
    Will create a folder external/vcpkg/ where vcpkg will be installed, with OpenCV and SDL.
    You can skip this part if you already have OpenCV and SDL somewhere else on your system.
    """
    vcpkg_exe = ".\\vcpkg" if os.name == "nt" else "./vcpkg"

    print("# install_vcpkg_thirdparties")
    os.chdir(EXTERNAL_DIR)

    if not os.path.isdir("vcpkg"):
        run("git clone https://github.com/Microsoft/vcpkg.git")

    os.chdir("vcpkg")
    run("git pull")

    if os.name == 'nt':
        run(".\\bootstrap-vcpkg.bat")
    else:
        run("./bootstrap-vcpkg.sh")

    if os.name == 'nt':
        packages = ["sdl2", "opencv4[core,jpeg,png]" ]
        triplet = _vcpkg_optional_triplet_name()
        for package in packages:
            cmd = f"{vcpkg_exe} install {package}:{triplet}"
            run(cmd)
    else:
        run(f"{vcpkg_exe} install sdl2 opencv4")


def vcpkg_export():
    """
    Create a compressed archive of the vcpkg folder.
    Used mainly for CI where vcpkg is deployed by downloading a precompiled vcpkg folder.
    """
    # vcpkg_osx_intel.tgz created with
    # tar --exclude=vcpkg/.git  --exclude=vcpkg/downloads --exclude=vcpkg/buildtrees  -zcvf "vcpkg_osx_intel.tgz" vcpkg
    os.chdir(EXTERNAL_DIR)
    
    # Build vcpkg libraries
    if os.name != "nt":
        vcpkg_install_thirdparties()
    else:
        old_win32 = OPTIONS.BUILD_32BITS.Value
        OPTIONS.BUILD_32BITS.Value = True
        vcpkg_install_thirdparties()
        OPTIONS.BUILD_32BITS.Value = False
        vcpkg_install_thirdparties()
        OPTIONS.BUILD_32BITS.Value = old_win32

    os.chdir(EXTERNAL_DIR)
    if os.path.exists("vcpkg.7z"):
        run("rm vcpkg.7z")
    run(f'7z a -t7z vcpkg_{os.name}.7z vcpkg -xr!"vcpkg/downloads" -xr!"vcpkg/buildtrees" -xr!"vcpkg/.git" -xr!"vcpkg/packages"')
    # scp vcpkg_nt.7z pascal@traineq.org:HTML/ImmvisionGithubFiles/


######################################################################
# emscripten
######################################################################
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

    # build
    if not os.path.isdir("opencv_build_emscripten"):
        os.makedirs("opencv_build_emscripten")
    os.chdir("opencv_build_emscripten")

    cmd = """emcmake cmake ../opencv \
    -DCMAKE_INSTALL_PREFIX=../opencv_install_emscripten \
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
    run("make install")


def cmake_emscripten_build():
    """
    Run cmake with correct flags for a build with emscripten.
    OpenCV_DIR will point to external/opencv_build_emscripten. 
    Run this script from your desired build dir.
    """
    print("# cmake_emscripten_build")
    if INVOKE_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(INVOKE_DIR)
    cmd = f"emcmake cmake {REPO_DIR} -DOpenCV_DIR={EXTERNAL_DIR}/opencv_install_emscripten/lib/cmake/opencv4"
    run(cmd)


def build_emscripten_with_timestamp():
    """
    Helper to build the emscripten version
    This will update src/immvision_demos/inspector_demo/datestr.h and then run `make`
    (this date is displayed in the javascript console at startup to help diagnose issues) 
    You can safely run `make` manually instead.
    """
    print("# build_emscripten_with_timestamp")
    if INVOKE_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    os.chdir(INVOKE_DIR)
    now = datetime.datetime.now()
    datestr_file = REPO_DIR + "/src/immvision_demos/inspector_demo/datestr.h"
    with open(datestr_file, "w") as f:
        f.write(f"#define datestr \"{now}\"")
    run("make -j")

# TODO: add install_emscripten!

######################################################################
# Main
######################################################################
def get_all_function_categories():
    function_list_third_parties = {
        "name": "Install imgui third parties",
        "functions": [
            all_imgui_downloads,
            imgui_download,
            hello_imgui_download,
        ]
    }

    function_list_cmake = {
        "name": "Build with cmake",
        "functions": [
            run_cmake,
        ]
    }

    function_list_vcpkg = {
        "name": "Build with vcpkg",
        "functions": [
            vcpkg_install_thirdparties,
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

    function_list_advanced = {
        "name": "Advanced functions",
        "functions": [
            vcpkg_export,
        ]
    }

    all_function_categories = [function_list_cmake, function_list_third_parties]
    if OPTIONS.USE_VCPKG.Value:
        all_function_categories.append(function_list_vcpkg)
    if HAS_EMSCRIPTEN:
        all_function_categories.append(function_list_emscripten)
    if OPTIONS.SHOW_ADVANCED_FUNCTIONS.Value:
        all_function_categories.append(function_list_advanced)
    return all_function_categories


def get_all_functions():
    all_functions = []
    for c in get_all_function_categories():
        all_functions = all_functions + (c["functions"])
    return all_functions


def display_options_impl(as_command_line_flags: bool):
    option_vars = vars(OPTIONS)
    for option_name, option_variable in option_vars.items():
        option_name = option_name.lower()
        if as_command_line_flags:
            option_intro=f"\t[--{option_name}=True|False]".ljust(40)
            option_desc=option_variable.Description.ljust(60)
            print(f"{option_intro}    {option_desc}\tDefault={option_variable.Value}")
        else:
            option_intro=f"{option_name}={option_variable.Value}".ljust(40)
            option_desc=option_variable.Description
            print(f"{option_intro}\t{option_desc}")
    print(f"Build type: {CMAKE_BUILD_TYPE}  \n\tedit {__file__} to change the build type")


def help_options():
    print("Command line options:")
    display_options_impl(True)


def help_commands():
    print("Available commands: (specify at least one of these commands)")
    for category in get_all_function_categories():
        print(f"\t{category['name']}")
        print("\t=================================================")
        for fn in category['functions']:
            doc_lines = fn.__doc__.split("\n")[1:]
            def indent_doc_line(line):
                return "\t\t" + line.strip()
            doc_detail_lines = list(map(indent_doc_line, doc_lines))
            doc_indented = "\n".join(doc_detail_lines)
            print(f"\t[-{fn.__name__}]")
            print(f"{doc_indented}")


def help():
    help_commands()
    help_options()


def display_current_options():
    print("Current options:")
    display_options_impl(False)


def parse_args_options():
    option_vars = vars(OPTIONS)
    for arg in sys.argv[1:]:
        if not arg.startswith("--"):
            continue
        found_corresponding_option = False
        for option_name, option_variable in option_vars.items():
            option_name = option_name.lower()
            if arg.lower().startswith("--"+ option_name.lower()):
                found_corresponding_option = True
                items = arg.split("=")
                error_message = f"Error: use --{option_name}=True or --{option_name}=False"
                if len(items) != 2:
                    raise ValueError(error_message)
                str_value = items[1].upper()
                if str_value != "TRUE" and str_value != "FALSE":
                    raise ValueError(error_message)
                else:
                    option_variable.Value = True if str_value == "TRUE" else False
        if not found_corresponding_option:
            raise ValueError(f"Bad option: {arg}")


def parse_args_commands() -> typing.List[str] :
    commands = []
    for arg in sys.argv[1:]:
        if arg[:2] == "--":
            continue
        if arg[:1] != "-":
            continue
        found = False
        for function_name in get_all_functions():
            if arg.lower() == "-" + function_name.__name__:
                commands.append(function_name)
                found = True
        if not found:
            raise ValueError(f"command not found: {arg}")
    return commands


def main():
    for arg in sys.argv[1:]:
        if not arg.startswith("-"):
            print(f"Bad argument: {arg}")
            help()
            return 1
    try:
        parse_args_options()
    except ValueError as e:
        print(f"Error: {e}\n")
        help()
        return 1

    try:
        commands = parse_args_commands()
    except ValueError as e:
        print(f"Error: {e}\n")
        help()
        return 1

    if len(commands) == 0:
        help()
        return 1

    for command in commands:
        print(f"# Running command: {command.__name__}")
        command()

    return 0


if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
