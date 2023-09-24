#ifndef IMMVISION_BUILDING_PYBIND

#include "immvision/internal/gl/gl_provider.h"
#include "immvision_gl_loader/immvision_gl_loader.h"
#include "immvision/image.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/drawing/internal_icons.h"
#include <iostream>

#if defined(IMMVISION_USE_GLAD)
#include <glad/glad.h>
#endif

namespace ImmVision_GlProvider
{
    void _AssertOpenGlLoaderWorking()
    {
        // These OpenGL functions pointers should be filled once the OpenGL loader was inited
        size_t glGenTexturesAddress = (size_t)glGenTextures;
        size_t glDeleteTexturesAddress = (size_t)glDeleteTextures;

        // If they are empty, and if we are using glad, the user probably forgot to call gladLoadGL().
        // Let's help him (this is especially useful for python bindings where no bindings for glad are provided)
        if ((glGenTexturesAddress == 0) || (glDeleteTexturesAddress == 0))
        {
#if defined(IMMVISION_USE_GLAD)
            gladLoadGL();
            glGenTexturesAddress = (size_t)glGenTextures;
            glDeleteTexturesAddress = (size_t)glDeleteTextures;
#endif
        }

        if ((glGenTexturesAddress == 0) || (glDeleteTexturesAddress == 0))
        {
            const char* err_msg = "glGenTextures/glDeleteTexturesAddress address not initialized. Is your your OpenGL Loader initialized?";
            std::cerr << err_msg;
            assert(false);
            throw std::runtime_error(err_msg);
        }
    }

    void InitGlProvider()
    {
        // InitGlProvider must be called after the OpenGl Loader is initialized
        _AssertOpenGlLoaderWorking();
    }

    void ResetGlProvider()
    {
        // InitGlProvider must be called before the OpenGl Loader is reset
        _AssertOpenGlLoaderWorking();
        ImmVision::ClearTextureCache();
        ImmVision::Colormap::ClearColormapsTexturesCache();
        ImmVision::Icons::ClearIconsTextureCache();
    }

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, ImTextureID textureId)
    {
        //static int counter = 0;
        //++counter;
        //std::cout << "Blit_RGBA_Buffer counter=" << counter << "\n";
        GLuint textureIdAsUint = (GLuint)(size_t)textureId;
        glBindTexture(GL_TEXTURE_2D, textureIdAsUint);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(__EMSCRIPTEN__) || defined(IMMVISION_USE_GLES2) || defined(IMMVISION_USE_GLES3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        GLenum gl_color_flag_input = GL_RGBA;
        GLenum gl_color_flag_output = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, gl_color_flag_input,
                     image_width,
                     image_height, 0, gl_color_flag_output, GL_UNSIGNED_BYTE, image_data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ImTextureID GenTexture()
    {
        //std::cout << "GenTexture()\n";
        _AssertOpenGlLoaderWorking();
        GLuint textureId_Gl;
        glGenTextures(1, &textureId_Gl);
        return (ImTextureID)(size_t)textureId_Gl;
    }

    void DeleteTexture(ImTextureID texture_id)
    {
        //std::cout << "DeleteTexture()\n";
        _AssertOpenGlLoaderWorking();
        GLuint textureIdAsUint = (GLuint)(size_t)texture_id;
        glDeleteTextures(1, &textureIdAsUint);
    }
}

#endif // #ifndef IMMVISION_BUILDING_PYBIND
