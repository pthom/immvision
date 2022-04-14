#ifdef IMMVISION_PYBIND

#include "immvision/internal/gl_provider.h"
#include "immvision/internal/opencv_pybind_converter.h"

#include <opencv2/core.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <iostream>


namespace ImmVision_GlProvider
{
    namespace PythonFunctions
    {
        pybind11::object python_module = pybind11::module_::import("gl_provider_python");
        pybind11::object Blit_RGBA_Buffer = python_module.attr("Blit_RGBA_Buffer");
        pybind11::object GenTexture = python_module.attr("GenTexture");
        pybind11::object DeleteTexture = python_module.attr("DeleteTexture");
    }

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, unsigned int textureId)
    {
//        std::cout << "C++ : Blit_RGBA_Buffer -> about to import gl_provider_python\n";
//        pybind11::object python_module = pybind11::module_::import("gl_provider_python");
//        (void)python_module;

        std::cout << "C++ : Blit_RGBA_Buffer about to create Mat\n";
        cv::Mat m(image_height, image_width, CV_8UC4, image_data);
        std::cout << "C++ : Blit_RGBA_Buffer about to call PythonFunctions::Blit_RGBA_Buffer\n";
        PythonFunctions::Blit_RGBA_Buffer(m, textureId);
        std::cout << "C++ : Blit_RGBA_Buffer done!\n";
    }

    unsigned int GenTexture()
    {
        std::cout << "C++ About to call GenTexture\n";
        pybind11::object id_object = PythonFunctions::GenTexture();
        auto texture_id = id_object.cast<unsigned int>();
        std::cout << "C++ After calling GenTexture, texture_id=%i" << texture_id << "\n";
        return  texture_id;
    }

    void DeleteTexture(unsigned int texture_id)
    {
        std::cout << "C++ About to call DeleteTexture\n";
        PythonFunctions::DeleteTexture(texture_id);
        std::cout << "C++ After calling DeleteTexture\n";
    }
}

#endif // #ifdef IMMVISION_PYBIND