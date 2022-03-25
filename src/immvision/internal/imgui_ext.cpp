#include "immvision/internal/imgui_ext.h"
#include "imgui.h"

#include <sstream>
#include <vector>

namespace immvision_ImGuiExt
{
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
    {
        float vf = *v;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, format, flags);
        if (changed)
            *v = vf;
        return changed;
    }

    ImVec2 ComputeDisplayImageSize(
        ImVec2 askedImageSize,
        ImVec2 realImageSize
    )
    {
        if ((askedImageSize.x == 0.f) && (askedImageSize.y == 0.f))
            return realImageSize;
        else if (askedImageSize.x == 0.f)
            return ImVec2(askedImageSize.y * realImageSize.x / realImageSize.y, askedImageSize.y);
        else if (askedImageSize.y == 0.f)
            return ImVec2(askedImageSize.x, askedImageSize.x * realImageSize.y / realImageSize.x);
        else
            return askedImageSize;
    }
    cv::Size ComputeDisplayImageSize(cv::Size askedImageSize, cv::Size realImageSize)
    {
        auto toSize = [](ImVec2 v) { return cv::Size((int)(v.x + 0.5), (int)(v.y + 0.5)); };
        auto toImVec2 = [](cv::Size v) { return ImVec2((float)v.width, (float)v.height); };
        return toSize( ComputeDisplayImageSize(toImVec2(askedImageSize), toImVec2(realImageSize)) );
    }

}

