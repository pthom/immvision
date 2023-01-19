#pragma once

#include <opencv2/core/core.hpp>
#include <string>
#include <optional>

namespace ImmVision
{
    namespace ImmDebug_Internal
    {
        struct ImagePayload
        {
            cv::Mat Image;
            std::string Legend = "";
            cv::Point2d ZoomCenter = cv::Point2d();
            double ZoomRatio = -1.;
            std::string ZoomKey = "";
            std::string ColorAdjustmentsKey = "";
            bool isColorOrderBGR = true;
        };

        void                        SaveImagePayload(const ImagePayload & imagePayload);
        std::optional<ImagePayload> ReadImagePayload();

    } // namespace ImmDebug_Internal
} // namespace ImmVision