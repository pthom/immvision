#pragma once

#include "imgui.h"
#include <opencv2/core.hpp>

// Some extensions to ImGui, specific to ImmVision
namespace ImGuiImm
{
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags);

    ImVec2 ComputeDisplayImageSize(ImVec2 askedImageSize, ImVec2 realImageSize);
    cv::Size ComputeDisplayImageSize(cv::Size askedImageSize, cv::Size realImageSize);

    void PushDisabled();
    void PopDisabled();
    void SameLineAlignRight(float rightMargin = 0.f, float alignRegionWidth = -1.f);

    bool CollapsingHeaderFixedWidth(const char* name, float width, ImGuiTreeNodeFlags flags = 0);

    // cf https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353
    void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel();

    void BeginGroupPanel_FlagBorder(const char* name, bool draw_border, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel_FlagBorder();
    ImVec2 GroupPanel_FlagBorder_LastKnownSize(const char* name);

}

