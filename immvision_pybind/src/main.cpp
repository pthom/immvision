#include "immvision/immvision.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "immvision/internal/opencv_pybind_converter.h"


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

namespace ImmVision_GlProvider
{
    void InitGlProvider();
    void ResetGlProvider();
}

void Image(
    const cv::Mat& m,
    bool refresh,
    const std::array<int, 2> size = {0, 0},
    bool isBgrOrBgra = true
)
{
    cv::Size cv_size(size[0], size[1]);
    ImmVision::Image(m, refresh, cv_size, isBgrOrBgra);
}

cv::Point2d ImageNavigator(const cv::Mat& image)
{
    cv::Size cv_size(600, 600);
    return ImmVision::ImageNavigator(image, cv_size);
}


// This function will call the 2 casters defined in opencv_pybind_converter
// The unit tests check that the values and types are unmodified
cv::Mat RoundTrip_Mat_To_BufferInfo_To_Mat(const cv::Mat& m)
{
    return m;
}

PYBIND11_MODULE(IMMVISION_PYBIND_BIN_MODULE_NAME, m) {
    m.doc() = R"pbdoc(
        immvision: immediate image debugger and insights
        -----------------------
        https://github.com/pthom/immvision/
    )pbdoc";

    m.def("add", [](int a, int b) { return a + b; },
          R"pbdoc(Add two numbers)pbdoc"
          );

    m.def("Image", Image);
    m.def("ImageNavigator", ImageNavigator);

    m.def("InitGlProvider", ImmVision_GlProvider::InitGlProvider);
    m.def("ResetGlProvider", ImmVision_GlProvider::ResetGlProvider);

    py::class_<cv::Point2d>(m, "cv.Point2d") // TODO : transform into array!
        .def(py::init<>())
        .def_readwrite("x", &cv::Point2d::x)
        .def_readwrite("y", &cv::Point2d::y);


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
