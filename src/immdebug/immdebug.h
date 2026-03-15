#pragma once

#include "immvision/immvision_types.h"
#include <string>

namespace ImmVision
{
    void ImmDebug(
        const ImageBuffer & image,
        const std::string & legend = "",
        const Point2d & zoomCenter = Point2d(),
        double zoomRatio = -1.,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = "",
        bool isColorOrderBGR = true
    );

}
