#include "immvision/internal/gl/gl_texture.h"

#include "immvision/internal/cv/cv_drawing_utils.h"
#include "immvision/internal/gl/gl_provider.h"


namespace ImmVision
{
    GlTexture::GlTexture()
    {
        ImTextureID textureId_Gl = ImmVision_GlProvider::GenTexture();
        this->mImTextureId = textureId_Gl;
    }

    GlTexture::~GlTexture()
    {
        ImmVision_GlProvider::DeleteTexture(mImTextureId);
    }

    //
    // ImageTextureCv
    //
    GlTextureCv::GlTextureCv(const cv::Mat& mat, bool isBgrOrder) : GlTextureCv()
    {
        BlitMat(mat, isBgrOrder);
    }

    void GlTextureCv::BlitMat(const cv::Mat& mat, bool isBgrOrder)
    {
        if (mat.empty())
            return;
        cv::Mat mat_rgba = CvDrawingUtils::converted_to_rgba_image(mat, isBgrOrder);

        ImmVision_GlProvider::Blit_RGBA_Buffer(mat_rgba.data, mat_rgba.cols, mat_rgba.rows, mImTextureId);
        this->mImageSize = ImVec2((float)mat_rgba.cols, (float) mat_rgba.rows);
    }
} // namespace ImmVision
