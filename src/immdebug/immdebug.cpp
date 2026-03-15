#include "immdebug/immdebug.h"
#include "immdebug/immdebug_internal.h"

namespace ImmVision
{
    void ImmDebug(
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
            image.to_cv_mat(),
            legend,
            cv::Point2d(zoomCenter),
            zoomRatio,
            zoomKey,
            colorAdjustmentsKey,
            isColorOrderBGR
        };
        ImmDebug_Internal::SaveImagePayload(imagePayload);
    }

}