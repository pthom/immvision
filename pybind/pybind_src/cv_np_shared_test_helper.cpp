#include "cv_np_shared_cast.h"

#include <pybind11/pybind11.h>
#include <opencv2/core.hpp>


// This function will call the 2 casters defined in opencv_pybind_converter
// The unit tests check that the values and types are unmodified
cv::Mat RoundTrip_Mat_To_BufferInfo_To_Mat(const cv::Mat& m)
{
    return m;
}


struct StructTest_CvNpShared
{
    // Create a mat with 3 rows, 4 columns and 1 channel
    // its shape for numpy should be (3, 4)
    cv::Mat m = cv::Mat::eye(cv::Size(4, 3), CV_8UC1);
    void SetM(int row, int col, uchar v) { m.at<uchar>(row, col) = v; }

    cv::Matx32d mx = cv::Matx32d::eye();
    void SetMX(int row, int col, double v) { mx(row, col) = v;}
};


void register_cv_np_shared_test_functions(pybind11::module& m)
{
    pybind11::class_<StructTest_CvNpShared>(m, "StructTest_CvNpShared")
        .def(pybind11::init<>())
        .def_readwrite("m", &StructTest_CvNpShared::m)
        .def("SetM", &StructTest_CvNpShared::SetM)
        .def_readwrite("mx", &StructTest_CvNpShared::mx)
        .def("SetMX", &StructTest_CvNpShared::SetMX)
        ;
}