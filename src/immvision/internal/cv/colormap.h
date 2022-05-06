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
        //
        // Base operations for ColormapScaleSettings
        //
        bool IsNone(const ColormapSettingsData& a);
        bool IsEqual(const ColormapSettingsData& v1, const ColormapSettingsData& v2);
        bool CanColormap(const cv::Mat &image);
        ColormapSettingsData ComputeInitialColormapSettings(const cv::Mat& m);


        //
        // Colormaps images and textures
        //
        std::vector<std::string> AvailableColormaps();
        const std::map<std::string, cv::Mat>& ColormapsImages();
        const std::map<std::string, unsigned int>& ColormapsTextures();
        void ClearColormapsTexturesCache();

        //
        // Apply Colormap
        //

        cv::Mat_<cv::Vec4b> ApplyColormap(const cv::Mat& m, const ColormapSettingsData& settings);


        //
        // GUI
        //
        void GuiShowColormapSettingsData(
            const cv::Mat &image, const cv::Rect& roi,
            ColormapSettingsData* inout_settings);

    } // namespace Colormap

} // namespace ImmVision