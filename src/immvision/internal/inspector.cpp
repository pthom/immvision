#include "immvision/inspector.h"
#include "immvision/image.h"
#include "immvision/internal/image_cache.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/imgui/image_widgets.h"

#include "imgui.h"


namespace ImmVision
{
    static ImVec2 gInspectorImageSize(0.f, 0.f);

    struct Inspector_ImageAndParams
    {
        ImageCache::KeyType id;
        std::string Label;
        cv::Mat Image;
        ImageParams Params;

        cv::Point2d InitialZoomCenter = cv::Point2d();
        double InitialZoomRatio = 1.;
        bool WasSentToTextureCache = false;
    };

    static std::vector<Inspector_ImageAndParams> s_Inspector_ImagesAndParams;
    ImageCache::ImageTextureCache sInspectorImageTextureCache(3600. * 24. * 365. * 10.); // 10 years TTL!
    static size_t s_Inspector_CurrentIndex = 0;

    // In the inspector, we cannot rely on the ID stack, since calls to AddImages will have a different stack
    // than when will later display the image
    static bool sDontUseIdStack = false;


    void Inspector_AddImage(
        const cv::Mat& image,
        const std::string& legend,
        const std::string& zoomKey,
        const std::string& colormapKey,
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        bool isColorOrderBGR
    )
    {
        ImageParams params;
        params.IsColorOrderBGR = isColorOrderBGR;
        params.ZoomKey = zoomKey;
        params.ColormapKey = colormapKey;
        params.ShowOptionsPanel = true;

        if (gInspectorImageSize.x > 0.f)
            params.ImageDisplaySize = cv::Size((int)gInspectorImageSize.x, (int)gInspectorImageSize.y);

        std::string label = legend + "##" + std::to_string(s_Inspector_ImagesAndParams.size());
        auto id = sInspectorImageTextureCache.GetID(label, sDontUseIdStack);
        s_Inspector_ImagesAndParams.push_back({id, label, image.clone(), params, zoomCenter, zoomRatio});

        // bump cache
        {
            auto& imageAndParams = s_Inspector_ImagesAndParams.back();
            sInspectorImageTextureCache.UpdateCache(id, imageAndParams.Image, &imageAndParams.Params, true);
        }
    }

