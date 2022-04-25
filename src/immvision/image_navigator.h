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
        double Factor = 1.;
        double Delta = 0.;
    };

    
    struct ImageNavigatorParams
    {
        //
        // struct ImageNavigatorParams
        // Stores all the display parameters and options for an ImageNavigator,
        // (its parameters default values are reasonable choices)
        // Its values will be updated when the user pans or zooms the image, adds watched pixels, etc.
        //

        //
        // Display size and title
        //

        // Size of the navigator (can be different from the image size)
        cv::Size ImageDisplaySize = cv::Size();
        // Title displayed in the border
        std::string Legend = "Image Navigator";

        //
        // Zoom and Pan (represented by an affine transform matrix, of size 3x3)
        //

        // ZoomPanMatrix can be created using MakeZoomPanMatrix to create a view centered around a given point
        cv::Matx33d ZoomPanMatrix = cv::Matx33d::eye();
        // If displaying several navigators, those with the same ZoomKey will zoom and pan together
        std::string ZoomKey = "";

        //
        // Color adjustments
        //
        // Color adjustments for float matrixes
        ColorAdjustmentsValues ColorAdjustments = ColorAdjustmentsValues();
        // If displaying several navigators, those with the same ColorAdjustmentsKey will adjust together
        std::string ColorAdjustmentsKey = "";

        //
        // Zoom and pan with the mouse
        //
        bool PanWithMouse = true;
        bool ZoomWithMouseWheel = true;

        // Color Order: RGB or RGBA versus BGR or BGRA (Note: by default OpenCV uses BGR and BGRA)
        bool IsColorOrderBGR = true;

        //
        // Image display options
        //
        // if SelectedChannel >= 0 then only this channel is displayed
        int  SelectedChannel = -1;
        // show a checkerboard behind transparent portions of 4 channels RGBA images
        bool ShowAlphaChannelCheckerboard = true;
        // Grid displayed when the zoom is high
        bool ShowGrid = true;
        // Pixel values show when the zoom is high
        bool DrawValuesOnZoomedPixels = true;

        //
        // Navigator display options
        //
        // Show matrix type and size
        bool ShowImageInfo = true;
        // Show pixel values
        bool ShowPixelInfo = true;
        // Show buttons that enable to zoom in/out (the mouse wheel also zoom)
        bool ShowZoomButtons = true;
        // Show a rectangular border with the legend
        bool ShowLegendBorder = true;
        // Open the options panel
        bool ShowOptions = false;
        // If set to true, then the option panel will be displayed in a transient tooltip window
        bool ShowOptionsInTooltip = false;

        //
        // Watched Pixels
        //
        // List of Watched Pixel coordinates
        std::vector<cv::Point> WatchedPixels;
        // Shall the watched pixels be drawn on the image
        bool HighlightWatchedPixels = true;
    };

    cv::Matx33d MakeZoomPanMatrix(
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        const cv::Size displayedImageSize
    );


    cv::Point2d ImageNavigator(const cv::Mat& image, ImageNavigatorParams* params, bool refreshImage = false);


    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        const cv::Size& imageDisplaySize = cv::Size(),
        const std::string& legend = "Image Navigator",
        bool refreshImage = false,
        bool showOptionsWhenAppearing = false,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = ""
    );



    void ClearNavigatorTextureCache();


} // namespace ImmVision
