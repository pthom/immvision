#include "immvision/internal/drawing/colormap.h"

#include "immvision/internal/misc/tinycolormap.hpp"
#include "immvision/internal/misc/magic_enum.hpp"
#include "immvision/internal/misc/math_utils.h"
#include "immvision/internal/gl/gl_texture.h"
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


        // Hack, to let g++-11.2 accept to compile magic_enum (see https://github.com/Neargye/magic_enum/issues/184)
        constexpr std::size_t WORKAROUND_MAGIC_ENUM = magic_enum::enum_count<tinycolormap::ColormapType>();


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


        ColormapGuiResult ShowColormapsGui(const std::string& selectedColormapName)
        {
            static std::string lastUnselectedColormap = "";


            ColormapGuiResult r;
            FillTextureCache();

            for (const auto& kv: sColormapsTexturesCache)
            {
                std::string colormapName = kv.first;
                bool isSelected = (colormapName == selectedColormapName);

                ImVec4 colorNormal(0.7f, 0.7f, 0.7f, 1.f);
                ImVec4 colorSelected(1.f, 1.f, 0.2f, 1.f);
                ImVec4 colorHovered = colorSelected; colorHovered.w = 0.65;

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
                if (ImGui::IsItemHovered())
                {
                    r.ColormapName = colormapName;
                    if ((colormapName != selectedColormapName) && (colormapName != lastUnselectedColormap) )
                        r.Action = GuiAction::Hovered;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                {
                    if (isSelected)
                    {
                        r.Action = GuiAction::UnApply;
                        lastUnselectedColormap = colormapName;
                    }
                    else
                    {
                        r.Action = GuiAction::Apply;
                        lastUnselectedColormap = "";
                    }
                }
            }
            return r;
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
            bool canColormap = true;
            if (image.channels() != 1)
                canColormap = false;
            bool isDepthOk = false;
            if ((image.depth() == CV_32F) || (image.depth() == CV_64F))
                isDepthOk = true;
#ifdef CV_16F
            if (image.depth() == CV_16F)
                isDepthOk = true;
#endif
            if (!isDepthOk)
                canColormap = false;

            return canColormap;
        }


    } // namespace Colormap
} // namespace ImmVision
