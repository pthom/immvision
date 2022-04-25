#include "immvision/image_navigator.h"
#include "cv_np/cv_np_shared_cast.h"
#include "transfer_imgui_context.h"

#include <pybind11/pybind11.h>
#include <opencv2/core.hpp>


namespace py = pybind11;


void ImageNavigator(const cv::Mat& image)
{
    transfer_imgui_context();
    cv::Size cv_size(500, 500);
    ImmVision::ImageNavigator(image, cv_size);
}



void pydef_image_navigator(pybind11::module& m)
{
    m.def("ImageNavigator", ImageNavigator);

    using namespace ImmVision;
    py::class_<ImageNavigatorParams>(m, "ImageNavigatorParams")
        .def(py::init<>())
        .def_readwrite("ImageDisplaySize", &ImageNavigatorParams::ImageDisplaySize)
        .def_readwrite("Legend", &ImageNavigatorParams::Legend)
        .def_readwrite("ZoomMatrix", &ImageNavigatorParams::ZoomPanMatrix)
        .def_readwrite("WatchedPixels", &ImageNavigatorParams::WatchedPixels)
        ;

}