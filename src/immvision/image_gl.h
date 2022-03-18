#pragma once

#include "imgui.h"
#include <memory>

namespace ImmVision
{
struct ImageGl;
using ImageGlPtr = std::unique_ptr<ImageGl>;

struct ImageGl
{
private:
    ImageGl();
public:
    static ImageGlPtr FactorImage_From_Buffer(
        unsigned char *image_data, 
        int image_width, 
        int image_height,
        int nb_channels,
        bool flip_RedBlue
        );
    static ImageGlPtr FactorImage_From_RGB_Buffer(unsigned char *image_data, int image_width, int image_height);
    static ImageGlPtr FactorImage_From_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height);
    static ImageGlPtr FactorImage_From_BGR_Buffer(unsigned char *image_data, int image_width, int image_height);
    static ImageGlPtr FactorImage_From_BGRA_Buffer(unsigned char *image_data, int image_width, int image_height);

    ~ImageGl();

    void Draw(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    bool DrawButton(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));    // <0 frame_padding uses default frame padding settings. 0 for no padding

    ImVec2 imageSize;
    ImTextureID imTextureId;
};


} // namespace ImmVision
