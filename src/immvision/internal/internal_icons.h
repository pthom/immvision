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
        unsigned int GetIcon(IconType iconType);

        bool IconButton(IconType iconType, bool disabled = false, ImVec2 size = ImVec2(20.,  20.));

        void ClearIconsTextureCache();

        void DevelPlaygroundGui();

    } // namespace Icons
} // namespace ImmVision
