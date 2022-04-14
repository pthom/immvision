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
    py::object os = py::module_::import("image_rgba_to_texture");
    py::object image_rgba_to_texture = os.attr("image_rgba_to_texture");
    py::object delete_texture = os.attr("delete_texture");

    std::cout << "C++ About to call image_rgba_to_texture with image\n";
    std::cout << np_array_info(image_rgba);
    py::object id_object = image_rgba_to_texture(image_rgba);
    auto id_uint = id_object.cast<unsigned int>();
    std::cout << "C++ After calling image_rgba_to_texture, id_uint=%i" << id_uint << "\n";

    std::cout << "C++ About to call delete_texture\n";
    py::object q = delete_texture(id_uint);
    std::cout << "C++ After calling delete_texture\n";
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

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
