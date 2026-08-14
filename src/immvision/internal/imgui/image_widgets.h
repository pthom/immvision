#pragma once
#include "immvision/gl_texture.h"
#include "immvision/image.h"
#include "imgui.h"

namespace ImmVision
{
    namespace ImageWidgets
    {
        Point2d DisplayTexture_TrackMouse(const GlTexture& texture, ImVec2 displaySize, bool disableDragWindow);

        // UV-aware version: displays a texture with UV coordinates for GPU-based pan/zoom.
        // widgetOffset: pixel offset within the display area for the image.
        // widgetSize: actual size of the image widget (may be smaller than displaySize when zoomed out).
        // Returns mouse position in display-pixel coordinates (relative to displaySize top-left), or (-1,-1) if not hovering.
        Point2d DisplayTexture_TrackMouse_Uv(
            const GlTexture& texture, ImVec2 displaySize,
            ImVec2 uv0, ImVec2 uv1,
            ImVec2 widgetOffset, ImVec2 widgetSize,
            bool disableDragWindow);
        void ShowImageInfo(const ImageBuffer &image, double zoomFactor);
        void ShowPixelColorWidget(const ImageBuffer &image, Point pt, const ImageParams& params);

        // True if the image texture should be sampled with nearest-neighbor filtering,
        // given the interpolation mode and the current zoom factor
        bool ShallUseNearestSampler(const ImageParams& params);

        // Request nearest-neighbor sampling for the next AddImage calls in the current draw list.
        // Needed since imgui 1.92.8: the renderer backends' samplers override
        // glTexParameteri settings set on the texture (no-op on older imgui versions).
        void PushNearestTextureSampler();
        // Restore the default (linear) sampler
        void PopNearestTextureSampler();

        // If true, the collapsing headers will be synced across instances
        extern bool s_CollapsingHeader_CacheState_Sync;
        bool CollapsingHeader_OptionalCacheState(const char *name, bool forceOpen = false);
    } // namespace ImageWidgets

}
