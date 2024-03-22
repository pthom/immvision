#ifdef IMMVISION_SERIALIZE_JSON

#include "immvision/image.h"
#include "nlohmann/json.hpp"

namespace nlohmann
{
    template <>
    struct adl_serializer<cv::Size>
    {
        static void to_json(json& j, const cv::Size& size) {
            j = json{{"width", size.width}, {"height", size.height}};
        }

        static void from_json(const json& j, cv::Size& size) {
            j.at("width").get_to(size.width);
            j.at("height").get_to(size.height);
        }
    };

    template <>
    struct adl_serializer<cv::Point>
    {
        static void to_json(json& j, const cv::Point& point) {
            j = json{{"x", point.x}, {"y", point.y}};
        }

        static void from_json(const json& j, cv::Point& point) {
            j.at("x").get_to(point.x);
            j.at("y").get_to(point.y);
        }
    };

    template <>
    struct adl_serializer<cv::Matx33d>
    {
        static void to_json(json& j, const cv::Matx33d& mat) {
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    j[std::to_string(row) + std::to_string(col)] = mat(row, col);
                }
            }
        }

        static void from_json(const json& j, cv::Matx33d& mat) {
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    mat(row, col) = j[std::to_string(row) + std::to_string(col)];
                }
            }
        }
    };

    template <>
    struct adl_serializer<ImmVision::ColormapScaleFromStatsData>
    {
        static void to_json(json& j, const ImmVision::ColormapScaleFromStatsData& data) {
            j = json{
                {"ColorMapStatsType", data.ColorMapStatsType},
                {"NbSigmas", data.NbSigmas},
                {"UseStatsMin", data.UseStatsMin},
                {"UseStatsMax", data.UseStatsMax}
            };
        }

        static void from_json(const json& j, ImmVision::ColormapScaleFromStatsData& data) {
            j.at("ColorMapStatsType").get_to(data.ColorMapStatsType);
            j.at("NbSigmas").get_to(data.NbSigmas);
            j.at("UseStatsMin").get_to(data.UseStatsMin);
            j.at("UseStatsMax").get_to(data.UseStatsMax);
        }
    };

    template <>
    struct adl_serializer<ImmVision::ColormapSettingsData>
    {
        static void to_json(json& j, const ImmVision::ColormapSettingsData& data) {
            j = json{
                {"Colormap", data.Colormap},
                {"ColormapScaleMin", data.ColormapScaleMin},
                {"ColormapScaleMax", data.ColormapScaleMax},
                {"ColormapScaleFromStats", data.ColormapScaleFromStats},
                {"internal_ColormapHovered", data.internal_ColormapHovered}
            };
        }

        static void from_json(const json& j, ImmVision::ColormapSettingsData& data) {
            j.at("Colormap").get_to(data.Colormap);
            j.at("ColormapScaleMin").get_to(data.ColormapScaleMin);
            j.at("ColormapScaleMax").get_to(data.ColormapScaleMax);
            j.at("ColormapScaleFromStats").get_to(data.ColormapScaleFromStats);
            j.at("internal_ColormapHovered").get_to(data.internal_ColormapHovered);
        }
    };

    template <>
    struct adl_serializer<ImmVision::ImageParams>
    {
        static void to_json(json& j, const ImmVision::ImageParams& params)
        {
            j = json{
                {"RefreshImage", params.RefreshImage},
                {"ImageDisplaySize", params.ImageDisplaySize},
                {"ZoomPanMatrix", params.ZoomPanMatrix},
                {"ZoomKey", params.ZoomKey},
                {"ColormapSettings", params.ColormapSettings},
                {"ColormapKey", params.ColormapKey},
                {"PanWithMouse", params.PanWithMouse},
                {"ZoomWithMouseWheel", params.ZoomWithMouseWheel},
                {"IsColorOrderBGR", params.IsColorOrderBGR},
                {"SelectedChannel", params.SelectedChannel},
                {"ShowSchoolPaperBackground", params.ShowSchoolPaperBackground},
                {"ShowAlphaChannelCheckerboard", params.ShowAlphaChannelCheckerboard},
                {"ShowGrid", params.ShowGrid},
                {"DrawValuesOnZoomedPixels", params.DrawValuesOnZoomedPixels},
                {"ShowImageInfo", params.ShowImageInfo},
                {"ShowPixelInfo", params.ShowPixelInfo},
                {"ShowZoomButtons", params.ShowZoomButtons},
                {"ShowOptionsPanel", params.ShowOptionsPanel},
                {"ShowOptionsInTooltip", params.ShowOptionsInTooltip},
                {"ShowOptionsButton", params.ShowOptionsButton},
                {"WatchedPixels", params.WatchedPixels},
                {"AddWatchedPixelOnDoubleClick", params.AddWatchedPixelOnDoubleClick},
                {"HighlightWatchedPixels", params.HighlightWatchedPixels}
            };
        }

        static void from_json(const json& j, ImmVision::ImageParams& params)
        {
            j.at("RefreshImage").get_to(params.RefreshImage);
            j.at("ImageDisplaySize").get_to(params.ImageDisplaySize);
            j.at("ZoomPanMatrix").get_to(params.ZoomPanMatrix);
            j.at("ZoomKey").get_to(params.ZoomKey);
            j.at("ColormapSettings").get_to(params.ColormapSettings);
            j.at("ColormapKey").get_to(params.ColormapKey);
            j.at("PanWithMouse").get_to(params.PanWithMouse);
            j.at("ZoomWithMouseWheel").get_to(params.ZoomWithMouseWheel);
            j.at("IsColorOrderBGR").get_to(params.IsColorOrderBGR);
            j.at("SelectedChannel").get_to(params.SelectedChannel);
            j.at("ShowSchoolPaperBackground").get_to(params.ShowSchoolPaperBackground);
            j.at("ShowAlphaChannelCheckerboard").get_to(params.ShowAlphaChannelCheckerboard);
            j.at("ShowGrid").get_to(params.ShowGrid);
            j.at("DrawValuesOnZoomedPixels").get_to(params.DrawValuesOnZoomedPixels);
            j.at("ShowImageInfo").get_to(params.ShowImageInfo);
            j.at("ShowPixelInfo").get_to(params.ShowPixelInfo);
            j.at("ShowZoomButtons").get_to(params.ShowZoomButtons);
            j.at("ShowOptionsPanel").get_to(params.ShowOptionsPanel);
            j.at("ShowOptionsInTooltip").get_to(params.ShowOptionsInTooltip);
            j.at("ShowOptionsButton").get_to(params.ShowOptionsButton);
            j.at("WatchedPixels").get_to(params.WatchedPixels);
            j.at("AddWatchedPixelOnDoubleClick").get_to(params.AddWatchedPixelOnDoubleClick);
            j.at("HighlightWatchedPixels").get_to(params.HighlightWatchedPixels);
        }

    };
} // namespace nlohmann


namespace ImmVision
{
    using json = nlohmann::json;


    std::string ImageParamsToJson(const ImageParams& params)
    {
        json j = params;
        return j.dump(4);
    }

    void FillImageParamsFromJson(const std::string& json, ImageParams* params)
    {
        nlohmann::json j = nlohmann::json::parse(json);
        nlohmann::adl_serializer<ImageParams>::from_json(j, *params);
    }

    ImageParams ImageParamsFromJson(const std::string& json)
    {
        ImageParams params;
        FillImageParamsFromJson(json, &params);
        return params;
    }

}
#endif
