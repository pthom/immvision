#include "immvision/imgui_imm.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <cassert>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stack>

namespace ImGuiImm
{
    template<typename AnyFloat>
    bool SliderAnyFloat(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min,
        AnyFloat v_max,
        float width,
        bool logarithmic,
        int nb_decimals)
    {
        float vf = (float)*v;
        std::string formatString = std::string("%.") + std::to_string(nb_decimals) + "f";
        ImGui::SetNextItemWidth(width);
        ImGuiSliderFlags flags = 0;
        if (logarithmic)
            flags |= ImGuiSliderFlags_Logarithmic;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, formatString.c_str(), flags);
        if (changed)
            *v = (AnyFloat)vf;
        return changed;
    }

#define EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(AnyFloat)                       \
    template bool SliderAnyFloat<AnyFloat>(                                     \
    const char*label, AnyFloat* v, AnyFloat v_min, AnyFloat v_max, float width, \
    bool logarithmic, int nb_decimals);

    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(float);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(double);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(long double);


    template<typename AnyFloat>
    bool SliderAnyFloatLogarithmic(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min,
        AnyFloat v_max,
        float width,
        int nb_decimals)
    {
        return SliderAnyFloat(label, v, v_min, v_max, width, true, nb_decimals);
    }

#define EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(AnyFloat)                   \
    template bool SliderAnyFloatLogarithmic<AnyFloat>(                                  \
    const char*label, AnyFloat* v, AnyFloat v_min, AnyFloat v_max, float width,         \
    int nb_decimals);

    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(float);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(double);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(long double);


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
    ImmVision::Size ComputeDisplayImageSize(ImmVision::Size askedImageSize, ImmVision::Size realImageSize)
    {
        auto toSize = [](ImVec2 v) { return ImmVision::Size((int)((double)v.x + 0.5), (int)((double)v.y + 0.5)); };
        auto toImVec2 = [](ImmVision::Size v) { return ImVec2((float)v.width, (float)v.height); };
        return toSize( ComputeDisplayImageSize(toImVec2(askedImageSize), toImVec2(realImageSize)) );
    }

    void PushDisabled()
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.7f);

    }
    void PopDisabled()
    {
        ImGui::PopStyleVar();
        ImGui::PopItemFlag();
    }

    void SameLineAlignRight(float rightMargin, float alignRegionWidth)
    {
        auto window = ImGui::GetCurrentWindow();
        if (alignRegionWidth < 0.f)
            alignRegionWidth = window->Size.x;

        // Formulas taken from ImGui::ItemSize() code
        float xLineStart = IM_TRUNC(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
        float y = window->DC.CursorPosPrevLine.y;

        float x = xLineStart + alignRegionWidth - rightMargin;
        ImGui::SetCursorScreenPos({x, y});
    }



    // cf https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353
    static ImVector<ImRect> s_GroupPanelLabelStack;
    void BeginGroupPanel(const char* name, const ImVec2& size)
    {
        ImGui::BeginGroup();

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();
        ImGui::BeginGroup();

        ImVec2 effectiveSize = size;
        if (size.x < 0.0f)
            effectiveSize.x = ImGui::GetWindowWidth();
        else
            effectiveSize.x = size.x;
        ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        if (strlen(name) > 0)
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

        ImVec2 halfFrame = ImVec2(frameHeight * 0.25f * 0.5f, frameHeight * 0.5f);
        ImRect frameRect = ImRect(ImVec2(itemMin.x + halfFrame.x, itemMin.y + halfFrame.y), ImVec2(itemMax.x - halfFrame.x, itemMax.y));
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


    static std::stack<bool> s_GroupPanel_FlagBorder_DrawBorder;
    static std::stack<std::string> s_GroupPanel_FlagBorder_Names;
    static std::unordered_map<std::string, ImVec2> s_GroupPanel_FlagBorder_Sizes;

    void BeginGroupPanel_FlagBorder(const char* name, bool draw_border, const ImVec2& size)
    {
        std::string name_s(name);
        std::string name_displayed;
        {
            auto pos = name_s.find("##");
            if (pos != std::string::npos)
                name_displayed =  name_s.substr(0, pos);
            else
                name_displayed = name_s;
        }

        ImGui::BeginGroup();
        s_GroupPanel_FlagBorder_DrawBorder.push(draw_border);
        s_GroupPanel_FlagBorder_Names.push(name);
        if (draw_border)
            BeginGroupPanel(name_displayed.c_str(), size);
        else
        {
            ImGui::BeginGroup();
            if (strlen(name) > 0)
                ImGui::Text("%s", name_displayed.c_str());
        }
    }

    void EndGroupPanel_FlagBorder()
    {
        bool drawBorder = s_GroupPanel_FlagBorder_DrawBorder.top();
        s_GroupPanel_FlagBorder_DrawBorder.pop();
        if (drawBorder)
            EndGroupPanel();
        else
            ImGui::EndGroup();

        ImGui::EndGroup();

        // Store size
        {
            std::string name = s_GroupPanel_FlagBorder_Names.top();
            s_GroupPanel_FlagBorder_Names.pop();
            s_GroupPanel_FlagBorder_Sizes[name] = ImGui::GetItemRectSize();
        }
    }

    ImVec2 GroupPanel_FlagBorder_LastKnownSize(const char* name)
    {
        if (s_GroupPanel_FlagBorder_Sizes.find(name) == s_GroupPanel_FlagBorder_Sizes.end())
            return ImVec2(3.f, 3.f);
        else
            return s_GroupPanel_FlagBorder_Sizes.at(name);
    }

    std::stack<ImRect> s_OldWorkRects;
    void BeginGroupFixedWidth(float width)
    {
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(width, 1.f));
        ImRect oldWorkRect = ImGui::GetCurrentWindow()->WorkRect;
        {
            ImRect newRect = oldWorkRect;
            newRect.Max.x = ImGui::GetCursorScreenPos().x + width - ImGui::GetStyle().ItemSpacing.x;
            ImGui::GetCurrentWindow()->WorkRect = newRect;
            s_OldWorkRects.push(oldWorkRect);
        }
    }

    void EndGroupFixedWidth()
    {
        ImGui::EndGroup();
        assert(!s_OldWorkRects.empty());
        ImRect oldWorkRect = s_OldWorkRects.top();
        s_OldWorkRects.pop();
        ImGui::GetCurrentWindow()->WorkRect = oldWorkRect;
    }

    // Draw a fixed width Separator
    // useful when ImGui::Separator() overflows to the right
    void SeparatorFixedWidth(float width)
    {
        ImVec2 a = ImGui::GetCursorScreenPos();
        a.y += 4.f;
        ImVec2 b = a;
        b.x += width;
        auto col = ImGui::GetStyle().Colors[ImGuiCol_Separator];
        ImGui::GetWindowDrawList()->AddLine(a, b, ImGui::GetColorU32(col), 1.f);
        ImGui::NewLine();
    }

    bool ButtonWithTooltip(const std::string& label, const std::string& tooltip)
    {
        bool r = ImGui::Button(label.c_str());
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", tooltip.c_str());
        return r;
    }

}


