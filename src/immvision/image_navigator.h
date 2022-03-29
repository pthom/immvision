#pragma once

#include "imgui.h"
#include <opencv2/core.hpp>

namespace ImmVision
{
    struct ColorAdjustments
    {
        double Factor = 1., Delta = 0.;
    };

    struct ImageNavigatorParams
    {
        cv::Size ImageSize = cv::Size();
        std::string Legend = "";

        cv::Matx33d ZoomMatrix = cv::Matx33d::eye();
        std::string ZoomKey = "";

        ColorAdjustments ColorAdjustments = {};
        std::string ColorAdjustmentsKey = "";

        bool ShowGrid = true;
        bool ShowColorAsRGB = false;
    };

    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams& params,
        bool refresh = false
        );

} // namespace ImmVision