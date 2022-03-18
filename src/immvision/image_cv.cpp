#include "immvision/image_cv.h"

namespace ImmVision
{

ImageCv::ImageCv(const cv::Mat& image)
{
    m_imageMat = &image;
    m_imageGl.reset();
}

void ImageCv::updateTexture()
{
    m_imageGl.reset();
}

void ImageCv::updateTexture(const cv::Mat& newImage)
{
    m_imageMat = &newImage;
    m_imageGl.reset();
}

void ImageCv::_blitTexture()
{
    if (m_imageMat->type() == CV_8UC3)
        m_imageGl = ImageGl::FactorImage_From_BGR_Buffer(m_imageMat->data, m_imageMat->cols, m_imageMat->rows);
    else if (m_imageMat->type() == CV_8UC4)
        m_imageGl = ImageGl::FactorImage_From_BGRA_Buffer(m_imageMat->data, m_imageMat->cols, m_imageMat->rows);
    else
        assert(false); // bad image format!
}

void ImageCv::Draw(const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
    if (!m_imageGl)
        _blitTexture();
    m_imageGl->Draw(size, uv0, uv1, tint_col, border_col);
}

bool ImageCv::DrawButton(const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
    if (!m_imageGl)
        _blitTexture();
    return m_imageGl->DrawButton(size, uv0, uv1, frame_padding, bg_col, tint_col);
}


} // namespace ImmVision
