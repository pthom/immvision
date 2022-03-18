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


        void Draw(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
        bool DrawButton(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));         // <0 frame_padding uses default frame padding settings. 0 for no padding

        void Blit_Buffer(
            unsigned char *image_data,
            int image_width,
            int image_height,
            int nb_channels,
            bool flip_RedBlue
        );
        void Blit_RGB_Buffer(unsigned char *image_data, int image_width, int image_height);
        void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height);
        void Blit_BGR_Buffer(unsigned char *image_data, int image_width, int image_height);
        void Blit_BGRA_Buffer(unsigned char *image_data, int image_width, int image_height);

        // members
        ImVec2 imageSize;
        ImTextureID imTextureId;
    };


    struct GlTextureCv : public GlTexture
    {
        GlTextureCv() = default;
        GlTextureCv(const cv::Mat& mat);
        virtual ~GlTextureCv() = default;

        void BlitMat(const cv::Mat& mat);
    };

} // namespace ImmVision
