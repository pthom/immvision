#pragma once
#include "immvision/image.h"
#include <opencv2/core.hpp>

#include <map>
#include <vector>
#include <string>

// Uses https://github.com/jgreitemann/colormap
// Many thanks to Yuki Koyama

// Uses https://github.com/Neargye/magic_enum
// Many thanks to Daniil Goncharov


namespace ImmVision
{
    namespace Colormap
    {
        enum class GuiAction { None, Hovered, Apply, UnApply };

        struct ColormapGuiResult
        {
            std::string ColormapName = "";
            GuiAction Action = GuiAction::None;
        };

        ColormapGuiResult ShowColormapsGui(const std::string& selectedColormapName);

        std::vector<std::string> AvailableColormaps();

        const std::map<std::string, cv::Mat>& ColormapsImages();

        const std::map<std::string, unsigned int>& ColormapsTextures();

        void ClearColormapsTexturesCache();

        cv::Mat_<cv::Vec4b> ApplyColormap(
            const cv::Mat& m, const std::string& colormapName,
            double minValue, double maxValue
            );

        bool CanColormap(const cv::Mat &image);

    } // namespace Colormap

} // namespace ImmVision