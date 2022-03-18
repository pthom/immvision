#pragma once
#include "immvision/image_gl.h"
#include <opencv2/core.hpp>

// ImCv : Immediate Computer Vision


namespace immvision
{
struct ImageCv
{
    ImageCv(const cv::Mat& image);
    void updateTexture();
    void updateTexture(const cv::Mat& newImage);

    void Draw(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    bool DrawButton(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));    // <0 frame_padding uses default frame padding settings. 0 for no padding

private:
    void _blitTexture();

    ImageGlPtr m_imageGl = nullptr;
    const cv::Mat *  m_imageMat = nullptr;
};
} // namespace immvision


