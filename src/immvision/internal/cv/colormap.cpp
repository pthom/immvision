#include "immvision/internal/cv/colormap.h"

#include "immvision/internal/misc/tinycolormap.hpp"
#include "immvision/internal/misc/magic_enum.hpp"
#include "immvision/internal/misc/math_utils.h"
#include "immvision/internal/gl/gl_texture.h"
#include "immvision/imgui_imm.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
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
            return true;
        }


        bool CanColormap(const cv::Mat &image)
        {
            return image.channels() == 1;
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


        const insertion_order_map<std::string, cv::Mat>& ColormapsImages()
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
                auto images = ColormapsImages();
                for (const auto& k: images.insertion_order_keys())
                {
                    cv::Mat& m = images.get(k);
                    auto texture = std::make_unique<GlTextureCv>(m, true);
                    sColormapsTexturesCache.insert(k, std::move(texture));
                }
            }
        }


        const insertion_order_map<std::string, unsigned int>& ColormapsTextures()
        {
            FillTextureCache();

            static insertion_order_map<std::string, unsigned int> cache;
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

                float widthText = 75.f;
                ImVec2 sizeTexture(200.f, 15.f);

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


        void GuiShowColormapSettingsData(
            const cv::Mat &image,
            const cv::Rect& roi,
            ColormapSettingsData* inout_settings
            )
        {
            (void)image;(void)roi;
            ImGuiImm::SliderAnyFloatLogarithmic("Scale min", &inout_settings->ColormapScaleMin, -255., 255.);
            ImGuiImm::SliderAnyFloatLogarithmic("Scale max", &inout_settings->ColormapScaleMax, -255., 255.);
            GuiChooseColormap(inout_settings);
        }


    } // namespace Colormap
} // namespace ImmVision