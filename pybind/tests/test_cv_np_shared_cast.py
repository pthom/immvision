import pytest

import immvision
import numpy as np
import math


def are_float_close(x: float, y: float):
    return math.fabs(x - y) < 1E-5



"""
We are playing with this C++ class

struct StructTest_CvNpShared
{
    // Create a mat with 3 rows, 4 columns and 1 channel
    // its shape for numpy should be (3, 4)
    cv::Mat m = cv::Mat::eye(cv::Size(4, 3), CV_8UC1);
    void SetM(int row, int col, uchar v) { m.at<uchar>(row, col) = v; }

    cv::Matx32d mx = cv::Matx32d::eye();
    void SetMX(int row, int col, double v) { mx(row, col) = v;}
};
"""
def test_mat():
    # create object
    o: np.ndarray = immvision.cpp_immvision.StructTest_CvNpShared()
    assert o.m.shape == (3, 4)

    # play with its internal cv::Mat

    # From python, change value in the C++ Mat (o.m) and assert that the changes are applied, and visible from python
    o.m[0, 0] = 2
    assert o.m[0, 0] == 2

    # Make a python linked copy of the C++ Mat, named m_linked.
    # Values of m_mlinked and the C++ mat should change together
    m_linked = o.m
    m_linked[1, 1] = 3
    assert o.m[1, 1] == 3

    # Ask C++ to change a value in the matrix, at (0,0)
    # and verify that m_linked as well as o.m are impacted
    #print(o.m); print()
    o.SetM(0, 0, 10)
    #print(o.m); print()
    #print(o.m[0, 0])
    o.SetM(2, 3, 15)
    assert m_linked[0, 0] == 10
    assert m_linked[2, 3] == 15
    assert o.m[0, 0] == 10
    assert o.m[2, 3] == 15

    # Make a clone of the C++ mat and change a value in it
    # => Make sure that the C++ mat is not impacted
    m_clone = np.copy(o.m)
    m_clone[1, 1] = 18
    assert o.m[1, 1] != 18

    # Change the whole C++ mat, by assigning to it a new matrix of different type and dimension
    # check that the shape has changed, and that values are ok
    new_shape = (3, 4, 2)
    new_type = np.float32
    new_mat = np.zeros(new_shape, new_type)
    new_mat[0, 0, 0] = 42.1
    new_mat[1, 0, 1] = 43.1
    new_mat[0, 1, 1] = 44.1
    o.m = new_mat
    print(o.m.shape)
    assert o.m.shape == new_shape
    assert o.m.dtype == new_type
    print(f"{new_mat=}")
    print(f"{o.m=}")
    assert are_float_close(o.m[0, 0, 0], 42.1)
    assert are_float_close(o.m[1, 0, 1], 43.1)
    assert are_float_close(o.m[0, 1, 1], 44.1)


def test_matx():
    # create object
    o: np.ndarray = immvision.cpp_immvision.StructTest_CvNpShared()
    assert o.mx.shape == (3, 2)

    # play with its internal cv::Mat

    # From python, change value in the C++ Mat (o.m) and assert that the changes are applied, and visible from python
    o.mx[0, 0] = 2
    assert o.mx[0, 0] == 2

    # Make a python linked copy of the C++ Mat, named m_linked.
    # Values of m_mlinked and the C++ mat should change together
    m_linked = o.mx
    m_linked[1, 1] = 3
    assert o.mx[1, 1] == 3

    # Ask C++ to change a value in the matrix, at (0,0)
    # and verify that m_linked as well as o.m are impacted
    o.SetMX(0, 0, 10)
    o.SetMX(2, 1, 15)
    assert are_float_close(m_linked[0, 0], 10)
    assert are_float_close(m_linked[2, 1], 15)
    assert are_float_close(o.mx[0, 0], 10)
    assert are_float_close(o.mx[2, 1], 15)

    # Make a clone of the C++ mat and change a value in it
    # => Make sure that the C++ mat is not impacted
    m_clone = np.copy(o.mx)
    m_clone[1, 1] = 18
    assert not are_float_close(o.mx[1, 1], 18)

    # Change the whole C++ matx, by assigning to it a new matrix with different values
    # check that values are ok
    new_shape = o.mx.shape
    new_type = o.mx.dtype
    new_mat = np.zeros(new_shape, new_type)
    new_mat[0, 0] = 42.1
    new_mat[1, 0] = 43.1
    new_mat[0, 1] = 44.1
    o.mx = new_mat
    assert o.mx.shape == new_shape
    assert o.mx.dtype == new_type
    assert are_float_close(o.mx[0, 0], 42.1)
    assert are_float_close(o.mx[1, 0], 43.1)
    assert are_float_close(o.mx[0, 1], 44.1)

    # Try to change the shape of the Matx (not allowed)
    new_mat = np.zeros([100, 100, 10], new_type)
    with pytest.raises(RuntimeError):
        o.mx = new_mat


if __name__ == "__main__":
    test_mat()
    test_matx()
