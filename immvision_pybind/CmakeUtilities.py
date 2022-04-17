#!/usr/bin/env python3
import sys
import os
import shutil
import subprocess

# cmake will call this with 4 arguments:
# ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} COMMAND

# Post build: deploy library to ${CMAKE_BINARY_DIR}/_pybind/,
# so that we can use it as a python package name immvision


if len(sys.argv) != 5:
    print("This script is intended to be called from cmake!")
    exit(1)

CMAKE_CURRENT_SOURCE_DIR = sys.argv[1]
CMAKE_CURRENT_BINARY_DIR = sys.argv[2]
CMAKE_BINARY_DIR = sys.argv[3]
COMMAND = sys.argv[4]

THIS_DIR = os.path.dirname(__file__)

VENV_PARENT_DIR = THIS_DIR
IS_DOCKER_BUILDER = os.path.isfile("/IMMVISION_DOCKER_BUILDER")
VENV_NAME = "venv" if not IS_DOCKER_BUILDER else "venv_docker"
VENV_DIR = f"{VENV_PARENT_DIR}/{VENV_NAME}"
# use "source" for bash, but for docker we may get "sh" which uses "." instead
SOURCE_PYBIND_VENV = f"source {VENV_DIR}/bin/activate && " if not IS_DOCKER_BUILDER else f".  {VENV_DIR}/bin/activate && "

SILENT = True


# print(f"{immvision_pybind_source_dir=}")
# print(f"{current_bin_dir=}")
# print(f"{bin_dir=}")

# python CmakeUtilities.py /Users/pascal/dvp/OpenSource/ImGuiWork/immvision/immvision_pybind /Users/pascal/dvp/OpenSource/ImGuiWork/immvision/cmake-build-debug/immvision_pybind /Users/pascal/dvp/OpenSource/ImGuiWork/immvision/cmake-build-debug symlink_imgui_libs

def main():
    if COMMAND == "post_build_deploy":
        post_build_deploy()
    elif COMMAND == "symlink_imgui_libs":
        symlink_imgui_libs()
    else:
        raise Exception("Bad command \n" + s)
        exit(1)


def find_imgui_pip_lib_path():
    # print(f"{VENV_DIR=} {SOURCE_PYBIND_VENV=}")
    cmd = f"{SOURCE_PYBIND_VENV} pip show imgui"
    out = subprocess.check_output(cmd, shell=True).decode("UTF-8")
    lines = out.split("\n")
    result = ""
    for line in lines:
        token = "Location: "
        if line.startswith(token):
            result = line[len(token):] + "/imgui"
    return result


def symlink_imgui_libs():
    """
    Dans
    /dvp/sources/immvision_pybind/venv_docker/lib64/python3.10/site-packages/imgui#
    symlinks
    libcore.cpython.so -> core.cpython-310-x86_64-linux-gnu.so
    libinternal.cpython.so -> internal.cpython-310-x86_64-linux-gnu.so

    COMMENT FAIRE
    pip show imgui
    Name: imgui
    Version: 2.0.0
    Summary: Cython-based Python bindings for dear imgui
    Home-page: https://github.com/swistakm/pyimgui
    Author: Michał Jaworski
    Author-email: swistakm@gmail.com
    License: BSD
    Location: /dvp/sources/immvision_pybind/venv_docker/lib/python3.10/site-packages
    """
    if os.name == "nt":
        msg = "symlink_imgui_libs : can't work for nt!"
        print(msg)
        raise Exception(msg)

    if not SILENT:
        print("symlink_imgui_libs")
    imgui_pip_lib_path=  find_imgui_pip_lib_path()
    if not SILENT:
        print(f"{imgui_pip_lib_path=}")

    for filename in os.listdir(imgui_pip_lib_path):
        # print(filename)
        if filename.startswith("core.cpython") and filename.endswith(".so"):
            out_link = f"{imgui_pip_lib_path}/libimgui_core.cpython.so"
            if not os.path.isfile(out_link):
                cmd = f"ln -s {imgui_pip_lib_path}/{filename} {out_link}"
                if not SILENT:
                    print(cmd)
                run(cmd)
        if filename.startswith("internal.cpython") and filename.endswith(".so"):
            out_link = f"{imgui_pip_lib_path}/libimgui_internal.cpython.so"
            if not os.path.isfile(out_link):
                cmd = f"ln -s {imgui_pip_lib_path}/{filename} {out_link}"
                if not SILENT:
                    print(cmd)
                run(cmd)

    print(imgui_pip_lib_path, end="")


def run(cmd):
    if not SILENT:
        print(cmd)
    subprocess.check_call(cmd, shell=True)


def do_symlink(src, dst):
    if os.name == "nt":
        print(f"copying {src} to {dst}")
        shutil.copy(src, dst)
    else:
        if os.path.islink(dst) or os.path.isfile(dst):
            os.remove(dst)
        run(f"ln -s {src} {dst}")


def write_text_file(filename, content):
    with open(filename, "w") as f:
        f.write(content)


def symlink_all_files_of_extension(immvision_pybind_source_dir, extension, dst_dir):
    for filename in os.listdir(immvision_pybind_source_dir):
        full_path = f"{immvision_pybind_source_dir}/{filename}"
        if os.path.isfile(full_path) and full_path.endswith(extension):
            do_symlink(full_path, f"{dst_dir}/{filename}")



def post_build_deploy():
    pybind_libs_path = f"{CMAKE_BINARY_DIR}/_pybind_libs"
    out_dir = f"{pybind_libs_path}/immvision"
    if not os.path.isdir(out_dir):
        os.makedirs(out_dir)


    symlink_all_files_of_extension(CMAKE_CURRENT_BINARY_DIR, "so", out_dir)
    symlink_all_files_of_extension(CMAKE_CURRENT_BINARY_DIR, "pyd", out_dir)
    symlink_all_files_of_extension(f"{CMAKE_CURRENT_SOURCE_DIR}/src/immvision", "py", out_dir)

    # generate autogenerated files _pybind_libs_path.py
    python_code_libs_path = f"""
# Autogenerated file

# This script makes it possible to run the examples without pip install
# It adds a path to sys.path where the compilation occured ({pybind_libs_path})
# if you run your script with `-dev` as a first argument 

import sys
if len(sys.argv) >= 2 and sys.argv[1] == "-dev":
    pybind_libs_path="{pybind_libs_path}"
    sys.path = [ pybind_libs_path ] + sys.path
    """

    print(f"{sys.argv[0]}: generate autogenerated files _pybind_libs_path.py")
    write_text_file(f"{CMAKE_CURRENT_SOURCE_DIR}/debug_helper/_pybind_libs_path.py", python_code_libs_path)
    write_text_file(f"{CMAKE_CURRENT_SOURCE_DIR}/examples/_pybind_libs_path.py", python_code_libs_path)


if __name__ == "__main__":
    main()
