#!/usr/bin/env python3
import sys
import os

THIS_DIR = os.path.dirname(__file__)
path_examples = os.path.realpath(f"{THIS_DIR}/../pybind_examples/")
sys.path.append(path_examples)

import example_immvision
example_immvision.main()

# import immvision
# m = immvision.make_zoom_pan_matrix((12.34, 56.789), 2.25, (300, 200))
# print(m)
