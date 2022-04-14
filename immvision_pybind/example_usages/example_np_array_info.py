import sys

import os
sys.path.append(os.getcwd())

import imgui
import cv2
import numpy as np
import _core_immvision_pybind as immvision;


THIS_DIR = os.path.dirname(os.path.realpath(__file__))

def main():
    m = np.zeros([480, 640, 3], dtype=float)
    print(immvision.np_array_info(m))

    m = np.zeros([480, 640, 3], dtype=np.float32)
    print(immvision.np_array_info(m))

    m = np.zeros([480, 640], dtype=np.uint16)
    print(immvision.np_array_info(m))

    m = cv2.imread(f"{THIS_DIR}/Smiley.png")
    print(immvision.np_array_info(m))

if __name__ == "__main__":
    main()
