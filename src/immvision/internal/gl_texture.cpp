#include "gl_texture.h"

#include "immvision_gl_loader.h"

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
        GLuint textureId_Gl;
        glGenTextures(1, &textureId_Gl);
        this->imTextureId = toImTextureID(textureId_Gl);
    }

    GlTexture::~GlTexture()
    {
        GLuint textureId_Gl = toGLuint(this->imTextureId);
        glDeleteTextures(1, &textureId_Gl);
    }

    void GlTexture::Draw(const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->imageSize;
        ImGui::Image(this->imTextureId, size_, uv0, uv1, tint_col, border_col);
    }

    bool GlTexture::DrawButton(const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)    // <0 frame_padding uses default frame padding settings. 0 for no padding
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->imageSize;
        return ImGui::ImageButton(this->imTextureId, size_, uv0, uv1, frame_padding, bg_col, tint_col);
    }

    void GlTexture::Blit_Buffer(
        unsigned char *image_data,
        int image_width,
        int image_height,
        int nb_channels,
        bool flip_RedBlue
    )
    {
        glBindTexture(GL_TEXTURE_2D, toGLuint(this->imTextureId));
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

        imageSize = ImVec2((float)image_width, (float) image_height);
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

    void GlTextureCv::BlitMat(const cv::Mat& mat)
    {
        if (mat.empty())
            return;
        if (mat.type() == CV_8UC3)
            Blit_BGR_Buffer(mat.data, mat.cols, mat.rows);
        else if (mat.type() == CV_8UC4)
            Blit_BGRA_Buffer(mat.data, mat.cols, mat.rows);
        else
            assert(false); // bad image format!
    }
} // namespace ImmVision