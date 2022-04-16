#!/usr/bin/env python3
import _pybind_libs_path

import sys
import os

# Argh... the embedded interpreter does not find the pip libraries in this project...
sys.path.append("/Users/pascal/dvp/OpenSource/ImGuiWork/immvision/immvision_pybind/venv/lib/python3.9/site-packages")

THIS_DIR = os.path.dirname(__file__)
path_examples = os.path.realpath(f"{THIS_DIR}/../examples/")
sys.path.append(path_examples)

import example_immvision
example_immvision.main()
