#include "immvision/immvision.h"
#include "numpy_opencv_convert.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "glad/glad.h"
#include "immvision/internal/opencv_pybind_converter.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int add(int i, int j) {
    return i + j;
}

namespace py = pybind11;


//#define EXIT_WITH_MESSAGE(msg) \
//{ \
//    std::cerr << "throw runtime_error: " << msg << "\t\t at " << __FILE__ << ":" << __LINE__ << "\n"; \
//    throw std::runtime_error(msg); \
//}
//void InitGladGLLoader_auto()
//{
//    static bool was_inited = false;
//    if (!was_inited)
//    {
//        bool err = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0;
//        if (err)
//            EXIT_WITH_MESSAGE("Error in InitGladGLLoader");
//        was_inited = true;
//    }
//}
//
//  call this from python like this:
//
//  immvision.InitGladGLLoader(sdl2.SDL_GL_GetProcAddress)
//void InitGladGLLoader(size_t sdl_gl_proc_address_sizet)
//{
//    static bool was_inited = false;
//    if (!was_inited)
//    {
//        void* sdl_gl_proc_address = (void *)sdl_gl_proc_address_sizet;
//        printf("Trying to load from address: size_t=%zu pointer=%p \n", sdl_gl_proc_address_sizet, sdl_gl_proc_address);
//        bool err = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sdl_gl_proc_address)) == 0;
//        printf("Done loading\n");
//        if (err)
//            EXIT_WITH_MESSAGE("Error in InitGladGLLoader");
//        was_inited = true;
//    }
//}

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


template<typename T>
void Truc(const pybind11::array_t<T>& image_rgba)
{
    // Use Python to make our directories
    py::object python_module = py::module_::import("gl_provider_python");
    py::object Blit_RGBA_Buffer = python_module.attr("Blit_RGBA_Buffer");
    py::object GenTexture = python_module.attr("GenTexture");
    py::object DeleteTexture = python_module.attr("DeleteTexture");

    std::cout << "C++ About to call GenTexture\n";
    py::object id_object = GenTexture();
    auto texture_id = id_object.cast<unsigned int>();
    std::cout << "C++ After calling GenTexture, texture_id=%i" << texture_id << "\n";


    std::cout << "C++ About to call Blit_RGBA_Buffer with image\n";
    std::cout << np_array_info(image_rgba);
    Blit_RGBA_Buffer(image_rgba, texture_id);
    std::cout << "C++ After calling Blit_RGBA_Buffer\n";

    std::cout << "C++ About to call DeleteTexture\n";
    py::object q = DeleteTexture(texture_id);
    std::cout << "C++ After calling DeleteTexture\n";
}

void Muche(const cv::Mat& m)
{
    std::cout << "Muche size " << m.cols << " " << m.rows << "\n";
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
    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "np_array_info", np_array_info);
    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "Image", Image);
    //m.def("Truc", Truc);
    MODULE_DEF_FUNCTION_ALL_DEPTHS(m, "Truc", Truc);
    m.def("Muche", Muche);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
