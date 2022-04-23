#pragma once

#include "imgui.h"
#include <opencv2/core.hpp>
#include <vector>

namespace ImmVision
{
    //
    // struct ColorAdjustmentsValues: Color adjustments for float matrixes
    //
    // For float matrixes, pre-multiply and add a Delta before displaying
    // (the navigator expects values between 0 and 1, and will convert them to 0..255)
    struct ColorAdjustmentsValues
    {
        double Factor = 1., Delta = 0.;
    };

    
    //
    // struct ImageNavigatorParams
    //
    // Stores all the display parameters and options for an ImageNavigator,
    // (its parameters default values are reasonable choices)
    struct ImageNavigatorParams
    {
        //
        // Display size and title
        //

        // Size of the navigator (can be different from the image size)
        cv::Size ImageDisplaySize = cv::Size();
        // Title displayed in the border
        std::string Legend = "Image Navigator";

        //
        // Zoom and Pan
        //

        // ZoomMatrix can be created using MakeZoomMatrix to create a view centered around a given point
        cv::Matx33d ZoomMatrix = cv::Matx33d::eye();
        // If displaying several navigators, those with the same ZoomKey will zoom and pan together
        std::string ZoomKey = "";

        //
        // Color adjustments for float matrixes
        //

        ColorAdjustmentsValues ColorAdjustments = {};
        // If displaying several navigators, those with the same ColorAdjustmentsKey will adjust together
        std::string ColorAdjustmentsKey = "";

        //
        // Zoom and pan with the mouse
        //
        bool PanWithMouse = true;
        bool ZoomWithMouseWheel = true;

        //
        // Color Order: RGB or RGBA versus BGR or BGRA (Note: by default OpenCV uses BGR and BGRA)
        //
        bool IsColorOrderBGR = true;

        //
        // Image display options
        //
        // if SelectedChannel >= 0 then only this channel is displayed
        int  SelectedChannel = -1;
        bool ShowAlphaChannelCheckerboard = true;
        // Image display options when the zoom is high
        bool ShowGrid = true;
        bool DrawValuesOnZoomedPixels = true;

        //
        // Navigator display options
        //
        bool ShowImageInfo = true;
        bool ShowPixelInfo = true;
        bool ShowZoomButtons = true;
        bool ShowLegendBorder = true;
        bool ShowOptions = false;
        bool ShowOptionsInTooltip = false;

        //
        // Watched Pixels
        //
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


    void ClearNavigatorTextureCache();


    ////////////////////////////////////////////
    // Inspector
    ////////////////////////////////////////////
    void Inspector_AddImage(
        const cv::Mat& image,
        const std::string& legend,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = "",
        const cv::Point2d & zoomCenter = cv::Point2d(),
        double zoomRatio = -1.,
        bool isColorOrderBGR = true
    );
    void Inspector_Show();
    void Inspector_ClearImages();

} // namespace ImmVision
