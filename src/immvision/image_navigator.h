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
        std::string Legend = "Image Navigator";

        cv::Matx33d ZoomMatrix = cv::Matx33d::eye();
        std::string ZoomKey = "";

        ColorAdjustments ColorAdjustments = {};
        std::string ColorAdjustmentsKey = "";

        // Image navigation
        bool PanWithMouse = true;

        // Image display options
        bool ShowGrid = true;
        bool ShowColorAsRGB = false; // By default, color values are displayed as BGR or BGRA (as in OpenCV)
        int  SelectedChannel = -1;
        bool ShowAlphaChannelCheckerboard = true;

        // Navigator display options
        bool ShowImageInfo = true;
        bool ShowPixelInfo = true;
        bool ShowZoomButtons = true;
        bool ShowLegendBorder = true;

        bool ShowOptions = false;
        bool ShowOptionsInTooltip = true;
    };

    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams& params,
        bool refresh = false
        );

} // namespace ImmVision