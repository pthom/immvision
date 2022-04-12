#pragma once
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
