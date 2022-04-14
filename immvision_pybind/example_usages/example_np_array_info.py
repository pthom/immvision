import sys

import imgui
import cv2
import os
sys.path.append(os.getcwd())
import _core_immvision_pybind as immvision;
import numpy as np


def main():
    m = np.zeros([480, 640, 3], dtype=float)
    print(immvision.np_array_info(m))

    m = np.zeros([480, 640, 3], dtype=np.float32)
    print(immvision.np_array_info(m))

    m = np.zeros([480, 640], dtype=np.uint16)
    print(immvision.np_array_info(m))


if __name__ == "__main__":
    main()
