#pragma once

#include "imgui.h"
#include <opencv2/core.hpp>

// Some extensions to ImGui, specific to ImmVision
namespace ImGuiImm
{
    // A slider that works for float, double, and long double
    // Internally, it calls ImGui::SliderFloat (so that the accuracy may be a little reduced)
    // Be sure to cast all your params when calling, especially v_min and v_max
    template<typename AnyFloat>
    bool SliderAnyFloat(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min = AnyFloat(0.),
        AnyFloat v_max = AnyFloat(1.),
        float width = 200.f,
        bool logarithmic = false,
        int nb_decimals = 6);

    template<typename AnyFloat>
    bool SliderAnyFloatLogarithmic(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min = AnyFloat(0.),
        AnyFloat v_max = AnyFloat(1.),
        float width = 200.f,
        int nb_decimals = 6);


    ImVec2 ComputeDisplayImageSize(ImVec2 askedImageSize, ImVec2 realImageSize);
    cv::Size ComputeDisplayImageSize(cv::Size askedImageSize, cv::Size realImageSize);

    void PushDisabled();
    void PopDisabled();
    void SameLineAlignRight(float rightMargin = 0.f, float alignRegionWidth = -1.f);

    // cf https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353
    void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel();

    void BeginGroupPanel_FlagBorder(const char* name, bool draw_border, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel_FlagBorder();
    ImVec2 GroupPanel_FlagBorder_LastKnownSize(const char* name);

    // Draw a fixed width Separator
    // useful when ImGui::Separator() overflows to the right
    void SeparatorFixedWidth(float width);

    void BeginGroupFixedWidth(float width);
    void EndGroupFixedWidth();

    bool ButtonWithTooltip(const std::string& label, const std::string& tooltip);
}


