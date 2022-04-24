#include "immvision/inspector.h"
#include "immvision/image_navigator.h"
#include "immvision/internal/image_navigator_cache.h"
#include "immvision/internal/gl/imgui_imm_gl_image.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/imgui/image_navigator_widgets.h"

#include "imgui.h"


namespace ImmVision
{
    struct Inspector_ImageAndParams
    {
        cv::Mat Image;
        ImageNavigatorParams Params;

        const cv::Point2d InitialZoomCenter = cv::Point2d();
        double InitialZoomRatio = 1.;
        bool WasSentToTextureCache = false;
    };

    static std::vector<Inspector_ImageAndParams> s_Inspector_ImagesAndParams;
    static size_t s_Inspector_CurrentIndex = 0;


    void Inspector_AddImage(
        const cv::Mat& image,
        const std::string& legend,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey,
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        bool isColorOrderBGR
    )
    {
        ImageNavigatorParams params;
        params.Legend = legend;
        params.IsColorOrderBGR = isColorOrderBGR;
        params.ZoomKey = zoomKey;
        params.ColorAdjustmentsKey = colorAdjustmentsKey;
        params.ShowOptions = true;

        s_Inspector_ImagesAndParams.push_back({image, params, zoomCenter, zoomRatio});
    }

    void priv_Inspector_ShowImagesListbox(float width)
    {
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        if (ImGui::BeginListBox("##ImageNavigatorList",
                                ImVec2(width - 10.f, ImGui::GetContentRegionAvail().y)))
        {
            for (size_t i = 0; i < s_Inspector_ImagesAndParams.size(); ++i)
            {
                const bool is_selected = (s_Inspector_CurrentIndex == i);

                std::string id = s_Inspector_ImagesAndParams[i].Params.Legend + "##_" + std::to_string(i);
                auto &cacheImage = ImageNavigatorCache::gImageNavigatorTextureCache.GetCacheImages(
                    s_Inspector_ImagesAndParams[i].Image);

                ImVec2 itemSize(width - 10.f, 40.f);
                float imageHeight = itemSize.y - ImGui::GetTextLineHeight();
                ImVec2 pos = ImGui::GetCursorScreenPos();
                if (ImGui::Selectable(id.c_str(), is_selected, 0, itemSize))
                    s_Inspector_CurrentIndex = i;

                float imageRatio = cacheImage.GlTexture->mImageSize.x / cacheImage.GlTexture->mImageSize.y;
                ImVec2 image_tl(pos.x, pos.y + ImGui::GetTextLineHeight());
                ImVec2 image_br(pos.x + imageRatio * imageHeight, image_tl.y + imageHeight);

                ImGuiImmGlImage::GetWindowDrawList_AddImage(cacheImage.GlTexture->mImTextureId, image_tl, image_br);
            }
            ImGui::EndListBox();
        }
    };

    void priv_Inspector_CleanImagesParams(const ImVec2& imageSize)
    {
        for (auto& i :s_Inspector_ImagesAndParams)
        {
            // Force image size
            i.Params.ImageDisplaySize = cv::Size((int)imageSize.x, (int)imageSize.y);

            // Store in texture cache
            if (! i.WasSentToTextureCache)
            {
                if (i.InitialZoomRatio > 0.)
                {
                    i.Params.ZoomMatrix = ZoomPanTransform::MakeZoomMatrix(
                        i.InitialZoomCenter, i.InitialZoomRatio, i.Params.ImageDisplaySize);
                }
                ImageNavigatorCache::gImageNavigatorTextureCache.UpdateCache(i.Image, &i.Params, true);
                i.WasSentToTextureCache = true;
            }
        }

        // Propagate current options to hidden images
        if (s_Inspector_CurrentIndex < s_Inspector_ImagesAndParams.size())
        {
            const auto& currentParams = s_Inspector_ImagesAndParams[s_Inspector_CurrentIndex].Params;
            for (auto& v : s_Inspector_ImagesAndParams)
            {
                v.Params.ShowImageInfo = currentParams.ShowImageInfo;
                v.Params.ShowPixelInfo = currentParams.ShowPixelInfo;
                v.Params.ShowZoomButtons = currentParams.ShowZoomButtons;
                v.Params.ShowLegendBorder = currentParams.ShowLegendBorder;
                v.Params.ShowOptions = currentParams.ShowOptions;
                v.Params.ShowOptionsInTooltip = currentParams.ShowOptionsInTooltip;
                v.Params.PanWithMouse = currentParams.PanWithMouse;
                v.Params.ZoomWithMouseWheel = currentParams.ZoomWithMouseWheel;
            }
        }
    };

    ImVec2 priv_Inspector_ImageSize(float listWidth, bool showOptionsColumn)
    {
        ImVec2 imageSize;

        float x_margin = 30.f;
        float y_margin = 5.f;
        float navigator_info_height = 120.f;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto &params = s_Inspector_ImagesAndParams.front().Params;
            if (!params.ShowImageInfo)
                navigator_info_height -= 20.f;
            if (!params.ShowPixelInfo)
                navigator_info_height -= 20.f;
        }
        float navigator_options_width = showOptionsColumn ? 300.f : 0.f;
        ImVec2 winSize = ImGui::GetWindowSize();
        imageSize = ImVec2(
            winSize.x - listWidth - x_margin - navigator_options_width,
            winSize.y - y_margin - navigator_info_height);
        if (imageSize.x < 1.f)
            imageSize.x = 1.f;
        if (imageSize.y < 1.f)
            imageSize.y = 1.f;
        return imageSize;
    };


    void Inspector_Show()
    {
        ImageNavigatorWidgets::s_CollapsingHeader_CacheState_Sync = true;

        bool showOptionsColumn = true;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto& params = s_Inspector_ImagesAndParams.front().Params;
            if ( (params.ShowOptionsInTooltip) || (!params.ShowOptions))
                showOptionsColumn = false;
        }

        static float listWidth = ImGui::GetWindowSize().x / 10.f;

        ImVec2 imageSize = priv_Inspector_ImageSize(listWidth, showOptionsColumn);
        priv_Inspector_CleanImagesParams(imageSize);

        ImGui::Columns(2);

        //
        // First column: image list
        //
        {
            // Set column width
            {
                static bool wasWidthSet = false;
                if (!wasWidthSet)
                {
                    ImGui::SetColumnWidth(0, listWidth);
                    wasWidthSet = true;
                }
                ImGui::Text("Image list");
                listWidth = ImGui::GetColumnWidth(0);
            }
            // Show image list
            priv_Inspector_ShowImagesListbox(listWidth);
        }

        ImGui::NextColumn();

        //
        // Second column : navigator
        //
        {
            if (s_Inspector_ImagesAndParams.empty())
                s_Inspector_CurrentIndex = 0;
            else if (s_Inspector_CurrentIndex >= s_Inspector_ImagesAndParams.size())
                s_Inspector_CurrentIndex = s_Inspector_ImagesAndParams.size() - 1;

            if (s_Inspector_CurrentIndex < s_Inspector_ImagesAndParams.size())
            {
                auto& imageAndParams = s_Inspector_ImagesAndParams[s_Inspector_CurrentIndex];
                ImageNavigator(imageAndParams.Image, &imageAndParams.Params);
            }
        }

        ImGui::Columns(1);

        ImageNavigatorWidgets::s_CollapsingHeader_CacheState_Sync = false;
    }

    void Inspector_ClearImages()
    {
        s_Inspector_ImagesAndParams.clear();
    }

}