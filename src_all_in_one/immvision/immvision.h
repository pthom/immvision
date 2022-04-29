// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.h                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.h included by src/immvision/immvision.h                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "imgui.h"
#include <opencv2/core.hpp>
#include <vector>

namespace ImmVision
{
    // Color adjustments (esp. useful for a float matrix)
    struct ColorAdjustmentsValues
    {
        // Pre-multiply values by a Factor before displaying
        double Factor = 1.;
        // Add a delta to the values before displaying
        double Delta = 0.;
    };


    // Contains information about the mouse inside an image
    struct MouseInformation
    {
        // Mouse position in the original matrix/buffer (float values). Will be (-1, -1) if mouse is not hovering
        cv::Point2d MousePosition_Matrix = cv::Point2d(-1., -1.);
        // Mouse position in the displayed image, which can be zoomed, and only show a subset of the matrix (integer values).
        // Will be (-1, -1) if mouse is not hovering
        cv::Point MousePosition_Displayed = cv::Point(-1, -1);

        //
        // Note: you can query ImGui::IsMouseDown(mouse_button) (c++) or imgui.is_mouse_down(mouse_button) (Python)
        //
    };


    // Set of display parameters and options for an Image
    struct ImageParams
    {
        //
        // ImageParams store the parameters for a displayed image
        // (as well as user selected watched pixels, selected channel, etc.)
        // Its default constructor will give them reasonable choices, which you can adapt to your needs.
        // Its values will be updated when the user pans or zooms the image, adds watched pixels, etc.
        //

        // Refresh Image: images textures are cached. Change this boolean value if your image matrix/buffer has changed
        bool RefreshImage = false;

        // Mouse position information. These values are filled after displaying an image
        MouseInformation MouseInfo;

        //
        // Display size and title
        //

        // Size of the displayed image (can be different from the matrix size)
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
    };

    cv::Matx33d MakeZoomPanMatrix(
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        const cv::Size displayedImageSize
    );


    cv::Point2d Image(const cv::Mat& image, ImageParams* params, bool refreshImage = false);


    inline void Image2(const cv::Mat& image, const ImageParams& params) {}

//    cv::Point2d Image(
//        const cv::Mat& image,
//        const cv::Size& imageDisplaySize = cv::Size(),
//        const std::string& legend = "Image",
//        bool refreshImage = false,
//        bool showOptionsWhenAppearing = false,
//        const std::string& zoomKey = "",
//        const std::string& colorAdjustmentsKey = ""
//    );

//    void ImageOld(
//        const cv::Mat &mat,
//        bool refresh,
//        const cv::Size& size = cv::Size(0, 0),
//        bool isBgrOrBgra = true
//    );




    void ClearTextureCache();


} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.h continued                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/inspector.h included by src/immvision/immvision.h                        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>

namespace ImmVision
{
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