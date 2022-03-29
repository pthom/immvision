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
            ZoomFullView
        };
        ImTextureID GetIcon(IconType iconType);

        bool IconButton(IconType iconType, bool disabled = false, ImVec2 size = ImVec2(20.,  20.));

        void DevelPlaygroundGui();

    } // namespace Icons
} // namespace ImmVision
