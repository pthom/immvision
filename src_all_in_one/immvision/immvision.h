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

    void ClearTextureCache();

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
    struct ColorAdjustmentsValues
    {
        double Factor = 1., Delta = 0.;
    };

    struct ImageNavigatorParams
    {
        cv::Size ImageDisplaySize = cv::Size();
        std::string Legend = "Image Navigator";

        cv::Matx33d ZoomMatrix = cv::Matx33d::eye();
        std::string ZoomKey = "";

        ColorAdjustmentsValues ColorAdjustments = {};
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
        double zoomRatio = -1.,
        bool isColorOrderBGR = true
    );
    void Inspector_Show();
    void Inspector_ClearImages();

} // namespace ImmVision
