#include "immvision/internal/imgui/image_widgets.h"
#include "immvision/internal/cv/matrix_info_utils.h"

#include <map>
#include <sstream>

namespace ImmVision
{
    namespace ImageWidgets
    {
        void GlTexture_Draw_DisableDragWindow(const GlTexture& texture, const ImVec2 &size, bool disableDragWindow)
        {
            ImVec2 size_(size);
            if (size.x == 0.f)
                size_ = texture.SizeImVec2();

            ImVec2 imageTl = ImGui::GetCursorScreenPos();
            ImVec2 imageBr(imageTl.x + size.x, imageTl.y + size.y);
            std::stringstream id;
            id << "##" << texture.TextureId;
            if (disableDragWindow)
                ImGui::InvisibleButton(id.str().c_str(), size);
            else
                ImGui::Dummy(size);
            ImGui::GetWindowDrawList()->AddImage(texture.TextureId, imageTl, imageBr);
        }

        float FontSizeRatio()
        {
            float r = ImGui::GetFontSize() / 14.5;
            return r;
        }

        Point2d DisplayTexture_TrackMouse(const GlTexture& texture, ImVec2 displaySize, bool disableDragWindow)
        {
            ImVec2 imageTopLeft = ImGui::GetCursorScreenPos();
            GlTexture_Draw_DisableDragWindow(texture, displaySize, disableDragWindow);
            bool isImageHovered = ImGui::IsItemHovered();
            ImVec2 mouse = ImGui::GetMousePos();
            if (isImageHovered)
                return Point2d((double)(mouse.x - imageTopLeft.x), (double)(mouse.y - imageTopLeft.y));
            else
                return Point2d(-1., -1.);
        }

        Point2d DisplayTexture_TrackMouse_Uv(
            const GlTexture& texture, ImVec2 displaySize,
            ImVec2 uv0, ImVec2 uv1,
            ImVec2 widgetOffset, ImVec2 widgetSize,
            bool disableDragWindow)
        {
            ImVec2 areaTopLeft = ImGui::GetCursorScreenPos();

            // Reserve the full display area for consistent layout and mouse interaction
            std::stringstream id;
            id << "##" << texture.TextureId;
            if (disableDragWindow)
                ImGui::InvisibleButton(id.str().c_str(), displaySize);
            else
                ImGui::Dummy(displaySize);

            // Draw the image at the correct offset within the display area
            ImVec2 imageTl(areaTopLeft.x + widgetOffset.x, areaTopLeft.y + widgetOffset.y);
            ImVec2 imageBr(imageTl.x + widgetSize.x, imageTl.y + widgetSize.y);
            ImGui::GetWindowDrawList()->AddImage(texture.TextureId, imageTl, imageBr, uv0, uv1);

            // Return mouse position relative to the display area top-left
            bool isHovered = ImGui::IsItemHovered();
            ImVec2 mouse = ImGui::GetMousePos();
            if (isHovered)
                return Point2d((double)(mouse.x - areaTopLeft.x), (double)(mouse.y - areaTopLeft.y));
            else
                return Point2d(-1., -1.);
        }

        void ShowImageInfo(const ImageBuffer &image, double zoomFactor)
        {
            std::string info = MatrixInfoUtils::_MatInfo(image);
            ImGui::Text("%s - Zoom:%.3lf", info.c_str(), zoomFactor);
        }


        void ShowPixelColorWidget(
            const ImageBuffer &image,
            Point pt,
            const ImageParams& params)
        {
            bool isInImage = Rect(Point(0, 0), image.size()).contains(pt);
            auto UCharToFloat = [](int v) { return (float)((float) v / 255.f); };

            bool done = false;
            std::string id = std::string("##pixelcolor_") + std::to_string(pt.x) + "," + std::to_string(pt.y);
            if (image.depth == ImageDepth::uint8)
            {
                ImGuiColorEditFlags editFlags =
                    ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_AlphaPreviewHalf
                    | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Uint8;
                if (!isInImage)
                {
                    float dummyColor[4]{0.f, 0.f, 0.f, 255.f};
                    ImGui::SetNextItemWidth(1.f);
                    int colorEditFlags =
                        ImGuiColorEditFlags_NoInputs
                        | ImGuiColorEditFlags_InputRGB
                        | ImGuiColorEditFlags_DisplayRGB;
                    ImGui::ColorEdit4(id.c_str(), dummyColor, colorEditFlags );
                    done = true;
                }
                else if (image.channels == 3)
                {
                    const uint8_t* pixel = image.ptr<uint8_t>(pt.y) + pt.x * 3;
                    bool isColorOrderBgr = IsUsingBgrColorOrder();
                    ImVec4 colorAsImVec = isColorOrderBgr ?
                        ImVec4(UCharToFloat(pixel[2]), UCharToFloat(pixel[1]), UCharToFloat(pixel[0]), UCharToFloat(255))
                        : ImVec4(UCharToFloat(pixel[0]), UCharToFloat(pixel[1]), UCharToFloat(pixel[2]), UCharToFloat(255));
                    ImGui::SetNextItemWidth(120.f * FontSizeRatio());
                    ImGui::ColorEdit3(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
                else if (image.channels == 4)
                {
                    const uint8_t* pixel = image.ptr<uint8_t>(pt.y) + pt.x * 4;
                    bool isColorOrderBgr = IsUsingBgrColorOrder();
                    ImVec4 colorAsImVec = isColorOrderBgr ?
                        ImVec4(UCharToFloat(pixel[2]), UCharToFloat(pixel[1]), UCharToFloat(pixel[0]), UCharToFloat(pixel[3]))
                        : ImVec4(UCharToFloat(pixel[0]), UCharToFloat(pixel[1]), UCharToFloat(pixel[2]), UCharToFloat(pixel[3]));
                    ImGui::SetNextItemWidth(150.f * FontSizeRatio());
                    ImGui::ColorEdit4(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
            }
            if (! done)
            {
                std::string pixelInfo = MatrixInfoUtils::MatPixelColorInfo(image, pt.x, pt.y);
                ImGui::Text("%s", pixelInfo.c_str());
            }
        }


        // If true, the collapsing headers will be synced across instances
        bool s_CollapsingHeader_CacheState_Sync = false;

        bool CollapsingHeader_OptionalCacheState(const char *name, bool forceOpen)
        {
            static std::map<std::string, bool> collapsingHeadersState;
            bool shallOpen = forceOpen;
            if (s_CollapsingHeader_CacheState_Sync)
            {
                if (collapsingHeadersState.find(name) != collapsingHeadersState.end())
                {
                    bool wasOpenedLastTime = collapsingHeadersState.at(name);
                    if (wasOpenedLastTime)
                        shallOpen = true;
                }
            }
            if (shallOpen)
                ImGui::SetNextItemOpen(shallOpen, ImGuiCond_Always);
            bool opened = ImGui::CollapsingHeader(name);
            collapsingHeadersState[name] = opened;
            return opened;
        };

    } // namespace ImageWidgets

}
