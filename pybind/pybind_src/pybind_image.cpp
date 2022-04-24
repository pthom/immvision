#include "immvision/image.h"
#include "transfer_imgui_context.h"
#include "cv_np/cv_np_shared_cast.h"

#include <pybind11/pybind11.h>
#include <opencv2/core.hpp>

namespace py = pybind11;


void Image(
    const cv::Mat& m,
    bool refresh,
    const cv::Size size = {0, 0},
    bool isBgrOrBgra = true
)
{
    transfer_imgui_context();
    cv::Size cv_size(size.width, size.height);
    ImmVision::Image(m, refresh, cv_size, isBgrOrBgra);
}



void pydef_image(pybind11::module& m)
{
    m.def("Image", Image, "Image doc",
          py::arg("image"),
          py::arg("refresh") = false,
          py::arg("size") = pybind11::make_tuple(0, 0),
          py::arg("isBgrOrBgra") = true
    );

}