    void priv_Inspector_ShowImagesListbox(float width)
    {
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        int idxSuppress = -1;
        if (ImGui::BeginListBox("##ImageList",
                                ImVec2(width - 10.f, ImGui::GetContentRegionAvail().y)))
        {
            for (size_t i = 0; i < s_Inspector_ImagesAndParams.size(); ++i)
            {
                ImGui::PushID(i * 3424553);

                auto& imageAndParams = s_Inspector_ImagesAndParams[i];

                const bool is_selected = (s_Inspector_CurrentIndex == i);

                auto id = sInspectorImageTextureCache.GetID(imageAndParams.Label, sDontUseIdStack);
                auto &cacheImage = sInspectorImageTextureCache.GetCacheImageAndTexture(id);

                ImVec2 itemSize(width - 10.f, ImGui::GetFontSize() * 3.f);
                float imageHeight = itemSize.y - ImGui::GetTextLineHeight();
                ImVec2 pos = ImGui::GetCursorScreenPos();

                {
                    auto col = ImGui::GetStyle().Colors[ImGuiCol_Button];
                    col.x = 1.;
                    ImGui::PushStyleColor(ImGuiCol_Button, col);
                    if (ImGui::SmallButton("x"))
                        idxSuppress = i;
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                }

                std::string id_selectable = imageAndParams.Label + "##_" + std::to_string(i);
                if (ImGui::Selectable(id_selectable.c_str(), is_selected, 0, itemSize))
                    s_Inspector_CurrentIndex = i;

                float imageRatio = cacheImage.GlTexture->mImageSize.x / cacheImage.GlTexture->mImageSize.y;
                ImVec2 image_tl(pos.x, pos.y + ImGui::GetTextLineHeight());
                ImVec2 image_br(pos.x + imageRatio * imageHeight, image_tl.y + imageHeight);

                ImGui::GetWindowDrawList()->AddImage(cacheImage.GlTexture->mImTextureId, image_tl, image_br);

                ImGui::PopID();
            }
            ImGui::EndListBox();

            if (idxSuppress >= 0)
                s_Inspector_ImagesAndParams.erase(s_Inspector_ImagesAndParams.begin() + (size_t)idxSuppress);
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
                    i.Params.ZoomPanMatrix = ZoomPanTransform::MakeZoomMatrix(
                        i.InitialZoomCenter, i.InitialZoomRatio, i.Params.ImageDisplaySize);
                }

                sInspectorImageTextureCache.UpdateCache(i.id, i.Image, &i.Params, true);
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
                v.Params.ShowOptionsPanel = currentParams.ShowOptionsPanel;
                v.Params.ShowOptionsInTooltip = currentParams.ShowOptionsInTooltip;
                v.Params.PanWithMouse = currentParams.PanWithMouse;
                v.Params.ZoomWithMouseWheel = currentParams.ZoomWithMouseWheel;
                v.Params.AddWatchedPixelOnDoubleClick = currentParams.AddWatchedPixelOnDoubleClick;
            }
        }
    };

    ImVec2 priv_Inspector_ImageSize(float listWidth, bool showOptionsColumn)
    {
        ImVec2 imageSize;

        float emSize = ImGui::GetFontSize();
        float x_margin = emSize * 2.f;
        float y_margin = emSize / 3.f;
        float image_info_height =  ImGui::GetFontSize() * 10.f;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto &params = s_Inspector_ImagesAndParams.front().Params;
            if (!params.ShowImageInfo)
                image_info_height -= emSize * 1.5f;
            if (!params.ShowPixelInfo)
                image_info_height -= emSize * 1.5f;
        }
        float image_options_width = showOptionsColumn ? ImGui::GetFontSize() * 19.f : 0.f;
        ImVec2 winSize = ImGui::GetWindowSize();
        imageSize = ImVec2(
            winSize.x - listWidth - x_margin - image_options_width,
            winSize.y - y_margin - image_info_height);
        if (imageSize.x < 1.f)
            imageSize.x = 1.f;
        if (imageSize.y < 1.f)
            imageSize.y = 1.f;

        gInspectorImageSize = imageSize;
        return imageSize;
    };


    void Inspector_Show()
    {
        ImageWidgets::s_CollapsingHeader_CacheState_Sync = true;

        bool showOptionsColumn = true;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto& params = s_Inspector_ImagesAndParams.front().Params;
            if ( (params.ShowOptionsInTooltip) || (!params.ShowOptionsPanel))
                showOptionsColumn = false;
        }

        static float initialListWidth = ImGui::GetFontSize() * 8.5f;
        static float currentListWidth = initialListWidth;

        ImVec2 imageSize = priv_Inspector_ImageSize(currentListWidth, showOptionsColumn);
        priv_Inspector_CleanImagesParams(imageSize);

        ImGui::Columns(2);

        //
        // First column: image list
        //
        {
            // Set column width
            {
                static int idxFrame = 0;
                ++idxFrame;
                if (idxFrame <= 2) // The column width is not set at the first frame
                    ImGui::SetColumnWidth(0, initialListWidth);
                ImGui::Text("Image list");
                currentListWidth = ImGui::GetColumnWidth(0);
            }
            // Show image list
            priv_Inspector_ShowImagesListbox(currentListWidth);
        }

        ImGui::NextColumn();

        //
        // Second column : image
        //
        {
            if (s_Inspector_ImagesAndParams.empty())
                s_Inspector_CurrentIndex = 0;
            else if (s_Inspector_CurrentIndex >= s_Inspector_ImagesAndParams.size())
                s_Inspector_CurrentIndex = s_Inspector_ImagesAndParams.size() - 1;

            if (s_Inspector_CurrentIndex < s_Inspector_ImagesAndParams.size())
            {
                auto& imageAndParams = s_Inspector_ImagesAndParams[s_Inspector_CurrentIndex];
                Image(imageAndParams.Label, imageAndParams.Image, &imageAndParams.Params);
            }
        }

        ImGui::Columns(1);

        ImageWidgets::s_CollapsingHeader_CacheState_Sync = false;
    }

    void Inspector_ClearImages()
    {
        s_Inspector_ImagesAndParams.clear();
        sInspectorImageTextureCache.ClearImagesCache();
        s_Inspector_CurrentIndex = 0;
    }

}