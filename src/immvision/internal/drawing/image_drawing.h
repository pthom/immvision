#pragma once
#include "immvision/image.h"
#include "immvision/internal/gl/gl_texture.h"
#include <opencv2/core.hpp>

namespace ImmVision
{
    namespace ImageDrawing
    {
        cv::Mat DrawWatchedPixels(const cv::Mat& image, const ImageParams& params);

        void DrawGrid(cv::Mat& inOutImageRgba, const ImageParams& params);

        cv::Mat DrawValuesOnZoomedPixels(const cv::Mat& drawingImage, const cv::Mat& valuesImage,
                                         const ImageParams& params, bool drawPixelCoords);

        cv::Mat MakeSchoolPaperBackground(cv::Size s);

        void BlitImageTexture(
            const ImageParams& params,
            const cv::Mat& image,
            cv::Mat& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTextureCv* outTexture
        );

        bool HasColormapParam(const ImageParams& params);

    } // namespace ImageDrawing

} // namespace ImmVision