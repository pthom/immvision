#pragma once
#include "immvision/image.h"

namespace ImmVision
{
    namespace ColorAdjustmentsUtils
    {
        bool IsNone(const ColorAdjustmentsValues& a);
        bool IsEqual(const ColorAdjustmentsValues& v1, const ColorAdjustmentsValues& v2);
        cv::Mat Adjust(const ColorAdjustmentsValues& a, const cv::Mat &image);
        ColorAdjustmentsValues ComputeInitialImageAdjustments(const cv::Mat& m);
    } // namespace ImageAdjustmentsUtils

} // namespace ImmVision