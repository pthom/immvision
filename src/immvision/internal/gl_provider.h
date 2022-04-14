#pragma once

namespace ImmVision_GlProvider
{
    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, unsigned int textureId);
    unsigned int GenTexture();
    void DeleteTexture(unsigned int texture_id);
}

