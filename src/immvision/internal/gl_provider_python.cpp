#ifdef IMMVISION_PYBIND

#include "immvision/internal/gl_provider.h"
#include "immvision/internal/opencv_pybind_converter.h"

#include <opencv2/core.hpp>
#include <pybind11/pybind11.h>
#include <iostream>


namespace ImmVision_GlProvider
{
    struct PythonFunctions
    {
        PythonFunctions()
        {
            PythonModule = pybind11::module_::import("gl_provider_python");
            Blit_RGBA_Buffer = PythonModule.attr("Blit_RGBA_Buffer");
            GenTexture = PythonModule.attr("GenTexture");
            DeleteTexture = PythonModule.attr("DeleteTexture");
        }

        pybind11::object PythonModule;
        pybind11::object Blit_RGBA_Buffer;
        pybind11::object GenTexture;
        pybind11::object DeleteTexture;

        static PythonFunctions Instance()
        {
            static PythonFunctions instance;
            return instance;
        }
    };


    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, unsigned int textureId)
    {
//        std::cout << "C++ : Blit_RGBA_Buffer -> about to import gl_provider_python\n";
//        pybind11::object python_module = pybind11::module_::import("gl_provider_python");
//        (void)python_module;

        //PythonFunctions gPythonFunctions;

        std::cout << "C++ : Blit_RGBA_Buffer about to create Mat\n";
        cv::Mat m(image_height, image_width, CV_8UC4, image_data);
        std::cout << "C++ : Blit_RGBA_Buffer about to call PythonFunctions::Blit_RGBA_Buffer\n";
        PythonFunctions::Instance().Blit_RGBA_Buffer(m, textureId);
        std::cout << "C++ : Blit_RGBA_Buffer done!\n";
    }

    unsigned int GenTexture()
    {
        //PythonFunctions gPythonFunctions;

        std::cout << "C++ About to call GenTexture\n";
        pybind11::object id_object = PythonFunctions::Instance().GenTexture();
        auto texture_id = id_object.cast<unsigned int>();
        std::cout << "C++ After calling GenTexture, texture_id=%i" << texture_id << "\n";
        return  texture_id;
    }

    void DeleteTexture(unsigned int texture_id)
    {
        //PythonFunctions gPythonFunctions;

        std::cout << "C++ About to call DeleteTexture\n";
        PythonFunctions::Instance().DeleteTexture(texture_id);
        std::cout << "C++ After calling DeleteTexture\n";
    }
}

#endif // #ifdef IMMVISION_PYBIND