#pragma once
#include "immvision/gl_texture.h"
#include "immvision/image.h"
#include "imgui.h"

namespace ImmVision
{
    namespace ImageWidgets
    {
        Point2d DisplayTexture_TrackMouse(const GlTexture& texture, ImVec2 displaySize, bool disableDragWindow);
        void ShowImageInfo(const ImageBuffer &image, double zoomFactor);
        void ShowPixelColorWidget(const ImageBuffer &image, Point pt, const ImageParams& params);

        // If true, the collapsing headers will be synced across instances
        extern bool s_CollapsingHeader_CacheState_Sync;
        bool CollapsingHeader_OptionalCacheState(const char *name, bool forceOpen = false);
    } // namespace ImageWidgets

}
