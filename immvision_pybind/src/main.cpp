#include "immvision/immvision.h"

#include <pybind11/pybind11.h>
#include "immvision/internal/opencv_pybind_converter.h"


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;


//template<typename T>
//void Image(
//    const pybind11::array_t<T>& image,
//    bool refresh,
//    const std::array<int, 2> size = {0, 0},
//    bool isBgrOrBgra = true
//)
//{
//    cv::Mat m = numpy_opencv_convert::np_array_to_cv_mat(image);
//    cv::Size cv_size(size[0], size[1]);
//    ImmVision::Image(m, refresh, cv_size, isBgrOrBgra);
//}
//
//
//template<typename T>
//std::string np_array_info(const pybind11::array_t<T>& array)
//{
//    return numpy_opencv_convert::np_array_info(array);
//}
//
//
//template<typename T>
//void Truc(const pybind11::array_t<T>& image_rgba)
//{
//    // Use Python to make our directories
//    py::object python_module = py::module_::import("gl_provider_python");
//    py::object Blit_RGBA_Buffer = python_module.attr("Blit_RGBA_Buffer");
//    py::object GenTexture = python_module.attr("GenTexture");
//    py::object DeleteTexture = python_module.attr("DeleteTexture");
//
//    std::cout << "C++ About to call GenTexture\n";
//    py::object id_object = GenTexture();
//    auto texture_id = id_object.cast<unsigned int>();
//    std::cout << "C++ After calling GenTexture, texture_id=%i" << texture_id << "\n";
//
//
//    std::cout << "C++ About to call Blit_RGBA_Buffer with image\n";
//    std::cout << np_array_info(image_rgba);
//    Blit_RGBA_Buffer(image_rgba, texture_id);
//    std::cout << "C++ After calling Blit_RGBA_Buffer\n";
//
//    std::cout << "C++ About to call DeleteTexture\n";
//    py::object q = DeleteTexture(texture_id);
//    std::cout << "C++ After calling DeleteTexture\n";
//}
//
//
//
//void Truc2(const cv::Mat& image_rgba)
//{
//    // Use Python to make our directories
//    py::object python_module = py::module_::import("gl_provider_python");
//    py::object Blit_RGBA_Buffer = python_module.attr("Blit_RGBA_Buffer");
//    py::object GenTexture = python_module.attr("GenTexture");
//    py::object DeleteTexture = python_module.attr("DeleteTexture");
//
//    std::cout << "Entering Truc2 with image of size " << image_rgba.cols << "*" << image_rgba.rows
//              << " and type=" << image_rgba.type() << " (CV_8UC4=" << CV_8UC4 <<")\n";
//
//    std::cout << "C++ About to call GenTexture\n";
//    py::object id_object = GenTexture();
//    auto texture_id = id_object.cast<unsigned int>();
//    std::cout << "C++ After calling GenTexture, texture_id=%i" << texture_id << "\n";
//
//
//    std::cout << "C++ About to call Blit_RGBA_Buffer with image\n";
//    Blit_RGBA_Buffer(image_rgba, texture_id);
//    std::cout << "C++ After calling Blit_RGBA_Buffer\n";
//
//    std::cout << "C++ About to call DeleteTexture\n";
//    py::object q = DeleteTexture(texture_id);
//    std::cout << "C++ After calling DeleteTexture\n";
//}


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

    //m.def("InitGladGLLoader", InitGladGLLoader);
//    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "np_array_info", np_array_info);
//    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "Image", Image);
//    //m.def("Truc", Truc);
//    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "Truc", Truc);
//    m.def("Truc2", Truc2);
    m.def("RoundTrip_Mat_To_BufferInfo_To_Mat", RoundTrip_Mat_To_BufferInfo_To_Mat);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
