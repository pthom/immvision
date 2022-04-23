#include "cv_np_shared_cast.h"

#include <pybind11/pybind11.h>
#include <opencv2/core.hpp>


// This simple function will call
// * the cast Python->C++ (for the input parameter)
// * the cast C++->Python (for the returned value)
// The unit tests check that the values and types are unmodified
cv::Mat Test_CvNpRoundTrip(const cv::Mat& m)
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

    cv::Size s = cv::Size(123, 456);
    void SetWidth(int w) { s.width = w;}
    void SetHeight(int h) { s.height = h;}

    cv::Point2i pt = cv::Point2i(42, 43);
    void SetX(int x) { pt.x = x; }
    void SetY(int y) { pt.y = y; }

    cv::Point3d pt3 = cv::Point3d(41.5, 42., 42.5);
    void SetX3(double x) { pt3.x = x; }
    void SetY3(double y) { pt3.y = y; }
    void SetZ3(double z) { pt3.z = z; }
};


void register_cv_np_shared_test_functions(pybind11::module& m)
{
    pybind11::class_<StructTest_CvNpShared>(m, "StructTest_CvNpShared")
        .def(pybind11::init<>())

        .def_readwrite("m", &StructTest_CvNpShared::m)
        .def("SetM", &StructTest_CvNpShared::SetM)

        .def_readwrite("mx", &StructTest_CvNpShared::mx)
        .def("SetMX", &StructTest_CvNpShared::SetMX)

        .def_readwrite("s", &StructTest_CvNpShared::s)
        .def("SetWidth", &StructTest_CvNpShared::SetWidth)
        .def("SetHeight", &StructTest_CvNpShared::SetHeight)

        .def_readwrite("pt", &StructTest_CvNpShared::pt)
        .def("SetX", &StructTest_CvNpShared::SetX)
        .def("SetY", &StructTest_CvNpShared::SetY)

        .def_readwrite("pt3", &StructTest_CvNpShared::pt3)
        .def("SetX3", &StructTest_CvNpShared::SetX3)
        .def("SetY3", &StructTest_CvNpShared::SetY3)
        .def("SetZ3", &StructTest_CvNpShared::SetZ3)
        ;

    m.def("Test_CvNpRoundTrip", Test_CvNpRoundTrip);
}