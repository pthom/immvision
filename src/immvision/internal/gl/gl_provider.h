#pragma once
#include "imgui.h"

namespace ImmVision_GlProvider
{
    // InitGlProvider must be called after the OpenGl Loader is initialized
    void InitGlProvider();
    // InitGlProvider must be called before the OpenGl Loader is reset
    void ResetGlProvider();

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, ImTextureID textureId);
    ImTextureID GenTexture();
    void DeleteTexture(ImTextureID texture_id);
}
