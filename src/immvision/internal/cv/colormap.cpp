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
#include <iostream>

namespace ImmVision
{
    namespace Colormap
    {

        using ColormapType = tinycolormap::ColormapType;


        std::string LowerString(const std::string& s)
        {
            // Just one implementation among billions of others around the planet
            std::string r = s;
            std::transform(r.begin(), r.end(), r.begin(), ::tolower);
            return r;
        }


        std::vector<std::string> AvailableColormaps()
        {
            // Hack, to let g++-11.2 accept to compile magic_enum (see https://github.com/Neargye/magic_enum/issues/184)
            constexpr std::size_t WORKAROUND_MAGIC_ENUM = magic_enum::enum_count<tinycolormap::ColormapType>();
            (void)WORKAROUND_MAGIC_ENUM;

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


        const std::map<std::string, cv::Mat>& ColormapsImages()
        {
            static std::map<std::string, cv::Mat> cache;
            if (cache.empty())
            {
                magic_enum::enum_for_each<ColormapType>([] (auto val) {
                    ColormapType type = val;
                    const char* name = magic_enum::enum_name(type).data();
                    cache[name] = MakeColormapImage(type);
                });
            }
            return cache;
        }


        static std::map<std::string, std::unique_ptr<GlTextureCv>> sColormapsTexturesCache;


        void FillTextureCache()
        {
            if (sColormapsTexturesCache.empty())
            {
                auto images = ColormapsImages();
                for (const auto& kv: images)
                {
                    auto texture = std::make_unique<GlTextureCv>(kv.second, true);
                    sColormapsTexturesCache[kv.first] = std::move(texture);
                }
            }
        }


        const std::map<std::string, unsigned int>& ColormapsTextures()
        {
            FillTextureCache();

            static std::map<std::string, unsigned int> cache;
            if (cache.empty())
            {
                for (const auto& kv: sColormapsTexturesCache)
                    cache[kv.first] = sColormapsTexturesCache.at(kv.first)->mImTextureId;
            }
            return cache;
        }


        void ClearColormapsTexturesCache()
        {
            sColormapsTexturesCache.clear();
        }


        void GuiChooseColormap(const cv::Mat &image, const cv::Rect& roi,
                               ColormapSettingsData* params)
        {
            static std::string lastUnselectedColormap = "";
            FillTextureCache();

            params->internal_ColormapHovered = "";
            for (const auto& kv: sColormapsTexturesCache)
            {
                std::string colormapName = kv.first;
                bool isSelected = (colormapName == params->Colormap);

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
                if (isSelected)
                    color = colorSelected;
                else if (isHovered)
                    color = colorHovered;
                else
                    color = colorNormal;

                auto pos = ImGui::GetCursorPos();
                ImGui::TextColored(color, "%s", colormapName.c_str());
                pos.x += widthText;
                ImGui::SetCursorPos(pos);
                if (isSelected)
                    kv.second->DrawButton(sizeTexture);
                else
                    kv.second->Draw(sizeTexture);
                if (ImGui::IsItemHovered() && (colormapName != lastUnselectedColormap))
                    params->internal_ColormapHovered = colormapName;
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                {
                    if (isSelected)
                    {
                        params->Colormap = "";
                        lastUnselectedColormap = colormapName;
                    }
                    else
                    {
                        params->Colormap = colormapName;
                        lastUnselectedColormap = "";
                    }
                }
            }
        }


        size_t ScaleTypeToInt(const std::string& type)
        {
            std::vector<std::string> allTypes =  AvailableColormapScaleTypes();
            for (size_t i = 0; i < allTypes.size(); ++i)
                if (LowerString(type) == LowerString(allTypes[i]))
                    return i;
            throw std::runtime_error("Bad Value for ColormapScaleType: " + type);
        }

        std::string IntToScaleType(size_t type)
        {
            std::vector<std::string> allTypes =  AvailableColormapScaleTypes();
            if (type > allTypes.size())
                throw std::runtime_error("Bad Value for ColormapScaleType: " + std::to_string(type));
            return allTypes[type];
        };


        void ShowColormapSettingsDataGui(
            const cv::Mat &image, const cv::Rect& roi,
            ColormapSettingsData* inoutSettings)
        {
            {
                ImGui::Text("Colormap scale type");
                int colormapScaleType = ScaleTypeToInt(inoutSettings->ColormapScaleType);
                std::vector<std::string> allTypes =  AvailableColormapScaleTypes();
                for (size_t i = 0; i < allTypes.size(); ++i)
                {
                    ImGui::RadioButton(allTypes[i].c_str(), &colormapScaleType, i);
                    ImGui::SameLine();
                }
                ImGui::NewLine();
                inoutSettings->ColormapScaleType = IntToScaleType(colormapScaleType);
            }

            ImGuiImm::SliderAnyFloatLogarithmic("Scale min", &inoutSettings->ColormapScaleMin, -255., 255.);
            ImGuiImm::SliderAnyFloatLogarithmic("Scale max", &inoutSettings->ColormapScaleMax, -255., 255.);

            GuiChooseColormap(image, roi, inoutSettings);
        }


        void UpdateColormapSettingsDataFromRoi(
            const cv::Mat &m,
            const cv::Rect& roi,
            bool forceRefresh,
            bool wasRoiChanged,
            ColormapSettingsData* params)
        {
            if (!CanColormap(m))
                return;
            if (LowerString(params->ColormapScaleType) == "manual")
                return;
            if (LowerString(params->ColormapScaleType) == "0,1")
            {
                params->ColormapScaleMin = 0.;
                params->ColormapScaleMax = 1.;
                return;
            }
            if (LowerString(params->ColormapScaleType) == "-1,1")
            {
                params->ColormapScaleMin = -1.;
                params->ColormapScaleMax = 1.;
                return;
            }

            bool needed = false;
            if (LowerString(params->ColormapScaleType) == "roi")
                needed = wasRoiChanged;
            if (forceRefresh)
                needed = true;

            if (needed)
            {
                double min, max;
                if (LowerString(params->ColormapScaleType) == "roi")
                    cv::minMaxLoc(m(roi), &min, &max);
                else
                    cv::minMaxLoc(m, &min, &max);
                params->ColormapScaleMin = min;
                params->ColormapScaleMax = max;
            }

        }


        template<typename _Tp>
        cv::Mat_<cv::Vec4b> _ApplyColormap(const cv::Mat &m, const std::string &colormapName,
                                           double minValue, double maxValue)
        {
            auto _colormapType = magic_enum::enum_cast<ColormapType>(colormapName);
            assert(_colormapType.has_value());
            auto colormapType = _colormapType.value();

            std::array<cv::Vec4b, 256> colorLut;
            for (size_t i = 0; i < 256; ++i)
            {
                double x = (double) i / 255.;
                 auto c = tinycolormap::GetColor(x, colormapType);
                colorLut[i] = { c.ri(), c.gi(), c.bi(), 255 };
            }

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


        cv::Mat_<cv::Vec4b> ApplyColormap(const cv::Mat &m, const std::string &colormapName, double minValue, double maxValue)
        {
            assert(CanColormap(m));

#ifdef CV_16F
            if (m.depth() == CV_16F)
                return _ApplyColormap<cv::float16_t>(m, colormapName, minValue, maxValue);
#endif
            if (m.depth() == CV_32F)
                return _ApplyColormap<float>(m, colormapName, minValue, maxValue);
            else if (m.depth() == CV_64F)
                return _ApplyColormap<double>(m, colormapName, minValue, maxValue);
            else
            {
                assert(false);
                throw std::runtime_error("ApplyColormap: bad depth");
            }
        }


        bool CanColormap(const cv::Mat &image)
        {
            return image.channels() == 1;
        }


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
            if (v1.ColormapScaleType != v2.ColormapScaleType)
                return false;
            if (v1.internal_ColormapHovered != v2.internal_ColormapHovered)
                return false;
            return true;
        }


        ColormapSettingsData ComputeInitialColormapSettings(const cv::Mat& m)
        {
            ColormapSettingsData r;
            return r;
        }


    } // namespace Colormap
} // namespace ImmVision
