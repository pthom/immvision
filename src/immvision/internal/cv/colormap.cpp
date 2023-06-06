#define IMGUI_DEFINE_MATH_OPERATORS
#include "immvision/internal/cv/colormap.h"

#include "immvision/internal/misc/tinycolormap.hpp"
#include "immvision/internal/misc/magic_enum.hpp"
#include "immvision/internal/misc/math_utils.h"
#include "immvision/internal/gl/gl_texture.h"
#include "immvision/imgui_imm.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <opencv2/core.hpp>
#include <array>
#include <optional>

namespace ImmVision
{
    namespace Colormap
    {
        //
        // Base operations for ColormapSettingsData
        //
        bool IsNone(const ColormapSettingsData& a)
        {
            ColormapSettingsData empty;
            return IsEqual(a, empty);
        }


        bool IsEqual(const ColormapSettingsData& v1, const ColormapSettingsData& v2)
        {
            if (v1.Colormap != v2.Colormap)
                return false;
            if (fabs(v1.ColormapScaleMax - v2.ColormapScaleMax) > 1E-6)
                return false;
            if (fabs(v1.ColormapScaleMin - v2.ColormapScaleMin) > 1E-6)
                return false;
            if (v1.internal_ColormapHovered != v2.internal_ColormapHovered)
                return false;
            if (!IsEqual(v1.ColormapScaleFromStats, v2.ColormapScaleFromStats))
                return false;
            return true;
        }


        bool IsEqual(const ColormapScaleFromStatsData& v1, const ColormapScaleFromStatsData& v2)
        {
            if (v1.ColorMapStatsType != v2.ColorMapStatsType)
                return false;
            if (fabs(v1.NbSigmas - v2.NbSigmas) > 1E-6)
                return false;
            if (v1.UseStatsMin != v2.UseStatsMin)
                return false;
            if (v1.UseStatsMax != v2.UseStatsMax)
                return false;
            return true;
        }


        bool CanColormap(const cv::Mat &image)
        {
            return ((image.type() == CV_32FC1) || (image.type() == CV_64FC1));
        }


        ColormapSettingsData ComputeInitialColormapSettings(const cv::Mat& m)
        {
            (void)m;
            ColormapSettingsData r;
            return r;
        }


        //
        // Colormaps images and textures
        //


        using ColormapType = tinycolormap::ColormapType;


        std::vector<std::string> AvailableColormaps()
        {
            std::vector<std::string> r;
            magic_enum::enum_for_each<ColormapType>([&r] (auto val) {
                ColormapType type = val;
                const char* name = magic_enum::enum_name(type).data();
                r.push_back(name);
            });
            return r;
        }


        cv::Mat MakeColormapImage(tinycolormap::ColormapType colorType)
        {
            int w = 256, h = 15;
            cv::Mat_<cv::Vec3b> m(cv::Size(w, h));
            for (int x = 0; x < w; ++x)
            {
                double k = MathUtils::UnLerp(0., (double)w, (double)x);
                auto col = tinycolormap::GetColor(k, colorType);
                for (int y = 0; y < h; ++y)
                    m(y, x) = cv::Vec3b( col.bi(), col.gi(), col.ri() );
            }

            return std::move(m);
        }


        const insertion_order_map<std::string, cv::Mat>& _ColormapsImages()
        {
            static insertion_order_map<std::string, cv::Mat> cache;
            if (cache.empty())
            {
                magic_enum::enum_for_each<ColormapType>([] (auto val) {
                    ColormapType type = val;
                    const char* name = magic_enum::enum_name(type).data();
                    cache.insert(name, MakeColormapImage(type));
                });
            }
            return cache;
        }


        static insertion_order_map<std::string, std::unique_ptr<GlTextureCv>> sColormapsTexturesCache;


        void FillTextureCache()
        {
            if (sColormapsTexturesCache.empty())
            {
                auto images = _ColormapsImages();
                for (const auto& k: images.insertion_order_keys())
                {
                    cv::Mat& m = images.get(k);
                    auto texture = std::make_unique<GlTextureCv>(m, true);
                    sColormapsTexturesCache.insert(k, std::move(texture));
                }
            }
        }


