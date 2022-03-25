#include "gl_texture.h"

#include "immvision_gl_loader.h"

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
        std::cout << "GlTexture::Blit_Buffer() \n";

        // GL_UNPACK_ALIGNMENT needs to be changed if the amount of data per row is not a multiple of 4.
        // For example, if you have RGB data, with 3 bytes per pixel
        if ( (nb_channels != 4) && (image_width %4 != 0) )
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glBindTexture(GL_TEXTURE_2D, toGLuint(this->mImTextureId));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(HELLOIMGUI_USE_GLES2) || defined(HELLOIMGUI_USE_GLES3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

        int gl_color_flag = 0;
        if (nb_channels == 3 && flip_RedBlue)
            gl_color_flag = GL_BGR;
        else if (nb_channels == 4 && flip_RedBlue)
            gl_color_flag = GL_BGRA;
        else if (nb_channels == 3)
            gl_color_flag = GL_RGB;
        else if (nb_channels == 4)
            gl_color_flag = GL_RGBA;
        else
            assert(false);

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
    GlTextureCv::GlTextureCv(const cv::Mat& mat) : GlTextureCv()
    {
        BlitMat(mat);
    }

    cv::Mat _internal_to_rgba_image(const cv::Mat &inputMat)
    {
        cv::Mat mat = inputMat;
        if (!inputMat.isContinuous())
            mat = inputMat.clone();

        cv::Mat mat_rgba;
        int nbChannels = mat.channels();
        if (nbChannels == 1)
        {
            if (mat.type() == CV_8UC1)
                cv::cvtColor(mat, mat_rgba, cv::COLOR_GRAY2BGRA);
            else if ((mat.type() == CV_32FC1) || (mat.type() == CV_64FC1))
            {
                cv::Mat grey_uchar;
                cv::Mat float_times_255 = mat * 255.;
                float_times_255.convertTo(grey_uchar, CV_8UC1);
                cv::cvtColor(grey_uchar, mat_rgba, cv::COLOR_GRAY2BGRA);
            }
        }
        else if (nbChannels == 3)
        {
            if (mat.type() == CV_8UC3)
                cv::cvtColor(mat, mat_rgba, cv::COLOR_BGR2BGRA);
            else
                throw std::runtime_error("with 3 channels, _internal_to_rgba_image only supports CV_8UC3");
        }
        else if (nbChannels == 4)
        {
            if (mat.type() == CV_8UC4)
                mat_rgba = mat;
            else
                throw std::runtime_error("with 4 channels, _internal_to_rgba_image only supports CV_8UC4");
        }
        return mat_rgba;
    }

    void GlTextureCv::BlitMat(const cv::Mat& mat)
    {
        if (mat.empty())
            return;
        cv::Mat mat_rgba = _internal_to_rgba_image(mat);

        Blit_BGRA_Buffer(mat_rgba.data, mat_rgba.cols, mat_rgba.rows);
    }
} // namespace ImmVision