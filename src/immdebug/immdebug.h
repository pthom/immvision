#pragma once

#include <opencv2/core/core.hpp>
#include <string>

namespace ImmVision
{
    void ImmDebug(
        const cv::Mat & image,
        const std::string & legend = "",
        const cv::Point2d & zoomCenter = cv::Point2d(),
        double zoomRatio = -1.,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = "",
        bool isColorOrderBGR = true
    );

    void LaunchImmDebugViewer();

}
