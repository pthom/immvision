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
        ImTextureID GetIcon(IconType iconType);

        bool IconButton(IconType iconType, bool disabled = false);

        void ClearIconsTextureCache();

        void DevelPlaygroundGui();

    } // namespace Icons
} // namespace ImmVision
