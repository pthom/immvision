#include "immvision/internal/drawing/internal_icons.h"
#include "immvision/imgui_imm.h"
#include "imgui_internal.h"
#include <cmath>


namespace ImmVision
{
    namespace Icons
    {
        // Helper: compute a point at (angle, distance) from origin
        static ImVec2 PointFromOther(ImVec2 o, float angleDegree, float distance)
        {
            float angleRadian = -angleDegree / 180.f * 3.14159265f;
            return ImVec2(o.x + cosf(angleRadian) * distance, o.y + sinf(angleRadian) * distance);
        }

        static void DrawMagnifier(ImDrawList* dl, ImVec2 tl, float size, IconType iconType)
        {
            // The glass is centered in the upper-right area, with the handle pointing to the lower-left
            float radius = size * 0.24f;
            // Offset the glass center so the whole icon (glass + handle) is visually centered
            ImVec2 btnCenter(tl.x + size * 0.5f, tl.y + size * 0.5f);
            ImVec2 glassCenter(btnCenter.x + size * 0.06f, btnCenter.y - size * 0.06f);
            ImU32 color = IM_COL32(255, 255, 255, 255);
            ImU32 shadow = IM_COL32(100, 100, 100, 180);
            float thick = size * 0.06f;
            float handleThick = size * 0.1f;

            // Handle endpoints
            ImVec2 handleInner = PointFromOther(glassCenter, 225.f, radius * 1.05f);
            ImVec2 handleOuter = PointFromOther(glassCenter, 225.f, radius * 1.85f);
            ImVec2 shadowOff(size * 0.02f, size * 0.02f);

            // Shadow
            dl->AddCircle(ImVec2(glassCenter.x + shadowOff.x, glassCenter.y + shadowOff.y), radius, shadow, 0, thick);
            dl->AddLine(ImVec2(handleInner.x + shadowOff.x, handleInner.y + shadowOff.y),
                        ImVec2(handleOuter.x + shadowOff.x, handleOuter.y + shadowOff.y), shadow, handleThick);

            // Magnifier glass
            dl->AddCircle(glassCenter, radius, color, 0, thick);
            // Handle
            dl->AddLine(handleInner, handleOuter, color, handleThick);

            // Plus/minus/1 inside the glass — centered on glassCenter
            float symbolLen = radius * 0.5f;
            float symbolThick = size * 0.05f;
            ImVec2 symbolCenter = glassCenter;
            symbolCenter.x -= size * 0.02f;
            symbolCenter.y -= size * 0.02f;
            if (iconType == IconType::ZoomPlus)
            {
                dl->AddLine(ImVec2(symbolCenter.x - symbolLen, symbolCenter.y),
                            ImVec2(symbolCenter.x + symbolLen, symbolCenter.y), color, symbolThick);
                dl->AddLine(ImVec2(symbolCenter.x, symbolCenter.y - symbolLen),
                            ImVec2(symbolCenter.x, symbolCenter.y + symbolLen), color, symbolThick);
            }
            else if (iconType == IconType::ZoomMinus)
            {
                dl->AddLine(ImVec2(symbolCenter.x - symbolLen, symbolCenter.y),
                            ImVec2(symbolCenter.x + symbolLen, symbolCenter.y), color, symbolThick);
            }
            else if (iconType == IconType::ZoomScaleOne)
            {
                // "1" inside the glass — vertical stroke + serif
                float h = radius * 0.4f;
                ImVec2 top(symbolCenter.x, symbolCenter.y - h);
                ImVec2 bot(symbolCenter.x, symbolCenter.y + h);
                dl->AddLine(top, bot, color, symbolThick);
                ImVec2 serif(top.x - radius * 0.15f, top.y + radius * 0.15f);
                dl->AddLine(top, serif, color, symbolThick);
            }
        }

        static void DrawFullView(ImDrawList* dl, ImVec2 tl, float size)
        {
            size *= 0.95;

            ImU32 color = IM_COL32(255, 255, 255, 255);
            float margin = size * 0.15f;
            float armLen = size * 0.25f;
            float thick = size * 0.07f;

            // Four corner brackets
            for (int iy = 0; iy <= 1; ++iy)
            {
                for (int ix = 0; ix <= 1; ++ix)
                {
                    ImVec2 corner(
                        ix == 0 ? tl.x + margin : tl.x + size - margin,
                        iy == 0 ? tl.y + margin : tl.y + size - margin);
                    float dx = ix == 0 ? armLen : -armLen;
                    float dy = iy == 0 ? armLen : -armLen;
                    dl->AddLine(corner, ImVec2(corner.x + dx, corner.y), color, thick);
                    dl->AddLine(corner, ImVec2(corner.x, corner.y + dy), color, thick);
                }
            }
        }

        static void DrawAdjustLevels(ImDrawList* dl, ImVec2 tl, float size)
        {
            ImU32 color = IM_COL32(255, 255, 255, 255);
            float thick = size * 0.08f;
            float sliderThick = size * 0.15f;

            tl.x -= size * 0.02f;
            float yMin = tl.y + size * 0.15f;
            float yMax = tl.y + size * 0.85f;
            int nbBars = 3;
            float sliderPositions[] = { 0.70f, 0.50f, 0.30f };

            for (int bar = 0; bar < nbBars; ++bar)
            {
                float xBar = tl.x + ((float)bar / ((float)nbBars + 0.17f) + 0.2f) * size;
                dl->AddLine(ImVec2(xBar, yMin), ImVec2(xBar, yMax), color, thick);

                float ySlider = tl.y + sliderPositions[bar] * size;
                float halfW = size * 0.08f;
                dl->AddLine(ImVec2(xBar - halfW, ySlider), ImVec2(xBar + halfW, ySlider), color, sliderThick);
            }
        }

        bool IconButton(IconType iconType, bool disabled)
        {
            ImGui::PushID((int)iconType);
            if (disabled)
                ImGuiImm::PushDisabled();

            float btnSize = ImGui::GetFontSize() * 1.5f;
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            bool clicked = ImGui::Button("##btn", ImVec2(btnSize, btnSize));

            // Draw icon shapes on top of the button
            ImDrawList* dl = ImGui::GetWindowDrawList();
            switch (iconType)
            {
            case IconType::ZoomPlus:
            case IconType::ZoomMinus:
            case IconType::ZoomScaleOne:
                DrawMagnifier(dl, cursorPos, btnSize, iconType);
                break;
            case IconType::ZoomFullView:
                DrawFullView(dl, cursorPos, btnSize);
                break;
            case IconType::AdjustLevels:
                DrawAdjustLevels(dl, cursorPos, btnSize);
                break;
            }

            if (disabled)
                ImGuiImm::PopDisabled();
            ImGui::PopID();
            return disabled ? false : clicked;
        }

        void ClearIconsTextureCache()
        {
            // No texture cache to clear anymore — icons are drawn via DrawList
        }

        void DevelPlaygroundGui()
        {
            IconButton(IconType::ZoomPlus);
            ImGui::SameLine();
            IconButton(IconType::ZoomMinus);
            ImGui::SameLine();
            IconButton(IconType::ZoomScaleOne);
            ImGui::SameLine();
            IconButton(IconType::ZoomFullView);
            ImGui::SameLine();
            IconButton(IconType::AdjustLevels);
        }

    } // namespace Icons

} // namespace ImmVision
