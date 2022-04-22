#!/usr/bin/env python3
import os
import subprocess
import datetime
import sys
import dataclasses
import typing
from pathlib import Path

CMAKE_BUILD_TYPE = "Release"


@dataclasses.dataclass
class Option:
    Value: bool
    Description: str


@dataclasses.dataclass
class Options:
    only_echo_command: Option = Option(True, "Only echo shell command, but do nothing")

    use_vcpkg: Option = Option(False, "Use vcpkg package manager")
    use_conan: Option = Option(False, "Use Conan package manager")

    use_powersave: Option = Option(True, "Use imgui powersave version")
    activate_all_warnings: Option = Option(True, "Activate all warnings, as errors")

    # advanced options below
    build_emscripten: Option = Option(False, "Build for emscripten")
    windows_vcpkg_static: Option = Option(True, "Let vcpkg build static libraries under windows")
    build_32bits: Option = Option(False, "Build 32 bits version (only possible under windows)")
    vcpkg_bypass_install: Option = Option(False, "Bypass vcpkg install (advanced option, for CI only)")
    emscripten_bypass_opencv_compil: Option = Option(False, "Skip opencv compil for emscripten (advanced, for CI only)")
    build_python_bindings: Option = Option(False, "Build python library using pybind11")


OPTIONS = Options()

# Directory global variables
INVOKE_DIR = os.getcwd()
THIS_DIR = os.path.dirname(os.path.realpath(__file__))
REPO_DIR = os.path.realpath(THIS_DIR + "/..")
EXTERNAL_DIR = REPO_DIR + "/external"
INVOKE_DIR_IS_REPO_DIR = False
HOME_FOLDER = Path.home()
if os.path.realpath(INVOKE_DIR) == os.path.realpath(REPO_DIR):
    INVOKE_DIR_IS_REPO_DIR = True
if os.path.realpath(INVOKE_DIR) == os.path.realpath(THIS_DIR):
    INVOKE_DIR_IS_REPO_DIR = True
if os.path.realpath(INVOKE_DIR) == os.path.realpath(EXTERNAL_DIR):
    INVOKE_DIR_IS_REPO_DIR = True

IS_DOCKER_BUILDER = os.path.isfile("/IMMVISION_DOCKER_BUILDER")

VENV_PARENT_DIR = f"{REPO_DIR}"
VENV_NAME = "venv" if not IS_DOCKER_BUILDER else "venv_docker"
VENV_DIR = f"{VENV_PARENT_DIR}/{VENV_NAME}"
# use "source" for bash, but for docker we may get "sh" which uses "." instead
source_cmd = ". "
VENV_RUN_SOURCE = f"{source_cmd} {VENV_DIR}/bin/activate && "
VENV_PACKAGES_DIR = f"{VENV_DIR}/lib/python{sys.version_info.major}.{sys.version_info.minor}/site-packages"

VCPKG_BASENAME = "vcpkg" if not IS_DOCKER_BUILDER else "vcpkg_docker"
VCPKG_DIR = f"{REPO_DIR}/external/{VCPKG_BASENAME}"


def has_program(program_name):
    paths = os.environ['PATH'].split(":")
    for path in paths:
        prog_path = f"{path}/{program_name}"
        if os.path.exists(prog_path) and os.path.isfile(prog_path):
            return True
    return False


HAS_EMSCRIPTEN = has_program("emcmake")

CHDIR_LAST_DIRECTORY = INVOKE_DIR


def my_chdir(folder):
    global CHDIR_LAST_DIRECTORY
    try:
        os.chdir(folder)
    except FileNotFoundError:
        if OPTIONS.only_echo_command.Value:
            print(f"# Warning, folder {folder} does not yet exist")
        else:
            raise FileNotFoundError(f"# Cannot chdir to folder {folder} !")
    if OPTIONS.only_echo_command.Value and folder != CHDIR_LAST_DIRECTORY:
        print(f"cd {folder}")
    CHDIR_LAST_DIRECTORY = folder


