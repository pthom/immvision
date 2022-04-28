#pragma once
#include "immvision/image_navigator.h"

#include <string>
#include <opencv2/core.hpp>

namespace ImmVision
{
    std::string ToString(const ColorAdjustmentsValues& params);
    std::string ToString(const ImageParams& params);
};