import sys

try:
    from skbuild import setup
except ImportError:
    print(
        "Please update pip, you need pip 10 or greater,\n"
        " or you need to install the PEP 518 requirements in pyproject.toml yourself",
        file=sys.stderr,
    )
    raise

from setuptools import find_packages

setup(
    name="immvision",
    version="0.0.1",
    description="immvision: immediate image debugger and insights",
    author="Pascal Thomet",
    license="",
    install_requires=['PyOpenGL','pysdl2', 'opencv-python', 'imgui @ git+https://github.com/pthom/pyimgui.git@pthom/docking_powersave'],
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    cmake_install_dir="src/immvision",
    include_package_data=True,
    extras_require={"test": ["pytest"]},
    python_requires=">=3.6",
)
