#include "immvision/internal/imgui_ext.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <sstream>
#include <vector>

namespace ImGuiImm
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
        else if ((askedImageSize.x == 0.f) && (realImageSize.y >= 1.f))
            return ImVec2(askedImageSize.y * realImageSize.x / realImageSize.y, askedImageSize.y);
        else if ((askedImageSize.y == 0.f) && (realImageSize.x >= 1.f))
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

    void PushDisabled()
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.7f);

    }
    void PopDisabled()
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    void SameLineAlignRight(float rightMargin, float alignRegionWidth)
    {
        auto window = ImGui::GetCurrentWindow();
        if (alignRegionWidth < 0.f)
            alignRegionWidth = window->Size.x;

        // Formulas taken from ImGui::ItemSize() code
        float xLineStart = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
        float y = window->DC.CursorPosPrevLine.y;

        float x = xLineStart + alignRegionWidth - rightMargin;
        ImGui::SetCursorScreenPos({x, y});
    }


    // cf https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353
    static ImVector<ImRect> s_GroupPanelLabelStack;
    void BeginGroupPanel(const char* name, const ImVec2& size)
    {
        ImGui::BeginGroup();

        auto cursorPos = ImGui::GetCursorScreenPos();
        auto itemSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();
        ImGui::BeginGroup();

        ImVec2 effectiveSize = size;
        if (size.x < 0.0f)
            effectiveSize.x = ImGui::GetWindowContentRegionWidth();
        else
            effectiveSize.x = size.x;
        ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::TextUnformatted(name);
        auto labelMin = ImGui::GetItemRectMin();
        auto labelMax = ImGui::GetItemRectMax();
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
        ImGui::BeginGroup();

        //ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

        ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x          -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x         -= frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
        ImGui::GetCurrentWindow()->Size.x                   -= frameHeight;

        auto itemWidth = ImGui::CalcItemWidth();
        ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

        s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
    }

    void EndGroupPanel()
    {
        ImGui::PopItemWidth();

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();

        ImGui::EndGroup();

        //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

        ImGui::EndGroup();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

        ImGui::EndGroup();

        auto itemMin = ImGui::GetItemRectMin();
        auto itemMax = ImGui::GetItemRectMax();
        //ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

        auto labelRect = s_GroupPanelLabelStack.back();
        s_GroupPanelLabelStack.pop_back();

        ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
        ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
        labelRect.Min.x -= itemSpacing.x;
        labelRect.Max.x += itemSpacing.x;
        for (int i = 0; i < 4; ++i)
        {
            switch (i)
            {
                // left half-plane
                case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true); break;
                    // right half-plane
                case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true); break;
                    // top
                case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true); break;
                    // bottom
                case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true); break;
            }

            ImGui::GetWindowDrawList()->AddRect(
                frameRect.Min, frameRect.Max,
                ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
                halfFrame.x);

            ImGui::PopClipRect();
        }

        ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x          += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x         += frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
        ImGui::GetCurrentWindow()->Size.x                   += frameHeight;

        ImGui::Dummy(ImVec2(0.0f, 0.0f));

        ImGui::EndGroup();
    }
}