def run(cmd, chain_commands=False):
    if OPTIONS.only_echo_command.Value:
        print(cmd)
        return

    if chain_commands and os.name == "nt":
        raise RuntimeError("chain_commands is not supported under windows")

    if chain_commands:
        cmd = _chain_and_echo_commands(cmd)
        subprocess.check_call(cmd, shell=True)
    else:
        if os.name == "nt":
            print("###### Run command ######")
            print(cmd)
            print()
            subprocess.check_call(cmd, shell=True)
        else:
            cmd = _cmd_to_echo_and_cmd(cmd)
            subprocess.check_call(cmd, shell=True)


def _cmd_to_echo_and_cmd_lines(cmd: str) -> [str]:
    lines_with_echo = [
        "echo '###### Run command ######'",
        f"echo '{cmd}'",
        "echo ''",
        cmd
    ]
    return lines_with_echo


def _cmd_to_echo_and_cmd(cmd: str) -> str:
    end_line = " &&         \\\n"
    return end_line.join(_cmd_to_echo_and_cmd_lines(cmd))


def _chain_and_echo_commands(commands: str):
    """
    Take a series of shell command on a multiline string (one command per line)
    and returns a shell command that will execute each of them in sequence,
    while echoing them, and ignoring commented lines (with a #)    
    """
    lines = commands.split("\n")
    # strip lines
    lines = map(lambda s: s.strip(), lines)
    # suppress empty lines
    lines = filter(lambda s: not len(s) == 0, lines)

    # add "echo commands" and process comments:
    # comments starting with # are discarded and comments starting with ## are displayed loudly
    lines_with_echo = []
    for line in lines:
        if line.startswith("##"):
            echo_line = f"echo '******************** {line[2:].strip()} ***************'"
            lines_with_echo.append(echo_line)
        elif not line.startswith("#"):
            lines_with_echo = lines_with_echo + _cmd_to_echo_and_cmd_lines(line)

    # End of line joiner
    end_line = " &&         \\\n"

    r = end_line.join(lines_with_echo)
    r = r.replace("&& &&", "&& ")
    return r


def decorate_loudly_echo_function_name(fn):
    def wrapper_func(*args, **kwargs):
        msg = f"""
# ==================================================================
#                {fn.__name__}
# =================================================================="""
        if os.name == "nt":
            print(msg)
        else:
            for line in msg.split("\n"):
                subprocess.check_call(f"echo '{line}'", shell=True)

        return fn(*args, **kwargs)
    wrapper_func.__doc__ = fn.__doc__
    wrapper_func.__name__ = fn.__name__
    return wrapper_func


######################################################################
# cmake and build
######################################################################
@decorate_loudly_echo_function_name
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
    if INVOKE_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    my_chdir(INVOKE_DIR)

    if OPTIONS.build_emscripten.Value:
        _complain_and_exit_if_emscripten_absent()

    if OPTIONS.use_conan.Value:
        print("# Install dependencies via conan")
        run(f"conan install {REPO_DIR} --build=missing")

    if os.name == "nt":
        new_line = "    "
    else:
        new_line = "    \\\n     "

    cmake_cmd = "emcmake cmake" if OPTIONS.build_emscripten.Value else "cmake"
    cmake_cmd = cmake_cmd + f" {REPO_DIR}"

    if OPTIONS.use_vcpkg.Value:
        cmake_cmd = cmake_cmd + f"{new_line} -DCMAKE_TOOLCHAIN_FILE={VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake"
        triplet = _vcpkg_optional_triplet_name()
        if len(triplet) > 0:
            cmake_cmd = cmake_cmd + f"{new_line} -DVCPKG_TARGET_TRIPLET={triplet}"

    if OPTIONS.build_emscripten.Value:
        cmake_cmd = cmake_cmd + f"{new_line} -DOpenCV_DIR={EXTERNAL_DIR}/opencv_install_emscripten/lib/cmake/opencv4"

    if OPTIONS.use_powersave.Value:
        cmake_cmd = cmake_cmd + f"{new_line} -DIMMVISION_USE_POWERSAVE=ON"

    if OPTIONS.activate_all_warnings.Value:
        cmake_cmd = cmake_cmd + f"{new_line} -DIMMVISION_ACTIVATE_ALL_WARNINGS=ON"

    if OPTIONS.build_python_bindings.Value:
        cmake_cmd = cmake_cmd + f"{new_line} -DIMMVISION_BUILD_PYTHON_BINDINGS=ON"
        cmake_cmd = cmake_cmd + f"{new_line} -DPYTHON_EXECUTABLE={VENV_DIR}/bin/python"

    cmake_cmd = cmake_cmd + f"{new_line} -DCMAKE_BUILD_TYPE={CMAKE_BUILD_TYPE}"
    cmake_cmd = cmake_cmd + f"{new_line} -B ."

    if os.name == 'nt':
        arch = "win32" if OPTIONS.build_32bits.Value else "x64"
        cmake_cmd = cmake_cmd + f"{new_line} -A {arch}"

    if OPTIONS.build_python_bindings.Value:
        cmake_cmd = f"{VENV_RUN_SOURCE} {cmake_cmd}"
    run(cmake_cmd)


