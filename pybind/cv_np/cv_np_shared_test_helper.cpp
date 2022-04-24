#include "cv_np_shared_cast.h"

#include <pybind11/pybind11.h>
#include <opencv2/core.hpp>


// This simple function will call
// * the cast Python->C++ (for the input parameter)
// * the cast C++->Python (for the returned value)
// The unit tests check that the values and types are unmodified
cv::Mat CvNp_TestRoundTrip(const cv::Mat& m)
{
    return m;
}


struct CvNpSharedCast_TestHelper
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


void pydef_cv_np_shared_test(pybind11::module& m)
{
    pybind11::class_<CvNpSharedCast_TestHelper>(m, "CvNpSharedCast_TestHelper")
        .def(pybind11::init<>())

        .def_readwrite("m", &CvNpSharedCast_TestHelper::m)
        .def("SetM", &CvNpSharedCast_TestHelper::SetM)

        .def_readwrite("mx", &CvNpSharedCast_TestHelper::mx)
        .def("SetMX", &CvNpSharedCast_TestHelper::SetMX)

        .def_readwrite("s", &CvNpSharedCast_TestHelper::s)
        .def("SetWidth", &CvNpSharedCast_TestHelper::SetWidth)
        .def("SetHeight", &CvNpSharedCast_TestHelper::SetHeight)

        .def_readwrite("pt", &CvNpSharedCast_TestHelper::pt)
        .def("SetX", &CvNpSharedCast_TestHelper::SetX)
        .def("SetY", &CvNpSharedCast_TestHelper::SetY)

        .def_readwrite("pt3", &CvNpSharedCast_TestHelper::pt3)
        .def("SetX3", &CvNpSharedCast_TestHelper::SetX3)
        .def("SetY3", &CvNpSharedCast_TestHelper::SetY3)
        .def("SetZ3", &CvNpSharedCast_TestHelper::SetZ3)
        ;

    m.def("CvNp_TestRoundTrip", CvNp_TestRoundTrip);
}