        const insertion_order_map<std::string, ImTextureID>& ColormapsTextures()
        {
            FillTextureCache();

            static insertion_order_map<std::string, ImTextureID> cache;
            if (cache.empty())
            {
                for (const auto& k: sColormapsTexturesCache.insertion_order_keys())
                    cache.insert(k, sColormapsTexturesCache.get(k)->mImTextureId);
            }
            return cache;
        }


        void ClearColormapsTexturesCache()
        {
            sColormapsTexturesCache.clear();
        }



        //
        // Apply Colormap
        //


        template<typename _Tp>
        cv::Mat_<cv::Vec4b> _ApplyColormap(const cv::Mat &m, const ColormapSettingsData& settings)
        {
            assert(CanColormap(m));

            std::string colormapName = settings.internal_ColormapHovered.empty() ? settings.Colormap : settings.internal_ColormapHovered;

            auto _colormapType = magic_enum::enum_cast<ColormapType>(colormapName);
            if (!_colormapType.has_value())
            {
                fprintf(stderr, "_ApplyColormap: bad colormap name: %s\n", settings.Colormap.c_str());
                assert(false);
            }
            auto colormapType = _colormapType.value();

            std::array<cv::Vec4b, 256> colorLut;
            for (size_t i = 0; i < 256; ++i)
            {
                double x = (double) i / 255.;
                 auto c = tinycolormap::GetColor(x, colormapType);
                colorLut[i] = { c.ri(), c.gi(), c.bi(), 255 };
            }

            double minValue = settings.ColormapScaleMin;
            double maxValue = settings.ColormapScaleMax;
            auto fnGetColor = [&](_Tp value) -> cv::Vec4b
            {
                double k = (value - minValue) / (maxValue - minValue);
                k = std::clamp(k, 0., 1.);
                size_t idx = (size_t)(k * 255.);
                return colorLut[idx];
            };

            cv::Mat_<cv::Vec4b> rgba(m.size());
            for (int y = 0; y < m.rows; ++y)
            {
                cv::Vec4b *dst = &rgba(y, 0);
                const _Tp* src = &m.at<_Tp>(y, 0);
                for (int x = 0; x < m.cols; ++x)
                {
                    *dst = fnGetColor(*src);
                    ++dst;
                    ++src;
                }
            }
            return rgba;
        }


        cv::Mat_<cv::Vec4b> ApplyColormap(const cv::Mat &m, const ColormapSettingsData& settings)
        {
            if (m.depth() == CV_8U)
                return _ApplyColormap<uchar>(m, settings);
            else if (m.depth() == CV_8S)
                return _ApplyColormap<char>(m, settings);
            else if (m.depth() == CV_16U)
                return _ApplyColormap<uint16_t>(m, settings);
            else if (m.depth() == CV_16S)
                return _ApplyColormap<int16_t>(m, settings);
            else if (m.depth() == CV_32S)
                return _ApplyColormap<int32_t>(m, settings);
            if (m.depth() == CV_32F)
                return _ApplyColormap<float>(m, settings);
            else if (m.depth() == CV_64F)
                return _ApplyColormap<double>(m, settings);
#ifdef CV_16F
            else if (m.depth() == CV_16F)
                return _ApplyColormap<cv::float16_t>(m, settings);
#endif
            else
            {
                assert(false);
                throw std::runtime_error("ApplyColormap: bad depth");
            }
        }


        //
        // Interactive update during pan and zoom
        //
        struct ImageStats
        {
            double mean, stdev;
            double min, max;
        };

        ImageStats FillImageStats(const cv::Mat& m)
        {
            assert(m.channels() == 1);
            ImageStats r;
            cv::minMaxLoc(m, &r.min, &r.max);
            cv::Scalar mean, deviation;
            cv::meanStdDev(m, mean, deviation);
            r.mean = mean[0];
            r.stdev = deviation[0];
            return r;
        }




