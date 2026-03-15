#pragma once
#include "imgui.h"
#include "immvision/image.h"
#include "immvision/immvision_types.h"
#include "immvision/internal/misc/insertion_order_map.h"

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
        bool CanColormap(const ImageBuffer &image);
        ColormapSettingsData ComputeInitialColormapSettings(const ImageBuffer& m);


        //
        // Colormaps images and textures
        //
        std::vector<std::string> AvailableColormaps();

        const insertion_order_map<std::string, ImTextureID>& ColormapsTextures();
        void ClearColormapsTexturesCache();

        //
        // Apply Colormap
        //
        ImageBuffer ApplyColormap(const ImageBuffer& m, const ColormapSettingsData& settings);


        //
        // Interactive update during pan and zoom, full init on new Image
        //
        void UpdateRoiStatsInteractively(
            const ImageBuffer &image,
            const Rect& roi,
            ColormapSettingsData* inout_settings);
        void InitStatsOnNewImage(
            const ImageBuffer &image,
            const Rect& roi,
            ColormapSettingsData* inout_settings);

        //
        // GUI
        //
        void GuiShowColormapSettingsData(
            const ImageBuffer &image,
            const Rect& roi,
            float availableGuiWidth,
            ColormapSettingsData* inout_settings);

    } // namespace Colormap

} // namespace ImmVision
