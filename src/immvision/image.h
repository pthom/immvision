#pragma once
#include <opencv2/core/core.hpp>
#include "imgui.h"

namespace ImmVision
{
    void Image(
        const cv::Mat &mat,
        bool refresh,
        const cv::Size& size = cv::Size(0, 0)
    );

    cv::Point2d GetImageMousePos();

    void ClearTextureCache();

} // namespace ImmVision
