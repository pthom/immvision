#pragma once
#include "immvision/immvision_types.h"

namespace ImmVision
{
    // Convert any ImageBuffer to RGBA uint8.
    // isBgrOrder only applies to uint8 images (OpenCV convention).
    ImageBuffer converted_to_rgba_image(const ImageBuffer& inputMat, bool isBgrOrder);

} // namespace ImmVision