        void ApplyColormapStatsToMinMax(const cv::Mat& m, std::optional<cv::Rect> roi, ColormapSettingsData* inout_settings)
        {
            bool isRoi = roi.has_value();

            ImageStats imageStats;
            if (isRoi)
                imageStats = FillImageStats(m(roi.value()));
            else
                imageStats = FillImageStats(m);

            if (inout_settings->ColormapScaleFromStats.UseStatsMin)
                inout_settings->ColormapScaleMin = imageStats.min;
            else
                inout_settings->ColormapScaleMin =
                    imageStats.mean - (double) inout_settings->ColormapScaleFromStats.NbSigmas * imageStats.stdev;

            if (inout_settings->ColormapScaleFromStats.UseStatsMax)
                inout_settings->ColormapScaleMax = imageStats.max;
            else
                inout_settings->ColormapScaleMax =
                    imageStats.mean + (double) inout_settings->ColormapScaleFromStats.NbSigmas * imageStats.stdev;
        }


        void UpdateRoiStatsInteractively(
            const cv::Mat &image,
            const cv::Rect& roi,
            ColormapSettingsData* inout_settings)
        {
            if (image.channels() != 1)
                return;

            if(roi.empty())
                return;

            if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI)
                ApplyColormapStatsToMinMax(image, roi, inout_settings);
        }


