#pragma once

#include "imgui.h"
#include <opencv2/core.hpp>

namespace immvision_ImGuiExt
{
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags);

    ImVec2 ComputeDisplayImageSize(ImVec2 askedImageSize, ImVec2 realImageSize);
    cv::Size ComputeDisplayImageSize(cv::Size askedImageSize, cv::Size realImageSize);
}

