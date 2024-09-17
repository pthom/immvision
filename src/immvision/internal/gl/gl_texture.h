#pragma once
#include "imgui.h"
#include <opencv2/core.hpp>
#include <memory>

namespace ImmVision
{
    /// GlTexture holds a OpenGL Texture (created via glGenTextures)
    /// You can blit (i.e transfer) image buffer onto it.
    /// The linked OpenGL texture lifetime is linked to this.
    /// GlTexture is not copiable (since it holds a reference to a texture stored on the GPU)
    struct GlTexture
    {
        GlTexture();
        virtual ~GlTexture();

        // non copiable
        GlTexture(const GlTexture& ) = delete;
        GlTexture& operator=(const GlTexture& ) = delete;

        // members
        ImVec2 mImageSize;
        ImTextureID mImTextureId;
    };

    struct GlTextureCv : public GlTexture
    {
        GlTextureCv() = default;
        GlTextureCv(const cv::Mat& mat, bool isBgrOrder);
        ~GlTextureCv() override = default;

        void BlitMat(const cv::Mat& mat, bool isBgrOrder);
    };
} // namespace ImmVision
