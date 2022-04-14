#include "immvision/immvision.h"
#include "numpy_opencv_convert.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "glad/glad.h"
#include <SDL.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int add(int i, int j) {
    return i + j;
}

namespace py = pybind11;


template<typename T>
void Image(
    const pybind11::array_t<T>& image,
    bool refresh,
    const std::array<int, 2> size = {0, 0},
    bool isBgrOrBgra = true
)
{
    cv::Mat m = numpy_opencv_convert::np_array_to_cv_mat(image);
    cv::Size cv_size(size[0], size[1]);
    ImmVision::Image(m, refresh, cv_size, isBgrOrBgra);
}



template<typename T>
std::string np_array_info(const pybind11::array_t<T>& array)
{
    return numpy_opencv_convert::np_array_info(array);
}





PYBIND11_MODULE(IMMVISION_PYBIND_BIN_MODULE_NAME, m) {
    m.doc() = R"pbdoc(
        immvision: immediate image debugger and insights
        -----------------------
        https://github.com/pthom/immvision/

        .. currentmodule:: immvision

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

//    m.def("add", &add, R"pbdoc(
//        Add two numbers
//
//        Some other explanation about the add function.
//    )pbdoc");

    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "np_array_info", np_array_info);
    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "Image", Image);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
