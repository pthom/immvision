#pragma once
#include "imgui.h"

namespace ImmVision_GlProvider
{
    // InitGlProvider must be called after the OpenGl Loader is initialized
    void InitGlProvider();
    // InitGlProvider must be called before the OpenGl Loader is reset
    void ResetGlProvider();

    // Upload RGBA pixel data to a texture and generate mipmaps.
    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, ImTextureID textureId);
    ImTextureID GenTexture();
    void DeleteTexture(ImTextureID texture_id);

    // GL texture filtering modes (values match OpenGL constants)
    enum class TextureFilter
    {
        Nearest,           // GL_NEAREST — pixel-perfect at high zoom
        Linear,            // GL_LINEAR — smooth interpolation
        LinearMipmapLinear // GL_LINEAR_MIPMAP_LINEAR — trilinear, for downsampling
    };

    // Set the min/mag filtering mode for a texture.
    void SetTextureFiltering(ImTextureID textureId, TextureFilter minFilter, TextureFilter magFilter);
}
