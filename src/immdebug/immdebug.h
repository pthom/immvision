#pragma once

#include "immvision/immvision_types.h"
#include <string>

namespace ImmVision
{
    // Send an image to the immdebug_viewer for visual debugging.
    // Assumes RGB channel order (the default for ImRead, stb, numpy, etc.).
    void ImmDebug(
        const ImageBuffer & image,
        const std::string & legend = "",
        const Point2d & zoomCenter = Point2d(),
        double zoomRatio = -1.,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = ""
    );

    // Same as ImmDebug, but for images in BGR channel order (OpenCV default).
    void ImmDebugBgr(
        const ImageBuffer & image,
        const std::string & legend = "",
        const Point2d & zoomCenter = Point2d(),
        double zoomRatio = -1.,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = ""
    );

}