@decorate_loudly_echo_function_name
def run_build():
    """
    Build the project using:
    * "make -j" under Linux and MacOS
    * "cmake --build . --config {CMAKE_BUILD_TYPE}" under Windows
    """
    if INVOKE_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    my_chdir(INVOKE_DIR)
    if os.name == "nt":
        run(f"cmake --build . --config {CMAKE_BUILD_TYPE}")
    else:
        run("make -j")


def run_build_all():
    """
    All in one build process:
    * Install third parties imgui and hello_imgui
    * Install opencv and SDL (via vcpkg, conan, or via apt packages)
        (Do set --use_vcpkg=True or --use_conan=True to activate vcpkg or conan)
    * Run cmake
    * Launch the build
    """
    if INVOKE_DIR_IS_REPO_DIR:
        print("Run this from your build dir!")
        return
    my_chdir(INVOKE_DIR)

    # Install third parties imgui and hello_imgui
    all_imgui_downloads()

    # Create virtual env for pybind
    if OPTIONS.build_python_bindings.Value:
        pybind_make_venv()

    # Install opencv and SDL (via vcpkg, conan, or via apt packages)
    if OPTIONS.use_vcpkg.Value and not OPTIONS.vcpkg_bypass_install.Value:
        vcpkg_install_thirdparties()
    elif not OPTIONS.use_conan.Value and not OPTIONS.build_emscripten.Value:
        _propose_install_opencv_sdl_for_ubuntu()
    elif OPTIONS.build_emscripten.Value and not OPTIONS.emscripten_bypass_opencv_compil.Value:
        opencv_build_emscripten()

    # Run cmake
    run_cmake()

    # Launch the build
    run_build()


######################################################################
# imgui and hello_imgui
######################################################################
def _do_clone_repo(git_repo, folder, branch_or_tag):
    if not os.path.isdir(f"{EXTERNAL_DIR}/{folder}"):
        my_chdir(EXTERNAL_DIR)
        run(f"git clone {git_repo} {folder}")
    my_chdir(f"{EXTERNAL_DIR}/{folder}")
    run(f"git checkout {branch_or_tag}")
    run(f"git pull")


