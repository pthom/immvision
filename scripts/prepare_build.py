#!/usr/bin/env python3
import os.path
import platform

import ci_utils


def install_conan_if_needed():
    if not ci_utils.has_program("conan"):
        ci_utils.run("pip3 install conan")
    ci_utils.run("conan profile new default --detect --force")
    if platform.system() == "Linux":
        ci_utils.run("conan profile update settings.compiler.libcxx=libstdc++11 default")


def prepare_build(build_folder="build"):
    ci_utils.do_clone_repo("https://github.com/pthom/imgui_bundle.git", "imgui_bundle", pull=True, init_submodules=True)

    # Make build folder (absolute path)
    if not os.path.isabs(build_folder):
        build_folder = ci_utils.REPO_DIR + "/" + build_folder
    if not os.path.isdir(build_folder):
        os.makedirs(build_folder)

    # Run conan install
    install_conan_if_needed()
    ci_utils.chdir(build_folder)
    ci_utils.run("conan install .. --build outdated")


if __name__ == "__main__":
    prepare_build()
