#pragma once
#include "immvision/internal/gl/gl_texture.h"
#include "immvision/image.h"
#include "imgui.h"
#include <opencv2/core.hpp>

namespace ImmVision
{
    namespace ImageWidgets
    {
        cv::Point2d DisplayTexture_TrackMouse(const GlTextureCv& texture, ImVec2 displaySize, bool disableDragWindow);
        void ShowImageInfo(const cv::Mat &image, double zoomFactor);
        void ShowPixelColorWidget(const cv::Mat &image, cv::Point pt, const ImageParams& params);

        // If true, the collapsing headers will be synced across instances
        extern bool s_CollapsingHeader_CacheState_Sync;
        bool CollapsingHeader_OptionalCacheState(const char *name, bool forceOpen = false);
    } // namespace ImageWidgets

}
