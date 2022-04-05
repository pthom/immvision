#pragma once

#include "imgui.h"
#include <opencv2/core.hpp>
#include <vector>

namespace ImmVision
{
    struct ColorAdjustments
    {
        double Factor = 1., Delta = 0.;
    };

    struct ImageNavigatorParams
    {
        cv::Size ImageDisplaySize = cv::Size();
        std::string Legend = "Image Navigator";

        cv::Matx33d ZoomMatrix = cv::Matx33d::eye();
        std::string ZoomKey = "";

        ColorAdjustments ColorAdjustments = {};
        std::string ColorAdjustmentsKey = "";

        // Image navigation
        bool PanWithMouse = true;
        bool ZoomWithMouseWheel = true;

        // Input Image Color order: RGB or RGBA versus BGR or BGRA
        // (Note: by default OpenCV uses BGR and BGRA)
        bool IsColorOrderBGR = true;

        // Image display options
        int  SelectedChannel = -1; // if >= 0 then only this channel is displayed
        bool ShowAlphaChannelCheckerboard = true;
        // Image display options when zoom is high
        bool ShowGrid = true;
        bool DrawValuesOnZoomedPixels = true;

        // Navigator display options
        bool ShowImageInfo = true;
        bool ShowPixelInfo = true;
        bool ShowZoomButtons = true;
        bool ShowLegendBorder = true;
        bool ShowOptions = false;
        bool ShowOptionsInTooltip = false;

        // Watched Pixels
        std::vector<cv::Point> WatchedPixels;
        bool HighlightWatchedPixels = true;
    };

    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams* params,
        bool refreshImage = false
        );

    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        const cv::Size& imageDisplaySize = cv::Size(),
        const std::string& legend = "Image Navigator",
        bool refreshImage = false,
        bool showOptionsWhenAppearing = false,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = ""
    );

    cv::Matx33d MakeZoomMatrix(
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        const cv::Size displayedImageSize
    );




    ////////////////////////////////////////////
    // Inspector
    ////////////////////////////////////////////
    void Inspector_AddImage(
        const cv::Mat& image,
        const std::string& legend,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = "",
        const cv::Point2d & zoomCenter = cv::Point2d(),
        double zoomRatio = 1.,
        bool isColorOrderBGR = true
    );
    void Inspector_ShowWindow(bool* p_open = NULL);
    void Inspector_ClearImages();

} // namespace ImmVision