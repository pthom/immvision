#pragma once
#include "immvision/immvision_types.h"
#include <string>
#include "immvision/image.h"

// IMMVISION_API is a marker for public API functions. IMMVISION_STRUCT_API is a marker for public API structs (in comment lines)
// Usage of ImmVision as a shared library is not recommended. No guaranty of ABI stability is provided
#ifndef IMMVISION_API
#define IMMVISION_API
#endif

namespace ImmVision
{
    // Add an image to the inspector. Call this from anywhere (e.g. at different steps
    // of an image processing pipeline). Later, call Inspector_Show() to display all collected images.
    //
    // :param image:
    //     The image to add.
    //     C++: accepts ImageBuffer directly, or cv::Mat (implicit conversion, zero-copy).
    //     Python: pass a numpy.ndarray.
    IMMVISION_API void Inspector_AddImage(
        const ImageBuffer& image,
        const std::string& legend,
        const std::string& zoomKey = "",
        const std::string& colormapKey = "",
        const Point2d & zoomCenter = Point2d(),
        double zoomRatio = -1.
    );

    IMMVISION_API void Inspector_Show();

    IMMVISION_API void Inspector_ClearImages();

} // namespace ImmVision
