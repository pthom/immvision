#ifdef IMMVISION_BUILDING_PYBIND

#include "immvision/internal/gl_provider.h"
#include "immvision/image.h"
#include "immvision/internal/opencv_pybind_converter.h"

#include <opencv2/core.hpp>
#include <pybind11/pybind11.h>
#include <iostream>
#include <memory>


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
            IncCounter = PythonModule.attr("IncCounter");
        }
        pybind11::object PythonModule;
        pybind11::object Blit_RGBA_Buffer;
        pybind11::object GenTexture;
        pybind11::object DeleteTexture;
        pybind11::object IncCounter;
    };

    std::unique_ptr<PythonFunctions> gPythonFunctions; // = std::make_unique<PythonFunctions>();

    void InitGlProvider()
    {
        // InitGlProvider must be called after the OpenGl Loader is initialized
        if (!gPythonFunctions)
            gPythonFunctions = std::make_unique<PythonFunctions>();
    }

    void ResetGlProvider()
    {
        // InitGlProvider must be called before the OpenGl Loader is reset
        ImmVision::ClearAllTextureCaches();
        gPythonFunctions.release();
    }


    bool gEnableOpenGl = true;

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, unsigned int textureId)
    {
//        std::cout << "C++ : Blit_RGBA_Buffer -> about to import gl_provider_python\n";
//        pybind11::object python_module = pybind11::module_::import("gl_provider_python");
//        (void)python_module;

        assert(gPythonFunctions);

        if (gEnableOpenGl)
        {
            std::cout << "C++ : Blit_RGBA_Buffer about to create Mat\n";
            cv::Mat m(image_height, image_width, CV_8UC4, image_data);
            std::cout << "C++ : Blit_RGBA_Buffer about to call PythonFunctions::Blit_RGBA_Buffer\n";
            gPythonFunctions->Blit_RGBA_Buffer(m, textureId);
            std::cout << "C++ : Blit_RGBA_Buffer done!\n";
        }
    }

    unsigned int GenTexture()
    {
        assert(gPythonFunctions);

        if (gEnableOpenGl)
        {
            std::cout << "C++ About to call GenTexture\n";
            pybind11::object id_object = gPythonFunctions->GenTexture();
            auto texture_id = id_object.cast<unsigned int>();
            std::cout << "C++ After calling GenTexture, texture_id=%i" << texture_id << "\n";
            return  texture_id;
        }
        else
            return 0;
    }

    void DeleteTexture(unsigned int texture_id)
    {
        assert(gPythonFunctions);
        if (gEnableOpenGl)
        {
            std::cout << "C++ About to call DeleteTexture\n";
            gPythonFunctions->DeleteTexture(texture_id);
            std::cout << "C++ After calling DeleteTexture\n";
        }
    }

    void IncCounter()
    {
        gPythonFunctions->IncCounter();
    }
}

#endif // #ifdef IMMVISION_BUILDING_PYBIND