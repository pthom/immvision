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

    void GlTexture::Draw_DisableDragWindow(const ImVec2 &size, bool disableDragWindow) const
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->mImageSize;

        ImVec2 imageTl = ImGui::GetCursorScreenPos();
        ImVec2 imageBr(imageTl.x + size.x, imageTl.y + size.y);
        std::stringstream id;
        id << "##" << mImTextureId;
        if (disableDragWindow)
            ImGui::InvisibleButton(id.str().c_str(), size);
        else
            ImGui::Dummy(size);
        ImGui::GetWindowDrawList()->AddImage(mImTextureId, imageTl, imageBr);
    }

    void GlTexture::Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height)
    {
        ImmVision_GlProvider::Blit_RGBA_Buffer(image_data, image_width, image_height, this->mImTextureId);
        mImageSize = ImVec2((float)image_width, (float) image_height);
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
