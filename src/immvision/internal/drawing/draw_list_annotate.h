#pragma once
#include "immvision/image.h"
#include "immvision/gl_texture.h"

namespace ImmVision
{
    // Draw images annotations via ImGui DrawLists
    namespace DrawListAnnotate
    {
        // Draw backgrounds via DrawList (call before rendering the image)
        void DrawSchoolPaperBackground(const ImageParams& params, ImVec2 widgetTopLeft);
        void DrawAlphaCheckerboardBackground(const ImageParams& params, ImVec2 widgetTopLeft);

        // Draw annotations (grid, pixel values, watched pixels) via ImGui DrawList
        // in screen space, overlaying the image widget.
        // widgetTopLeft: screen position of the display area top-left.
        // image: the original image (for reading pixel values).
        void DrawAnnotationsOverlay(
            const ImageParams& params,
            const ImageBuffer& image,
            ImVec2 widgetTopLeft);

    } // namespace DrawListAnnotate

} // namespace ImmVision
