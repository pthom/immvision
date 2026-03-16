#pragma once

#include "immvision/immvision_types.h"
#include <string>
#include <optional>

namespace ImmVision
{
    namespace ImmDebug_Internal
    {
        struct ImagePayload
        {
            ImageBuffer Image;
            std::string Legend = "";
            Point2d ZoomCenter = Point2d();
            double ZoomRatio = -1.;
            std::string ZoomKey = "";
            std::string ColorAdjustmentsKey = "";
            bool isColorOrderBGR = true;
        };

        void                        SaveImagePayload(const ImagePayload & imagePayload);
        std::optional<ImagePayload> ReadImagePayload();

    } // namespace ImmDebug_Internal
} // namespace ImmVision
