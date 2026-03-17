#include "immvision/gl_texture.h"
#include "immvision/internal/cv/convert_to_rgba.h"
#include "immvision/internal/gl/gl_provider.h"


namespace ImmVision
{
    GlTexture::GlTexture()
    {
        ImTextureID textureId_Gl = ImmVision_GlProvider::GenTexture();
        this->TextureId = textureId_Gl;
    }

    GlTexture::~GlTexture()
    {
        ImmVision_GlProvider::DeleteTexture(TextureId);
    }

    GlTexture::GlTexture(const ImageBuffer& image, bool isColorOrderBGR) : GlTexture()
    {
        UpdateFromImage(image, isColorOrderBGR);
    }

    void GlTexture::UpdateFromImage(const ImageBuffer& image, bool isColorOrderBGR)
    {
        if (image.empty())
            return;
        ImageBuffer mat_rgba = converted_to_rgba_image(image, isColorOrderBGR);

        ImmVision_GlProvider::Blit_RGBA_Buffer(static_cast<unsigned char*>(mat_rgba.data), mat_rgba.width, mat_rgba.height, TextureId);
        this->ImageSize = Size(mat_rgba.width, mat_rgba.height);
    }

    ImVec2 GlTexture::SizeImVec2() const
    {
        return {(float)ImageSize.width, (float)ImageSize.height};
    }

} // namespace ImmVision
