#pragma once
#include "imgui.h"

namespace ImmVision
{
    namespace Icons
    {
        enum class IconType
        {
            ZoomPlus,
            ZoomMinus,
            ZoomScaleOne,
            ZoomFullView,
            AdjustLevels,
        };

        // Draw an icon button using DrawList (no textures needed)
        bool IconButton(IconType iconType, bool disabled = false);

        // No-op (kept for API compatibility with gl_provider reset)
        void ClearIconsTextureCache();

        void DevelPlaygroundGui();

    } // namespace Icons
} // namespace ImmVision