@decorate_loudly_echo_function_name
def imgui_download():
    """
    Download imgui into external/imgui/
    Two versions are avaible:
    * standard version: docking branch of the standard repo (https://github.com/ocornut/imgui.git)
    * power save version: see PR https://github.com/ocornut/imgui/pull/4076
      This PR was adapted to imgui docking version of March 2022 here:
      https://github.com/pthom/imgui/tree/docking_powersave
    """
    git_repo = "https://github.com/pthom/imgui.git" if OPTIONS.use_powersave.Value \
        else "https://github.com/ocornut/imgui.git"
    branch = "docking_powersave" if OPTIONS.use_powersave.Value else "docking"

    # Check if we changed to/from powersave mode
    if os.path.isdir(f"{EXTERNAL_DIR}/imgui"):
        previous_dir = os.getcwd()
        os.chdir(f"{EXTERNAL_DIR}/imgui")
        cmd_result = subprocess.check_output("git branch", shell=True).decode("utf-8")
        cmd_result_lines = cmd_result.split("\n")
        os.chdir(previous_dir)
        need_reset_repo = False
        if OPTIONS.use_powersave.Value and "* docking_powersave" not in cmd_result_lines:
            need_reset_repo = True
        if not OPTIONS.use_powersave.Value and "* docking" not in cmd_result_lines:
            need_reset_repo = True
        if need_reset_repo:
            my_chdir(EXTERNAL_DIR)
            print("# Need to re-download imgui (changed powersave mode)")
            print(f"""
            # Please remove external/imgui manually:
                cd {EXTERNAL_DIR}
                rm -rf imgui
                cd -
            """)
            sys.exit(1)

    _do_clone_repo(git_repo, "imgui", branch)


@decorate_loudly_echo_function_name
def hello_imgui_download():
    """
    Download hello_imgui into external/hello_imgui (optional)
    hello_imgui is not required, and is only used to build the demos
    and the standalone image debug viewer.
    """
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
        if OPTIONS.windows_vcpkg_static.Value:
            return "x86-windows-static-md" if OPTIONS.build_32bits.Value else "x64-windows-static-md"
        else:
            return "x86-windows" if OPTIONS.build_32bits.Value else "x64-windows"
    return ""


@decorate_loudly_echo_function_name
def vcpkg_install_thirdparties():
    """
    Install vcpkg + opencv and sdl (for desktop builds) into external/vcpkg
    Will create a folder external/vcpkg/ where vcpkg will be installed, with OpenCV and SDL.
    You can skip this part if you already have OpenCV and SDL somewhere else on your system.
    """
    vcpkg_exe = ".\\vcpkg" if os.name == "nt" else "./vcpkg"

    if not os.path.isdir(f"{VCPKG_DIR}"):
        my_chdir(EXTERNAL_DIR)
        _do_clone_repo("https://github.com/Microsoft/vcpkg.git", VCPKG_BASENAME, "master")
        my_chdir(f"{VCPKG_DIR}")
        if os.name == 'nt':
            run(".\\bootstrap-vcpkg.bat")
        else:
            run("./bootstrap-vcpkg.sh")

    my_chdir(f"{VCPKG_DIR}")
    run("git pull")

    packages = ["sdl2", "opencv4[core,jpeg,png]"]
    for package in packages:
        triplet = _vcpkg_optional_triplet_name()
        if len(triplet) > 0:
            cmd = f'{vcpkg_exe} install "{package}:{triplet}"'
        else:
            cmd = f'{vcpkg_exe} install "{package}"'
        run(cmd)


def vcpkg_export():
    """
    (Advanced function) Create a compressed archive of the vcpkg folder
    where build and temporary folder are excluded.
    Used mainly for CI where vcpkg is deployed by downloading a precompiled vcpkg folder.
    """
    my_chdir(EXTERNAL_DIR)
    
    # Build vcpkg libraries
    if os.name != "nt":
        vcpkg_install_thirdparties()
    else:
        old_win32 = OPTIONS.build_32bits.Value
        OPTIONS.build_32bits.Value = True
        vcpkg_install_thirdparties()
        OPTIONS.build_32bits.Value = False
        vcpkg_install_thirdparties()
        OPTIONS.build_32bits.Value = old_win32

    my_chdir(EXTERNAL_DIR)
    if os.path.exists("vcpkg.7z"):
        run("rm vcpkg.7z")
    run(f'7z a -t7z vcpkg_{os.name}.7z vcpkg -xr!"vcpkg/downloads" -xr!"vcpkg/buildtrees" -xr!"vcpkg/.git" -xr!"vcpkg/packages"')
    # scp vcpkg_nt.7z pascal@traineq.org:HTML/ImmvisionGithubFiles/


