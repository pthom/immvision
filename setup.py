import sys
import logging
import os
# from setuptools import find_packages


try:
    from skbuild import setup
except ImportError:
    print(
        "Please update pip, you need pip 10 or greater,\n"
        " or you need to install the PEP 518 requirements in pyproject.toml yourself",
        file=sys.stderr,
    )
    raise


INSTALL_REQUIRES = ['PyOpenGL','pysdl2', 'pysdl2-dll', 'glfw', 'opencv-python', 'numpy', 'matplotlib', 'imgui @ git+https://github.com/pthom/pyimgui.git@pthom/docking_powersave']
PACKAGES = ["immvision", "immvision/imgui_runner"]


if "IMMVISION_PIP_EDITABLE" in os.environ:
    logging.warning("""
    IMMVISION_PIP_EDITABLE is set: skipping requirements.Use
        unset IMMVISION_PIP_EDITABLE 
    to deactivate this""")

    # Speedup build in editable mode. Make sure to install the requirements manually before
    INSTALL_REQUIRES = []

    # Warning! The package list should be :
    #       packages= ["immvision", "immvision/imgui_runner"]
    # However, the pip editable mode fails when there are several packages (or subpackages)
    PACKAGES= ["immvision"]
else:
    logging.warning("""
    IMMVISION_PIP_EDITABLE is not set. Use
        export IMMVISION_PIP_EDITABLE=1
    to activate it.""")


setup(
    name="immvision",
    version="0.0.1",
    description="immvision: immediate image debugger and insights",
    author="Pascal Thomet",
    license="",

    packages= PACKAGES,
    install_requires= INSTALL_REQUIRES,

    package_dir={"": "pybind/src_python"},
    cmake_install_dir="pybind/src_python/immvision",
    include_package_data=True,
    extras_require={"test": ["pytest"]},
    python_requires=">=3.6",
)
