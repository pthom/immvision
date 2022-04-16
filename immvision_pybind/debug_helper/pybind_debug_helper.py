import sys
import os

import sys
sys.path.append(".")

THIS_DIR = os.path.dirname(__file__)
REPO_DIR = os.path.realpath(THIS_DIR + "/../..")

path_src_immvision = os.path.realpath(f"{REPO_DIR}/immvision_pybind/src/immvision")
path_compilation = REPO_DIR + "/cmake-build-debug/immvision_pybind"
path_examples = os.path.realpath(f"{REPO_DIR}/immvision_pybind/example_usages")
sys.path.append(path_src_immvision)
sys.path.append(path_compilation)
sys.path.append(path_examples)

sys.path.append("/Users/pascal/dvp/OpenSource/ImGuiWork/immvision/immvision_pybind/venv/lib/python3.9/site-packages")

import example_immvision
example_immvision.main()