######################################################################
# ubuntu
######################################################################
def _is_ubuntu():
    lsb_file = "/etc/lsb-release"
    if not os.path.isfile(lsb_file):
        return False
    with open(lsb_file, "r") as f:
        lines = f.readlines()
        for line in lines:
            if "Ubuntu" in line:
                return True
    return False


def _has_ubuntu_package_opencv_sdl():
    if not _is_ubuntu():
        return False
    try:
        subprocess.check_call("dpkg -L libopencv-dev > /dev/null", shell=True)
        subprocess.check_call("dpkg -L libsdl2-dev > /dev/null", shell=True)
    except subprocess.CalledProcessError:
        return False
    return True


def _propose_install_opencv_sdl_for_ubuntu():
    if not _is_ubuntu():
        return
    if not _has_ubuntu_package_opencv_sdl():
        print("""
        #
        # You need to install opencv and sdl2:
        # * Either use Conan (--use_conan=True) or Vcpkg (--use_vcpkg=True)
        # * Or install opencv and sdl2 packages:
        #        Under Ubuntu, run:
        #            sudo apt-get update && sudo apt-get install -y libopencv-dev libsdl2-dev
        #
        """)


######################################################################
# emscripten
######################################################################
def _complain_and_exit_if_emscripten_absent():
    if not HAS_EMSCRIPTEN:
        if os.path.isdir(f"{HOME_FOLDER}/emsdk"):
            print(f"""
            Emscripten is not activated! Before running this script, call
                source {HOME_FOLDER}/emsdk/emsdk_env.sh
            .
            """)
            exit(1)
        else:
            print(f"""
            Emscripten is not installed or not activated.

            1. First, install emscripten :
                * Either using this script, which will install it in one step inside {HOME_FOLDER}/emsdk. Run:
                        {sys.argv[0]} -install_emscripten  
                * Or following the official docs: https://emscripten.org/docs/getting_started/downloads.html

            2. Then, activate emscripten: before running this script. Call:
                    source {HOME_FOLDER}/emsdk/emsdk_env.sh
            """)
            exit(1)


@decorate_loudly_echo_function_name
def install_emscripten():
    """
    Will install emscripten into {HOME_FOLDER}/emsdk
    """
    if os.name == "nt":
        print("This was not tested under Windows!")
        exit(1)
    if not os.path.isdir(f"{HOME_FOLDER}/emsdk"):
        my_chdir(HOME_FOLDER)
        run("git clone https://github.com/emscripten-core/emsdk.git")
    my_chdir(f"{HOME_FOLDER}/emsdk")
    # Fetch the latest version of the emsdk (not needed the first time you clone)
    run("git pull")
    # Download and install the latest SDK tools.
    run("./emsdk install latest")
    # Make the "latest" SDK "active" for the current user. (writes .emscripten file)
    run("./emsdk activate latest")


@decorate_loudly_echo_function_name
def opencv_build_emscripten():
    """
    Download and build opencv for emscripten (into external/opencv and external/external/opencv_build_emscripten)
    (only used for emscripten builds, where OpenCV is not available through vcpkg)
    Will clone opencv code into external/opencv, then build it for emscripten into external/opencv_build_emscripten
    """
    _complain_and_exit_if_emscripten_absent()

    # Git clone
    if not os.path.isdir(f"{EXTERNAL_DIR}/opencv"):
        my_chdir(EXTERNAL_DIR)
        run("git clone https://github.com/opencv/opencv.git")

    # build
    if not os.path.isdir(f"{EXTERNAL_DIR}/opencv_build_emscripten"):
        my_chdir(EXTERNAL_DIR)
        run("mkdir -p opencv_build_emscripten")

    my_chdir(f"{EXTERNAL_DIR}/opencv_build_emscripten")

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