        void InitStatsOnNewImage(
            const cv::Mat &image,
            const cv::Rect& roi,
            ColormapSettingsData* inout_settings)
        {
            if (image.channels() != 1)
                return;

            if (roi.empty())
                return;

            if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI)
                ApplyColormapStatsToMinMax(image, roi, inout_settings);
            else if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromFullImage)
                ApplyColormapStatsToMinMax(image, std::nullopt, inout_settings);
        }


        //
        // GUI
        //
        void GuiChooseColormap(ColormapSettingsData* inout_params)
        {
            static std::optional<std::string> lastUnselectedColormap;
            FillTextureCache();

            inout_params->internal_ColormapHovered = "";
            for (const auto& kv: sColormapsTexturesCache.items())
            {
                std::string colormapName = kv.first;
                bool wasSelected = (colormapName == inout_params->Colormap);

                ImVec4 colorNormal(0.7f, 0.7f, 0.7f, 1.f);
                ImVec4 colorSelected(1.f, 1.f, 0.2f, 1.f);
                ImVec4 colorHovered = colorSelected;
                colorHovered.w = 0.65f;

                float kFont = ImGui::GetFontSize();
                float widthText = kFont * 5.5f;
                ImVec2 sizeTexture(kFont * 12.f, kFont * 0.8f);

                bool isHovered;
                {
                    auto posWidget = ImGui::GetCursorScreenPos();
                    auto posMouse = ImGui::GetMousePos();
                    ImRect bounding(posWidget, posWidget + ImVec2(sizeTexture.x + widthText, 15.f));
                    isHovered = bounding.Contains(posMouse);
                }

                ImVec4 color;
                if (wasSelected)
                    color = colorSelected;
                else if (isHovered)
                    color = colorHovered;
                else
                    color = colorNormal;

                auto pos = ImGui::GetCursorPos();
                ImGui::TextColored(color, "%s", colormapName.c_str());
                pos.x += widthText;
                ImGui::SetCursorPos(pos);
                if (wasSelected)
                    kv.second->DrawButton(sizeTexture);
                else
                kv.second->Draw(sizeTexture);
                if (ImGui::IsItemHovered())
                {
                    if (!lastUnselectedColormap.has_value())
                        inout_params->internal_ColormapHovered = colormapName;
                    if (lastUnselectedColormap.has_value() && (*lastUnselectedColormap != colormapName))
                        inout_params->internal_ColormapHovered = colormapName;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                {
                    if (wasSelected)
                    {
                        inout_params->Colormap = "None";
                        lastUnselectedColormap = colormapName;
                    }
                    else
                    {
                        inout_params->Colormap = colormapName;
                        lastUnselectedColormap = std::nullopt;
                    }
                }
            }
        }


        void DrawColorTabsSubtitles(const std::string &title, float availableGuiWidth)
        {
            ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            ImVec4 backColor = ImGui::GetStyleColorVec4(ImGuiCol_TabActive);
            backColor.w = 0.3f;

            // background rect
            {
                ImVec2 tl = ImGui::GetCursorScreenPos();
                ImVec2 br = tl;
                br.x += availableGuiWidth - 10.f;
                br.y += ImGui::GetFontSize() + 2.f;
                ImU32 col = ImGui::GetColorU32(backColor);
                float rounding = 4.f;
                ImGui::GetWindowDrawList()->AddRectFilled(tl, br, col, rounding);
            }
            std::string fullTitle = std::string("          Colormap Scale ") + title;

            ImGui::TextColored(textColor, "%s", fullTitle.c_str());
        }


        void GuiImageStats(const cv::Mat& m, std::optional<cv::Rect> roi, ColormapSettingsData* inout_settings, float availableGuiWidth)
        {
            float em = ImGui::GetFontSize();

            ImageStats imageStats;
            bool isRoi = roi.has_value();
            if (isRoi)
            {
                imageStats = FillImageStats(m(roi.value()));
                ImGui::PushID("ROI");
            }
            else
            {
                imageStats = FillImageStats(m);
                ImGui::PushID("Full");
            }

            ImGui::Text("Stats:");
            ImGui::Text("min=%.3lf max=%.3lf", imageStats.min, imageStats.max);
            ImGui::Text("mean=%3lf stdev=%3lf", imageStats.mean, imageStats.stdev);

            bool changed = false;

            {
                std::string help = "The number of sigmas will impact\n"
                                   "how the colormap scale min and max are calculated from\n"
                                   "the mean and standard deviation";
                changed |= ImGuiImm::SliderAnyFloat(
                    "Nb of sigmas",
                    &inout_settings->ColormapScaleFromStats.NbSigmas,
                    0., 8.,
                    em * 10.f);
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", help.c_str());
            }

            {
                std::string help = "If \"Use stats min\" is checked, then the scale\n"
                                   "will be calculated from the image minimum value \n"
                                   "instead of a sigma based value\n"
                                   "\n"
                                   "(same for max values)";
                changed |= ImGui::Checkbox("Use stats min", &inout_settings->ColormapScaleFromStats.UseStatsMin);
                ImGui::SameLine();
                changed |= ImGui::Checkbox("Use stats max", &inout_settings->ColormapScaleFromStats.UseStatsMax);
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", help.c_str());
            }

            if (changed)
                ApplyColormapStatsToMinMax(m, roi, inout_settings);

            {
                ImGui::Text("Colormap scale");
                ImGuiImm::SliderAnyFloatLogarithmic("Min", &inout_settings->ColormapScaleMin, -255., 255.,
                                                    em * 6.f);
                ImGui::SameLine();
                ImGuiImm::SliderAnyFloatLogarithmic("Max", &inout_settings->ColormapScaleMax, -255., 255.,
                                                    em * 6.f);
            }

            ImGui::PopID();
        }


        void GuiShowColormapSettingsData(
            const cv::Mat &image,
            const cv::Rect& roi,
            float availableGuiWidth,
            ColormapSettingsData* inout_settings
            )
        {
            GuiChooseColormap(inout_settings);

            {
                ImGui::NewLine();
                ImGui::Text("Colormap stats ROI:");
                if (ImGui::RadioButton(
                    "full image",
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromFullImage))
                {
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType = ColorMapStatsTypeId::FromFullImage;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton(
                    "visible ROI",
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI))
                {
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType = ColorMapStatsTypeId::FromVisibleROI;
                }
                std::string helpRoi = "Warning, if using \"number of sigmas\" on a ROI\n"
                                      "the colormap scale will vary immediately\n"
                                      "whenever you zoom in/out or pan";
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", helpRoi.c_str());
            }

            std::optional<cv::Rect> optionalRoi;
            if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI)
                optionalRoi = roi;
            GuiImageStats(image, optionalRoi, inout_settings, availableGuiWidth);

            // ImGuiImm::SeparatorFixedWidth(availableGuiWidth);

        }


    } // namespace Colormap
} // namespace ImmVision
