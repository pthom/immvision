#pragma once
#include "immvision/image_navigator.h"
#include "immvision/internal/gl/gl_texture.h"
#include <opencv2/core.hpp>

namespace ImmVision
{
    namespace ImageNavigatorDrawing
    {
        cv::Mat DrawWatchedPixels(const cv::Mat& image, const ImageNavigatorParams& params);

        void DrawGrid(cv::Mat& inOutImageRgba, const ImageNavigatorParams& params);

        cv::Mat DrawValuesOnZoomedPixels(const cv::Mat& drawingImage, const cv::Mat& valuesImage,
                                         const ImageNavigatorParams& params, bool drawPixelCoords);

        cv::Mat MakeWarpPaperBackground(cv::Size s);

        void BlitImageNavigatorTexture(
            const ImageNavigatorParams& params,
            const cv::Mat& image,
            cv::Mat& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTextureCv* outTexture
        );

    } // namespace ImageNavigatorDrawing

} // namespace ImmVision