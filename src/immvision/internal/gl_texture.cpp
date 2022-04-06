#include "gl_texture.h"

#include "immvision_gl_loader.h"
#include "immvision/internal/cv_drawing_utils.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#include <opencv2/imgproc.hpp>
#include <sstream>
#include <iostream>

namespace
{
    ImTextureID toImTextureID(GLuint v)
    {
        return (ImTextureID)(intptr_t)v;
    }
    GLuint toGLuint(ImTextureID v)
    {
        return (GLuint)(intptr_t)v;
    }
}

namespace ImmVision
{
    GlTexture::GlTexture()
    {
        std::cout << "GlTexture::GlTexture() \n";
        GLuint textureId_Gl;
        glGenTextures(1, &textureId_Gl);
        this->mImTextureId = toImTextureID(textureId_Gl);
    }

    GlTexture::~GlTexture()
    {
        std::cout << "GlTexture::~GlTexture() \n";
        GLuint textureId_Gl = toGLuint(this->mImTextureId);
        glDeleteTextures(1, &textureId_Gl);
    }

    void GlTexture::Draw(const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) const
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->mImageSize;
        ImGui::Image(this->mImTextureId, size_, uv0, uv1, tint_col, border_col);
    }

    bool GlTexture::DrawButton(const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) const
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->mImageSize;
        return ImGui::ImageButton(this->mImTextureId, size_, uv0, uv1, frame_padding, bg_col, tint_col);
    }

    void GlTexture::Draw_DisableDragWindow(const ImVec2 &size) const
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->mImageSize;

        ImVec2 imageTl = ImGui::GetCursorScreenPos();
        ImVec2 imageBr(imageTl.x + size.x, imageTl.y + size.y);
        std::stringstream id;
        id << "##" << (GLuint)(intptr_t)mImTextureId;
        ImGui::InvisibleButton(id.str().c_str(), size);
        ImGui::GetWindowDrawList()->AddImage(mImTextureId, imageTl, imageBr);
    }

    void GlTexture::Blit_Buffer(
        unsigned char *image_data,
        int image_width,
        int image_height,
        int nb_channels,
        bool flip_RedBlue
    )
    {
#ifdef __EMSCRIPTEN__
        if (flip_RedBlue)
        {
            std::cerr << "GlTexture::Blit_Buffer does not support flip_RedBlue under emscripten!\n";
            assert(false);
        }
#endif

        // GL_UNPACK_ALIGNMENT needs to be changed if the amount of data per row is not a multiple of 4.
        // For example, if you have RGB data, with 3 bytes per pixel
        if ( (nb_channels != 4) && (image_width %4 != 0) )
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glBindTexture(GL_TEXTURE_2D, toGLuint(this->mImTextureId));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(__EMSCRIPTEN__) || defined(IMMVISION_USE_GLES2) || defined(IMMVISION_USE_GLES3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

        GLenum gl_color_flag = 0;
        if (false)
            ;
#ifndef __EMSCRIPTEN__
        else if (nb_channels == 3 && flip_RedBlue)
            gl_color_flag = GL_BGR;
        else if (nb_channels == 4 && flip_RedBlue)
            gl_color_flag = GL_BGRA;
#endif
        else if (nb_channels == 3)
            gl_color_flag = GL_RGB;
        else if (nb_channels == 4)
            gl_color_flag = GL_RGBA;
        else
        {
            std::cout << "GlTexture::Blit_Buffer() bad color\n";
            throw "GlTexture::Blit_Buffer() bad color";
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     image_width,
                     image_height, 0, gl_color_flag, GL_UNSIGNED_BYTE, image_data);
        glBindTexture(GL_TEXTURE_2D, 0);

        mImageSize = ImVec2((float)image_width, (float) image_height);
    }

    void GlTexture::Blit_RGB_Buffer(unsigned char *image_data, int image_width, int image_height)
    {
        Blit_Buffer(image_data, image_width, image_height, 3, false);
    }
    void GlTexture::Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height)
    {
        Blit_Buffer(image_data, image_width, image_height, 4, false);
    }
    void GlTexture::Blit_BGR_Buffer(unsigned char *image_data, int image_width, int image_height)
    {
        Blit_Buffer(image_data, image_width, image_height, 3, true);
    }
    void GlTexture::Blit_BGRA_Buffer(unsigned char *image_data, int image_width, int image_height)
    {
        Blit_Buffer(image_data, image_width, image_height, 4, true);
    }

    //
    // ImageTextureCv
    //
    GlTextureCv::GlTextureCv(const cv::Mat& mat, bool isBgrOrBgra) : GlTextureCv()
    {
        BlitMat(mat, isBgrOrBgra);
    }

    void GlTextureCv::BlitMat(const cv::Mat& mat, bool isBgrOrBgra)
    {
        if (mat.empty())
            return;
        cv::Mat mat_rgba = CvDrawingUtils::converted_to_rgba_image(mat, isBgrOrBgra);

        Blit_RGBA_Buffer(mat_rgba.data, mat_rgba.cols, mat_rgba.rows);
    }
} // namespace ImmVision