#pragma once
#include <opencv2/core.hpp>
#include <string>

// IMMVISION_API is a marker for public API functions. IMMVISION_STRUCT_API is a marker for public API structs (in comment lines)
// Usage of ImmVision as a shared library is not recommended. No guaranty of ABI stability is provided
#ifndef IMMVISION_API
#define IMMVISION_API
#endif

namespace ImmVision
{
    IMMVISION_API void Inspector_AddImage(
        const cv::Mat& image,
        const std::string& legend,
        const std::string& zoomKey = "",
        const std::string& colormapKey = "",
        const cv::Point2d & zoomCenter = cv::Point2d(),
        double zoomRatio = -1.,
        bool isColorOrderBGR = true
    );

    IMMVISION_API void Inspector_Show();

    IMMVISION_API void Inspector_ClearImages();

} // namespace ImmVision