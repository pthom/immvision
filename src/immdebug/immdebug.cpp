#include "immdebug/immdebug.h"
#include "immdebug/immdebug_internal.h"

namespace ImmVision
{
    void ImmDebug(
        const cv::Mat & image,
        const std::string & legend,
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey,
        bool isColorOrderBGR
    )
    {
        ImmDebug_Internal::ImagePayload imagePayload {
            image,
            legend,
            zoomCenter,
            zoomRatio,
            zoomKey,
            colorAdjustmentsKey,
            isColorOrderBGR
        };
        ImmDebug_Internal::SaveImagePayload(imagePayload);
    }

    void LaunchImmDebugViewer()
    {

    }

}