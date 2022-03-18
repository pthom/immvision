#include "immvision/image_gl.h"
#include "immvision_include_opengl.h"

namespace immvision
{
ImageGl::ImageGl()
: imageSize(0, 0)
, imTextureId(nullptr)
{
}

ImageGl::~ImageGl()
{
    GLuint textureId_Gl = (GLuint)(intptr_t)imTextureId;
    glDeleteTextures(1, &textureId_Gl);
}

void ImageGl::Draw(
    const ImVec2& size,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& tint_col,
    const ImVec4& border_col)
{
    ImVec2 size_(size);
    if (size.x == 0.f)
        size_ = this->imageSize;
    ImGui::Image(this->imTextureId, size_, uv0, uv1, tint_col, border_col);
}

bool ImageGl::DrawButton(
    const ImVec2& size,
    const ImVec2& uv0,
    const ImVec2& uv1,
    int frame_padding,
    const ImVec4& bg_col,
    const ImVec4& tint_col)
{
    ImVec2 size_(size);
    if (size.x == 0.f)
        size_ = this->imageSize;
    return ImGui::ImageButton(this->imTextureId, size_, uv0, uv1, frame_padding, bg_col, tint_col);
}

ImageGlPtr ImageGl::FactorImage_From_Buffer(
        unsigned char *image_data, 
        int image_width, 
        int image_height,
        int nb_channels,
        bool flip_RedBlue
        )
{
    ImageGlPtr r (new ImageGl());

    GLuint textureId_Gl;
    glGenTextures(1, &textureId_Gl);
    glBindTexture(GL_TEXTURE_2D, textureId_Gl);
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

    r->imageSize = ImVec2((float)image_width, (float) image_height);
    r->imTextureId = (void*)(intptr_t)textureId_Gl;
    return r;
}


ImageGlPtr ImageGl::FactorImage_From_RGBA_Buffer(
    unsigned char *image_data, int image_width, int image_height)
{
    return FactorImage_From_Buffer(image_data, image_width, image_height, 4, false);
}

ImageGlPtr ImageGl::FactorImage_From_RGB_Buffer(
    unsigned char *image_data, int image_width, int image_height)
{
    return FactorImage_From_Buffer(image_data, image_width, image_height, 3, false);
}

ImageGlPtr ImageGl::FactorImage_From_BGRA_Buffer(
    unsigned char *image_data, int image_width, int image_height)
{
    return FactorImage_From_Buffer(image_data, image_width, image_height, 4, true);
}

ImageGlPtr ImageGl::FactorImage_From_BGR_Buffer(
    unsigned char *image_data, int image_width, int image_height)
{
    return FactorImage_From_Buffer(image_data, image_width, image_height, 3, true);
}

} // namespace immvision
