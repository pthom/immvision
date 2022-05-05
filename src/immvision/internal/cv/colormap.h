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

        void ShowColormapSettingsDataGui(
            const cv::Mat &image, const cv::Rect& roi,
            ColormapSettingsData* inoutSettings);

        void UpdateColormapSettingsDataFromRoi(
            const cv::Mat &image,
            const cv::Rect& roi,
            bool forceRefresh,
            bool wasRoiChanged,
            ColormapSettingsData* inoutSettings);

        std::vector<std::string> AvailableColormaps();

        const std::map<std::string, cv::Mat>& ColormapsImages();

        const std::map<std::string, unsigned int>& ColormapsTextures();

        void ClearColormapsTexturesCache();

        cv::Mat_<cv::Vec4b> ApplyColormap(
            const cv::Mat& m, const std::string& colormapName,
            double minValue, double maxValue
            );

        bool CanColormap(const cv::Mat &image);


        bool IsNone(const ColormapSettingsData& a);
        bool IsEqual(const ColormapSettingsData& v1, const ColormapSettingsData& v2);

        ColormapSettingsData ComputeInitialColormapSettings(const cv::Mat& m);

    } // namespace Colormap

} // namespace ImmVision