@decorate_loudly_echo_function_name
def emscripten_update_timestamp():
    """
    (Advanced) Helper to build the emscripten version
    This will update src/immvision_demos/inspector_demo/datestr.h with the latest date
    (this date is displayed in the javascript console at startup to help diagnose issues)
    """
    _complain_and_exit_if_emscripten_absent()
    now = datetime.datetime.now()
    datestr_file = f"{REPO_DIR}/src/immvision_demos/inspector_demo/datestr.h"
    with open(datestr_file, "w") as f:
        f.write(f"#define datestr \"{now}\"")


######################################################################
# pybind
######################################################################


@decorate_loudly_echo_function_name
def pybind_make_venv():
    """
    Creates a python virtual environment used to build python bindings,
    inside immvision_pybind/venv
    """
    my_chdir(f"{VENV_PARENT_DIR}")
    if not os.path.isdir(VENV_NAME):
        run(f"python3 -m venv {VENV_NAME}")
    cmd = f"{VENV_RUN_SOURCE} pip install -v -r {REPO_DIR}/pybind/requirements_dev_pybind.txt"
    run(cmd)

    print(f"""
    Now, activate your python venv with:
    
        source {VENV_DIR}/bin/activate 
    """)


@decorate_loudly_echo_function_name
def pybind_clone_pyimgui():
    """
    (Optional, for pybind building only) Clone pyimgui
    pyimgui's pip version includes a version of imgui that is too old.
    For development only, since scripts/requirements_dev_pybind.txt will install the correct version,
    """
    # We use a fork of pyimgui: https://github.com/pthom/pyimgui.git@pthom/docking_2022_04_05
    # proposed as a PR here: https://github.com/pyimgui/pyimgui/pull/274
    my_chdir(EXTERNAL_DIR)
    _do_clone_repo("https://github.com/pthom/pyimgui.git", "pyimgui", "pthom/docking_powsersave")
    my_chdir("pyimgui")
    run("git submodule update --init")
    run(f"{VENV_RUN_SOURCE} pip install .")


def _pybind_pip_install(editable: bool):
    """
    Runs `pip install` in the main directory and checks that the module works
    """
    def ls_echo_dir(folder):
        return f"echo ls {folder}: && echo ------------ && ls -alh {folder}"

    editable_flag = "--editable" if editable else ""
    ls_install_dir = ls_echo_dir(f"{REPO_DIR}/pybind/pybind_src/immvision") if editable \
        else ls_echo_dir(f"{VENV_PACKAGES_DIR}/immvision")

    my_chdir(REPO_DIR)
    commands = f"""
        ## Suppress temp build dir
        rm -rf {REPO_DIR}/_skbuild
        ## create virtual environment in venv
        python3 -m venv {VENV_NAME}
        ## Use virtual env venv in later commands
        {VENV_RUN_SOURCE}
        ## Clean previously installed package
        rm -rf {VENV_PACKAGES_DIR}/immvision 
        ## Do install
        pip install -v {editable_flag} .
        ## List files (for debug) 
        {ls_install_dir}
        ## Test if we can import the module
        python3 -c 'import immvision'
        python3 -c 'import immvision.test'
    """
    run(commands, chain_commands=True)


def pybind_pip_install_editable():
    """
    Runs `pip install --editable .` in the main directory and checks that the module works
    """
    _pybind_pip_install(True)


def pybind_pip_install():
    """
    Runs `pip install .` in the main directory and checks that the module works
    """
    _pybind_pip_install(False)


