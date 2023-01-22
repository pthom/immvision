#pragma once
#include "imgui.h"
#include "immvision/image.h"
#include "immvision/internal/misc/insertion_order_map.h"
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
        bool IsEqual(const ColormapScaleFromStatsData& v1, const ColormapScaleFromStatsData& v2);
        bool CanColormap(const cv::Mat &image);
        ColormapSettingsData ComputeInitialColormapSettings(const cv::Mat& m);


        //
        // Colormaps images and textures
        //
        std::vector<std::string> AvailableColormaps();

        const insertion_order_map<std::string, ImTextureID>& ColormapsTextures();
        void ClearColormapsTexturesCache();

        //
        // Apply Colormap
        //
        cv::Mat_<cv::Vec4b> ApplyColormap(const cv::Mat& m, const ColormapSettingsData& settings);


        //
        // Interactive update during pan and zoom, full init on new Image
        //
        void UpdateRoiStatsInteractively(
            const cv::Mat &image,
            const cv::Rect& roi,
            ColormapSettingsData* inout_settings);
        void InitStatsOnNewImage(
            const cv::Mat &image,
            const cv::Rect& roi,
            ColormapSettingsData* inout_settings);

        //
        // GUI
        //
        void GuiShowColormapSettingsData(
            const cv::Mat &image,
            const cv::Rect& roi,
            float availableGuiWidth,
            ColormapSettingsData* inout_settings);

    } // namespace Colormap

} // namespace ImmVision