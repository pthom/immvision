#ifndef IMMVISION_PYBIND

#include "immvision/internal/gl_provider.h"
#include "immvision_gl_loader/immvision_gl_loader.h"
#include <iostream>

//#ifdef __APPLE__
//#include "TargetConditionals.h"
//#endif

namespace ImmVision_GlProvider
{
    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, unsigned int textureId)
    {
        // std::cout << "Blit_RGBA_Buffer()\n";
        glBindTexture(GL_TEXTURE_2D, textureId);
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

    unsigned int GenTexture()
    {
        std::cout << "GenTexture()\n";
        size_t glGenTexturesAddress = (size_t)glGenTextures;
        if (glGenTexturesAddress == 0)
        {
            const char* err_msg = "glGenTextures address not initialized. Did you initialize your OpenGL Loader?";
            std::cerr << err_msg;
            throw std::runtime_error(err_msg);
        }

        GLuint textureId_Gl;
        glGenTextures(1, &textureId_Gl);
        return textureId_Gl;
    }

    void DeleteTexture(unsigned int texture_id)
    {
        std::cout << "DeleteTexture()\n";
        glDeleteTextures(1, &texture_id);
    }
}

#endif // #ifndef IMMVISION_PYBIND
