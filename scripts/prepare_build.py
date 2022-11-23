#!/usr/bin/env python3
import os.path
import platform

import ci_utils


def install_conan_if_needed():
    if not ci_utils.has_program("conan"):
        if platform.system() == "Darwin":
            ci_utils.run("brew install conan")
        else:
            ci_utils.run("pip3 install conan")
    ci_utils.run("conan profile new default --detect --force")
    if platform.system() == "Linux":
        ci_utils.run("conan profile update settings.compiler.libcxx=libstdc++11 default")


def _install_opencv_conan(build_folder):
    # Make build folder (absolute path)
    if not os.path.isabs(build_folder):
        build_folder = ci_utils.REPO_DIR + "/" + build_folder
    if not os.path.isdir(build_folder):
        os.makedirs(build_folder)
    # Run conan install
    install_conan_if_needed()
    ci_utils.chdir(build_folder)
    conan_file = (
        "../conanfile_opencv_default.txt" if platform.system() == "Windows" else "../conanfile_opencv_minimal.txt"
    )
    ci_utils.run(f"conan install {conan_file} --build missing")


def prepare_build(build_folder="build", with_conan: bool = True):
    _install_opencv_conan(build_folder)


if __name__ == "__main__":
    prepare_build()
