#pragma once

#include "imgui.h"
#include <opencv2/core.hpp>
#include <vector>

namespace ImmVision
{
    // !pydef_struct
    // Color adjustments (esp. useful for a float matrix)
    struct ColorAdjustmentsValues
    {
        // Pre-multiply values by a Factor before displaying
        double Factor = 1.;
        // Add a delta to the values before displaying
        double Delta = 0.;
    };


    // !pydef_struct
    // Contains information about the mouse inside an image
    struct MouseInformation
    {
        // Mouse position in the original image/matrix
        // This position is given with float coordinates, and will be (-1., -1.) if the mouse is not hovering the image
        cv::Point2d MousePosition = cv::Point2d(-1., -1.);
        // Mouse position in the displayed portion of the image (the original image can be zoomed,
        // and only show a subset if it may be shown).
        // This position is given with integer coordinates, and will be (-1, -1) if the mouse is not hovering the image
        cv::Point MousePosition_Displayed = cv::Point(-1, -1);

        //
        // Note: you can query ImGui::IsMouseDown(mouse_button) (c++) or imgui.is_mouse_down(mouse_button) (Python)
        //
    };


    // !pydef_struct
    // Set of display parameters and options for an Image
    struct ImageParams
    {
        //
        // ImageParams store the parameters for a displayed image
        // (as well as user selected watched pixels, selected channel, etc.)
        // Its default constructor will give them reasonable choices, which you can adapt to your needs.
        // Its values will be updated when the user pans or zooms the image, adds watched pixels, etc.
        //

        //
        // Refresh Images Textures
        //

        // Refresh Image: images textures are cached. Set to true if your image matrix/buffer has changed
        // (for example, for live video images)
        bool RefreshImage = false;

        //
        // Display size and title
        //

        // Size of the displayed image (can be different from the matrix size)
        // If you specify only the width or height (e.g (300, 0), then the other dimension
        // will be calculated automatically, respecting the original image w/h ratio.
        cv::Size ImageDisplaySize = cv::Size();
        // Title displayed in the border
        std::string Legend = "Image";

        //
        // Zoom and Pan (represented by an affine transform matrix, of size 3x3)
        //

        // ZoomPanMatrix can be created using MakeZoomPanMatrix to create a view centered around a given point
        cv::Matx33d ZoomPanMatrix = cv::Matx33d::eye();
        // If displaying several images, those with the same ZoomKey will zoom and pan together
        std::string ZoomKey = "";

        //
        // Color adjustments
        //
        // Color adjustments for float matrixes
        ColorAdjustmentsValues ColorAdjustments = ColorAdjustmentsValues();
        // If displaying several images, those with the same ColorAdjustmentsKey will adjust together
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
        // Image display options
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
        std::vector<cv::Point> WatchedPixels = std::vector<cv::Point>();
        // Shall the watched pixels be drawn on the image
        bool HighlightWatchedPixels = true;

        // Mouse position information. These values are filled after displaying an image
        MouseInformation MouseInfo = MouseInformation();
    };


    // !pydef_function
    // Create ImageParams that display the image only, with no decoration, and no user interaction
    ImageParams FactorImageParamsDisplayOnly();


    // !pydef_function
    // Create a zoom/pan matrix centered around a given point of interest
    cv::Matx33d MakeZoomPanMatrix(
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        const cv::Size displayedImageSize
    );


    // !pydef_function
    // Display an image, with full user control: zoom, pan, watch pixels, etc.
    //
    // Notes about the ImageParams:
    // - they may be modified by this function (you can extract from them the mouse position, watched pixels, etc.)
    // - thus, their scope should extend beyond the call to Image !
    //
    // If you cannot zoom/pan in a displayed image, extend the scope of the ImageParams!
    void Image(const cv::Mat& imageMatrix, ImageParams* params);


    // !pydef_function
    // Only, display the image, with no decoration, and no user interaction
    //
    // Parameters:
    //      - imageDisplaySize:
    //          Size of the displayed image (can be different from the matrix size)
    //          If you specify only the width or height (e.g (300, 0), then the other dimension
    //          will be calculated automatically, respecting the original image w/h ratio.
    //      - refreshImage: images textures are cached. Set to true if your image matrix/buffer has changed
    //          (for example, for live video images)
    //
    // Todo: add a global for isBgrOrBgra
    void ImageDisplay(
        const cv::Mat& imageMatrix,
        const cv::Size& imageDisplaySize = cv::Size(),
        bool refreshImage = false,
        bool showOptions = true,
        bool isBgrOrBgra = true
        );


    void ClearTextureCache();
} // namespace ImmVision
