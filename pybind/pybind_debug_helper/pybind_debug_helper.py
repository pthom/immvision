#!/usr/bin/env python3
import sys
import os

# Argh... the embedded interpreter does not find the pip libraries in this project...
sys.path.append("/Users/pascal/dvp/OpenSource/ImGuiWork/immvision/immvision_pybind/venv/lib/python3.9/site-packages")

THIS_DIR = os.path.dirname(__file__)
path_examples = os.path.realpath(f"{THIS_DIR}/../pybind_examples/")
sys.path.append(path_examples)

#import immvision.cpp_immvision
#print(immvision.cpp_immvision.CvNp_ListTypeSynonyms())

import example_immvision
example_immvision.main()


# sys.path.append(THIS_DIR + "/../cv_np")
# sys.path.append(THIS_DIR + "/../cv_np/tests")
# import test_cv_np_shared_cast
# test_cv_np_shared_cast.main()
