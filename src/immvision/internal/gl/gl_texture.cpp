#include "immvision/gl_texture.h"
#include "immvision/internal/cv/cv_drawing_utils.h"
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

    GlTexture::GlTexture(const cv::Mat& image, bool isColorOrderBGR) : GlTexture()
    {
        UpdateFromImage(image, isColorOrderBGR);
    }

    void GlTexture::UpdateFromImage(const cv::Mat& image, bool isColorOrderBGR)
    {
        if (image.empty())
            return;
        cv::Mat mat_rgba = CvDrawingUtils::converted_to_rgba_image(image, isColorOrderBGR);

        ImmVision_GlProvider::Blit_RGBA_Buffer(mat_rgba.data, mat_rgba.cols, mat_rgba.rows, TextureId);
        this->Size = mat_rgba.size();
    }

    ImVec2 GlTexture::SizeImVec2() const
    {
        return {(float)Size.width, (float)Size.height};
    }

} // namespace ImmVision
