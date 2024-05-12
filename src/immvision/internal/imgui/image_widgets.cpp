#include "immvision/internal/imgui/image_widgets.h"
#include "immvision/internal/cv/matrix_info_utils.h"

#include <map>

namespace ImmVision
{
    namespace ImageWidgets
    {
        float FontSizeRatio()
        {
            float r = ImGui::GetFontSize() / 14.5;
            return r;
        }

        cv::Point2d DisplayTexture_TrackMouse(const GlTextureCv& texture, ImVec2 displaySize, bool disableDragWindow)
        {
            ImVec2 imageTopLeft = ImGui::GetCursorScreenPos();
            texture.Draw_DisableDragWindow(displaySize, disableDragWindow);
            bool isImageHovered = ImGui::IsItemHovered();
            ImVec2 mouse = ImGui::GetMousePos();
            if (isImageHovered)
                return cv::Point2d((double)(mouse.x - imageTopLeft.x), (double)(mouse.y - imageTopLeft.y));
            else
                return cv::Point2d(-1., -1.);
        }

        void ShowImageInfo(const cv::Mat &image, double zoomFactor)
        {
            std::string info = MatrixInfoUtils::_MatInfo(image);
            ImGui::Text("%s - Zoom:%.3lf", info.c_str(), zoomFactor);
        }


        void ShowPixelColorWidget(
            const cv::Mat &image,
            cv::Point pt,
            const ImageParams& params)
        {
            bool isInImage = cv::Rect(cv::Point(0, 0), image.size()).contains((pt));
            auto UCharToFloat = [](int v) { return (float)((float) v / 255.f); };
            auto Vec3bToImVec4 = [&UCharToFloat, &params](cv::Vec3b v) {
                return params.IsColorOrderBGR ?
                       ImVec4(UCharToFloat(v[2]), UCharToFloat(v[1]), UCharToFloat(v[0]), UCharToFloat(255))
                                              :   ImVec4(UCharToFloat(v[0]), UCharToFloat(v[1]), UCharToFloat(v[2]), UCharToFloat(255));
            };
            auto Vec4bToImVec4 = [&UCharToFloat, &params](cv::Vec4b v) {
                return params.IsColorOrderBGR ?
                       ImVec4(UCharToFloat(v[2]), UCharToFloat(v[1]), UCharToFloat(v[0]), UCharToFloat(v[3]))
                                              :    ImVec4(UCharToFloat(v[0]), UCharToFloat(v[1]), UCharToFloat(v[2]), UCharToFloat(v[3]));
            };

            bool done = false;
            std::string id = std::string("##pixelcolor_") + std::to_string(pt.x) + "," + std::to_string(pt.y);
            if (image.depth() == CV_8U)
            {
                ImGuiColorEditFlags editFlags =
                    ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_AlphaPreviewHalf
                    | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Uint8;
                if (!isInImage)
                {
                    // ColorEdit4 introduces a strange line spacing on the next group
                    // which cannot be simulated with ImGui::Dummy
                    // => we add a dummy one (hopefully black on a black background)
                    float dummyColor[4]{0.f, 0.f, 0.f, 255.f};
                    ImGui::SetNextItemWidth(1.f);
                    int colorEditFlags =
                        ImGuiColorEditFlags_NoInputs
                        | ImGuiColorEditFlags_InputRGB
                        | ImGuiColorEditFlags_DisplayRGB;
                    ImGui::ColorEdit4(id.c_str(), dummyColor, colorEditFlags );
                    done = true;
                }
                else if (image.channels() == 3)
                {
                    cv::Vec3b col = image.at<cv::Vec3b>(pt.y, pt.x);
                    ImVec4 colorAsImVec = Vec3bToImVec4(col);
                    ImGui::SetNextItemWidth(150.f * FontSizeRatio());
                    ImGui::ColorEdit3(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
                else if (image.channels() == 4)
                {
                    cv::Vec4b col = image.at<cv::Vec4b>(pt.y, pt.x);
                    ImVec4 colorAsImVec = Vec4bToImVec4(col);
                    ImGui::SetNextItemWidth(200.f * FontSizeRatio());
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