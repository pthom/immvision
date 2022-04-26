// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.h                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.h included by src/immvision/immvision.h                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <opencv2/core.hpp>
#include "imgui.h"

/**
@@md#ImmVision::Image

 __ImmVision::Image()__ will show an image from a cv::Mat.

Two signatures are provided:

* `ImmVision::Image(const cv::Mat &mat, bool refresh, const cv::Size& size = cv::Size(0, 0))`:
   * `mat` is the image to show
   * `refresh` indicates whether this cv::Mat was updated since it was last shown.
     If refresh is false, then the corresponding displayed texture will be fetched from a cache
     (i.e the cv::Mat data is not transferred again to the GPU)
     If refresh is true, then the corresponding displayed texture will be refreshed.
   * `size` is the desired display size.
        * If it is (0,0) then the actual cv::Mat size will be used.
        * If it is (w,0) then the display width will be w, and the displayed image height
          will conform to the vertical/horizontal ratio of the image size.
        * If it is (0,h) then the display height will be h, and the displayed image width
          will conform to the vertical/horizontal ratio of the image size.
        * If it is (w,h) then this is the displayed size

* `ImmVision::ImageStill(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0))`:
    Same a 'ImmVision::Image` where refresh is set to false

* `ImmVision::ImageAnimated(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0))`:
    Same a 'ImmVision::Image` where refresh is set to true

* `cv::Point2d ImmVision::GetImageMousePos()`: returns the position of the mouse on the last displayed image.
 This position is in *unscaled image coords*, i.e if the image was displayed with a smaller/bigger size, the returned
 mouse position is still between (0,0) and (image.cols, image.rows)

* `ImmVision::ClearTextureCache()`: clear the internal GPU texture cache. You should rarely/never need to do it.
  (the cache is handled automatically, and older GPU textures are cleared from the cache after 3 seconds if they are
  not displayed anymore)
@@md
*/

namespace ImmVision
{
    void Image(
        const cv::Mat &mat,
        bool refresh,
        const cv::Size& size = cv::Size(0, 0),
        bool isBgrOrBgra = true
    );

    inline void ImageStill(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0), bool isBgrOrBgra = true)
        { Image(mat, false, size, isBgrOrBgra); }
    inline void ImageAnimated(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0), bool isBgrOrBgra = true)
        { Image(mat, true, size, isBgrOrBgra); }

    cv::Point2d GetImageMousePos();

    void ClearImageTextureCache();
    void ClearAllTextureCaches();

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.h continued                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.h included by src/immvision/immvision.h                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


    // Set of display parameters and options for an ImageNavigator
    struct ImageNavigatorParams
    {
        //
        // ImageNavigatorParams store the parameters for an ImageNavigator
        // (as well as user selected watched pixels, selected channel, etc.)
        // Its default constructor will give them reasonable choices, which you can adapt to your needs.
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
        std::vector<cv::Point> WatchedPixels = std::vector<cv::Point>();
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