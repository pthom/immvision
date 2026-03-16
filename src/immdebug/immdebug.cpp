#include "immdebug/immdebug.h"
#include "immdebug/immdebug_internal.h"

namespace ImmVision
{
    static void ImmDebugImpl(
        const ImageBuffer & image,
        const std::string & legend,
        const Point2d & zoomCenter,
        double zoomRatio,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey,
        bool isColorOrderBGR
    )
    {
        ImmDebug_Internal::ImagePayload imagePayload {
            image.clone(),
            legend,
            zoomCenter,
            zoomRatio,
            zoomKey,
            colorAdjustmentsKey,
            isColorOrderBGR
        };
        ImmDebug_Internal::SaveImagePayload(imagePayload);
    }

    void ImmDebug(
        const ImageBuffer & image,
        const std::string & legend,
        const Point2d & zoomCenter,
        double zoomRatio,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey
    )
    {
        ImmDebugImpl(image, legend, zoomCenter, zoomRatio, zoomKey, colorAdjustmentsKey, false);
    }

    void ImmDebugBgr(
        const ImageBuffer & image,
        const std::string & legend,
        const Point2d & zoomCenter,
        double zoomRatio,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey
    )
    {
        ImmDebugImpl(image, legend, zoomCenter, zoomRatio, zoomKey, colorAdjustmentsKey, true);
    }

}
