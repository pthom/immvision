import imgui;
import cv2;
import _core_immvision_pybind as immvision;
import numpy as np


def test_version():
    assert m.__version__ == "0.0.1"


def test_add():
    assert m.add(1, 2) == 3


def test_sub():
    assert m.subtract(1, 2) == -1
