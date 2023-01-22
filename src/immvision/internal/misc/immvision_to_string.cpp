#include "immvision/internal/misc/immvision_to_string.h"
#include "immvision/internal/misc/string_utils.h"

namespace ImmVision
{
    std::string ToString(ColorMapStatsTypeId id)
    {
        if (id == ColorMapStatsTypeId::FromFullImage)
            return "FromFullImage";
        else
            return "FromVisibleROI";
    }

    std::string ToString(const ColormapScaleFromStatsData& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "ColormapScaleFromStatsData\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "ColorMapStatsType: " + ToString(v.ColorMapStatsType) + "\n";
        inner = inner + "NbSigmas: " + ToString(v.NbSigmas) + "\n";
        inner = inner + "UseStatsMin: " + ToString(v.UseStatsMin) + "\n";
        inner = inner + "UseStatsMax: " + ToString(v.UseStatsMax) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }
    
    std::string ToString(const ColormapSettingsData& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "ColormapSettingsData\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "Colormap: " + ToString(v.Colormap) + "\n";
        inner = inner + "ColormapScaleMin: " + ToString(v.ColormapScaleMin) + "\n";
        inner = inner + "ColormapScaleMax: " + ToString(v.ColormapScaleMax) + "\n";
        inner = inner + "ColormapScaleFromStats: " + ToString(v.ColormapScaleFromStats) + "\n";
        inner = inner + "internal_ColormapHovered: " + ToString(v.internal_ColormapHovered) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }
    
    std::string ToString(const MouseInformation& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "MouseInformation\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "IsMouseHovering: " + ToString(v.IsMouseHovering) + "\n";
        inner = inner + "MousePosition: " + ToString(v.MousePosition) + "\n";
        inner = inner + "MousePosition_Displayed: " + ToString(v.MousePosition_Displayed) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }
    
    std::string ToString(const ImageParams& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "ImageParams\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "RefreshImage: " + ToString(v.RefreshImage) + "\n";
        inner = inner + "ImageDisplaySize: " + ToString(v.ImageDisplaySize) + "\n";
        inner = inner + "ZoomPanMatrix: " + ToString(v.ZoomPanMatrix) + "\n";
        inner = inner + "ZoomKey: " + ToString(v.ZoomKey) + "\n";
        inner = inner + "ColormapSettings: " + ToString(v.ColormapSettings) + "\n";
        inner = inner + "ColormapKey: " + ToString(v.ColormapKey) + "\n";
        inner = inner + "PanWithMouse: " + ToString(v.PanWithMouse) + "\n";
        inner = inner + "ZoomWithMouseWheel: " + ToString(v.ZoomWithMouseWheel) + "\n";
        inner = inner + "IsColorOrderBGR: " + ToString(v.IsColorOrderBGR) + "\n";
        inner = inner + "SelectedChannel: " + ToString(v.SelectedChannel) + "\n";
        inner = inner + "ShowSchoolPaperBackground: " + ToString(v.ShowSchoolPaperBackground) + "\n";
        inner = inner + "ShowAlphaChannelCheckerboard: " + ToString(v.ShowAlphaChannelCheckerboard) + "\n";
        inner = inner + "ShowGrid: " + ToString(v.ShowGrid) + "\n";
        inner = inner + "DrawValuesOnZoomedPixels: " + ToString(v.DrawValuesOnZoomedPixels) + "\n";
        inner = inner + "ShowImageInfo: " + ToString(v.ShowImageInfo) + "\n";
        inner = inner + "ShowPixelInfo: " + ToString(v.ShowPixelInfo) + "\n";
        inner = inner + "ShowZoomButtons: " + ToString(v.ShowZoomButtons) + "\n";
        inner = inner + "ShowOptionsPanel: " + ToString(v.ShowOptionsPanel) + "\n";
        inner = inner + "ShowOptionsInTooltip: " + ToString(v.ShowOptionsInTooltip) + "\n";
        inner = inner + "ShowOptionsButton: " + ToString(v.ShowOptionsButton) + "\n";
        inner = inner + "WatchedPixels: " + ToString(v.WatchedPixels) + "\n";
        inner = inner + "AddWatchedPixelOnDoubleClick: " + ToString(v.AddWatchedPixelOnDoubleClick) + "\n";
        inner = inner + "HighlightWatchedPixels: " + ToString(v.HighlightWatchedPixels) + "\n";
        inner = inner + "MouseInfo: " + ToString(v.MouseInfo) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }
    
}