######################################################################
# Main
######################################################################
def get_all_function_categories():
    function_list_all_in_one = {
        "name": "All in one build process",
        "functions": [
            run_build_all,
        ]
    }

    function_list_third_parties = {
        "name": "Install imgui third parties",
        "functions": [
            all_imgui_downloads,
            imgui_download,
            hello_imgui_download,
        ]
    }

    function_list_build = {
        "name": "Run cmake and build project",
        "functions": [
            run_cmake,
            run_build
        ]
    }

    function_list_vcpkg = {
        "name": "Build vcpkg packages (if --use_vcpkg=True)",
        "functions": [
            vcpkg_install_thirdparties,
        ]
    }

    function_list_emscripten = {
        "name": """Build for emscripten (activate your emscripten environment before!)""",
        "functions": [
            install_emscripten,
            opencv_build_emscripten,
            emscripten_update_timestamp
        ]
    }

    function_list_advanced = {
        "name": "Advanced functions",
        "functions": [
            vcpkg_export,
        ]
    }

    function_list_pybind = {
        "name": "Functions to build python bindings (immvision_pybind)",
        "functions": [
            pybind_make_venv,
            pybind_clone_pyimgui,
            pybind_pip_install,
            pybind_pip_install_editable
        ]
    }

    all_function_categories = [
        function_list_all_in_one,
        function_list_build, function_list_third_parties,
        function_list_vcpkg, function_list_emscripten,
        function_list_advanced,
        function_list_pybind
    ]
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
            option_intro = f"\t[--{option_name}=True|False]".ljust(40)
            option_desc = option_variable.Description.ljust(60)
            print(f"{option_intro}    {option_desc}\tDefault={option_variable.Value}")
        else:
            option_intro = f"{option_name}={option_variable.Value}".ljust(40)
            option_desc = option_variable.Description
            print(f"{option_intro}\t{option_desc}")
    print(f"Build type: {CMAKE_BUILD_TYPE}  \n\tedit {__file__} to change the build type")


def help_options():
    print("Command line options:")
    display_options_impl(True)


def help_commands():
    print("Available commands: (specify at least one of these commands)")
    for category in get_all_function_categories():
        print(f"\t{category['name']}")
        print("\t==================================================================================================")
        for fn in category['functions']:
            doc_lines = fn.__doc__.split("\n")[1:]

            def indent_doc_line(line):
                return "\t\t" + line.strip()
            doc_detail_lines = list(map(indent_doc_line, doc_lines))
            doc_indented = "\n".join(doc_detail_lines)
            print(f"\t[-{fn.__name__}]")
            print(f"{doc_indented}")


def show_help():
    help_commands()
    help_options()
    if not HAS_EMSCRIPTEN:
        print("Emscripten: activate emscripten in order to get related utilities, first : for example\n\
        source ~/emsdk/emsdk_env.sh")
    if OPTIONS.only_echo_command.Value:
        print(f"""
        Currently, commands are only output to the terminal. To run them, add 'run' as first param i.e 
                {sys.argv[0]} run ...
        """)


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
            if arg.lower().startswith("--" + option_name.lower()):
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

    if OPTIONS.use_conan.Value and not has_program("conan"):
        print("You need to install conan! See https://conan.io/ for instructions")
        exit(1)


def parse_args_commands() -> typing.List[typing.Callable]:
    commands = []
    for arg in sys.argv[1:]:
        if arg[:2] == "--":
            continue
        if arg[:1] != "-":
            continue
        found = False
        for function_pointer in get_all_functions():
            if arg.lower() == "-" + function_pointer.__name__:
                commands.append(function_pointer)
                found = True
        if not found:
            raise ValueError(f"command not found: {arg}")
    return commands


def main():
    if len(sys.argv) >= 2 and sys.argv[1].lower() == "run":
        OPTIONS.only_echo_command.Value = False
        sys.argv = sys.argv[0:1] + sys.argv[2:]
    for arg in sys.argv[1:]:
        if not arg.startswith("-"):
            print(f"Bad argument: {arg}")
            show_help()
            return 1
    try:
        parse_args_options()
    except ValueError as e:
        print(f"Error: {e}\n")
        show_help()
        return 1

    try:
        commands = parse_args_commands()
    except ValueError as e:
        print(f"Error: {e}\n")
        show_help()
        return 1

    if len(commands) == 0:
        show_help()
        return 1

    for command in commands:
        print(f"# Running command: {command.__name__}")
        command()

    return 0


if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
