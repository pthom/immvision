#include "immvision/inspector.h"
#include "immvision/image.h"
#include "immvision/internal/image_cache.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/imgui/image_widgets.h"
#include "immvision/internal/gl/gl_provider.h"
#include "immvision/imgui_imm.h"

#include "imgui.h"
#include "imgui_internal.h"


namespace ImmVision
{
    static ImVec2 gInspectorImageSize(0.f, 0.f);

    struct Inspector_ImageAndParams
    {
        ImageCache::KeyType id;
        std::string Label;
        ImageBuffer Image;
        ImageParams Params;

        Point2d InitialZoomCenter = Point2d();
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
        const ImageBuffer& image_buf,
        const std::string& legend,
        const std::string& zoomKey,
        const std::string& colormapKey,
        const Point2d & zoomCenter,
        double zoomRatio
    )
    {
        ImageBuffer image = image_buf.clone();
        ImageParams params;
        params.ZoomKey = zoomKey;
        params.ColormapKey = colormapKey;
        params.ShowOptionsPanel = true;

        if (gInspectorImageSize.x > 0.f)
            params.ImageDisplaySize = Size((int)gInspectorImageSize.x, (int)gInspectorImageSize.y);

        std::string label = legend + "##" + std::to_string(s_Inspector_ImagesAndParams.size());
        auto id = sInspectorImageTextureCache.GetID(label, sDontUseIdStack);
        s_Inspector_ImagesAndParams.push_back({id, label, image, params, zoomCenter, zoomRatio});

        // bump cache
        {
            auto& imageAndParams = s_Inspector_ImagesAndParams.back();
            sInspectorImageTextureCache.UpdateCache(id, imageAndParams.Image, &imageAndParams.Params, true);
        }
    }

    static float s_Inspector_ThumbnailHeight = 60.f;

    // Returns the index of the image to suppress, or -1
    int priv_Inspector_ShowFilmstrip()
    {
        int idxSuppress = -1;
        float thumbH = s_Inspector_ThumbnailHeight;

        // Horizontal scrolling child for the filmstrip
        float labelFontSize = ImGui::GetFontSize() * 0.75f;
        float largeScrollbarH = ImGui::GetFontSize() * 1.25f;
        float contentH = thumbH + labelFontSize + ImGui::GetStyle().ItemSpacing.y * 2.f;
        float stripH = contentH + largeScrollbarH;
        // Vertical slider for thumbnail size, to the left of the strip (fixed height)
        float sliderH = ImGui::GetFontSize() * 5.f;
        float em = ImGui::GetFontSize();
        ImGui::VSliderFloat("##thumbsize", ImVec2(em * 2.0f, sliderH), &s_Inspector_ThumbnailHeight, em * 3.f, em * 14.f, "");
        ImGui::SetItemTooltip("Thumbnail size: %.0f px", s_Inspector_ThumbnailHeight);
        ImGui::SameLine();
        // Larger scrollbar for the filmstrip
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, largeScrollbarH);

        ImGui::BeginChild("##filmstrip", ImVec2(0.f, stripH), ImGuiChildFlags_None,
                          ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

        // Forward mouse wheel to horizontal scroll
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
        {
            float wheel = ImGui::GetIO().MouseWheel + ImGui::GetIO().MouseWheelH;
            if (wheel != 0.f)
                ImGui::SetScrollX(ImGui::GetScrollX() - wheel * thumbH * 0.5f);
        }

        for (size_t i = 0; i < s_Inspector_ImagesAndParams.size(); ++i)
        {
            ImGui::PushID((int)i);

            auto& imageAndParams = s_Inspector_ImagesAndParams[i];
            const bool is_selected = (s_Inspector_CurrentIndex == i);

            auto id = sInspectorImageTextureCache.GetID(imageAndParams.Label, sDontUseIdStack);
            auto& cacheImage = sInspectorImageTextureCache.GetCacheImageAndTexture(id);

            // Compute thumbnail width from aspect ratio
            float imageRatio = cacheImage.mGlTexture->SizeImVec2().x / cacheImage.mGlTexture->SizeImVec2().y;
            if (imageRatio < 0.1f) imageRatio = 1.f; // safety
            float thumbW = imageRatio * thumbH;

            // Total item width: thumbnail + label space
            float labelWidth = ImGui::CalcTextSize(imageAndParams.Label.c_str()).x * (labelFontSize / ImGui::GetFontSize());
            float itemW = std::max(thumbW, std::min(labelWidth, thumbW * 1.5f));

            // All items on one line
            if (i > 0)
                ImGui::SameLine();

            // Draw the thumbnail as a selectable group
            ImGui::BeginGroup();
            {
                ImVec2 thumbTl = ImGui::GetCursorScreenPos();

                // Selectable background (covers thumb + label)
                float totalH = thumbH + labelFontSize + 2.f;
                std::string selId = "##sel_" + std::to_string(i);
                if (ImGui::Selectable(selId.c_str(), is_selected, 0, ImVec2(itemW, totalH)))
                    s_Inspector_CurrentIndex = i;
                ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

                // Draw thumbnail image on top of selectable (use mipmap filtering for quality)
                using TF = ImmVision_GlProvider::TextureFilter;
                ImmVision_GlProvider::SetTextureFiltering(cacheImage.mGlTexture->TextureId, TF::LinearMipmapLinear, TF::Linear);
                ImVec2 imgTl(thumbTl.x + (itemW - thumbW) * 0.5f, thumbTl.y);
                ImVec2 imgBr(imgTl.x + thumbW, imgTl.y + thumbH);
                ImGui::GetWindowDrawList()->AddImage(cacheImage.mGlTexture->TextureId, imgTl, imgBr);
                ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

                // Selected border
                if (is_selected)
                    ImGui::GetWindowDrawList()->AddRect(imgTl, imgBr, IM_COL32(100, 150, 255, 255), 0.f, 0, 2.f);

                // Delete button on hover (drawn via DrawList, subtle circle + "x")
                if (ImGui::IsItemHovered())
                {
                    ImDrawList* dl = ImGui::GetWindowDrawList();
                    float radius = ImGui::GetFontSize() * 0.45f;
                    ImVec2 center(std::floor(imgBr.x - radius - 3.f) + 0.5f, std::floor(imgTl.y + radius + 3.f) + 0.5f);

                    // Check if mouse is over the delete circle
                    ImVec2 mouse = ImGui::GetMousePos();
                    float dx = mouse.x - center.x, dy = mouse.y - center.y;
                    bool hoveringBtn = (dx * dx + dy * dy) <= radius * radius;

                    // Draw circle background + "x" with transparency
                    ImU32 bgColor = hoveringBtn ? IM_COL32(200, 60, 60, 200) : IM_COL32(0, 0, 0, 140);
                    ImU32 xColor = IM_COL32(255, 255, 255, hoveringBtn ? 255 : 200);
                    dl->AddCircleFilled(center, radius, bgColor);
                    float cross = radius * 0.45f;
                    center.x -= 0.5f; center.y -= 0.5f;
                    dl->AddLine(ImVec2(center.x - cross, center.y - cross), ImVec2(center.x + cross, center.y + cross), xColor, 1.5f);
                    dl->AddLine(ImVec2(center.x + cross, center.y - cross), ImVec2(center.x - cross, center.y + cross), xColor, 1.5f);

                    if (hoveringBtn && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        idxSuppress = (int)i;
                }

                // Label below thumbnail (smaller font, clipped to thumbnail width)
                {
                    ImVec2 labelPos(thumbTl.x, thumbTl.y + thumbH + 1.f);
                    ImGui::GetWindowDrawList()->PushClipRect(
                        labelPos, ImVec2(labelPos.x + itemW, labelPos.y + labelFontSize + 1.f), true);
                    ImGui::GetWindowDrawList()->AddText(
                        ImGui::GetFont(), labelFontSize, labelPos,
                        ImGui::GetColorU32(ImGuiCol_Text), imageAndParams.Label.c_str());
                    ImGui::GetWindowDrawList()->PopClipRect();
                }

            }
            ImGui::EndGroup();

            ImGui::PopID();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar(); // ScrollbarSize

        return idxSuppress;
    }

    void priv_Inspector_CleanImagesParams(const ImVec2& imageSize)
    {
        for (auto& i :s_Inspector_ImagesAndParams)
        {
            // Force image size
            i.Params.ImageDisplaySize = Size((int)imageSize.x, (int)imageSize.y);

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

    void Inspector_Show()
    {
        ImageWidgets::s_CollapsingHeader_CacheState_Sync = true;

        //
        // Filmstrip at the top
        //
        int idxSuppress = priv_Inspector_ShowFilmstrip();
        if (idxSuppress >= 0)
            s_Inspector_ImagesAndParams.erase(s_Inspector_ImagesAndParams.begin() + (size_t)idxSuppress);

        ImGui::Separator();

        //
        // Selected image below, using remaining available space
        //
        {
            if (s_Inspector_ImagesAndParams.empty())
                s_Inspector_CurrentIndex = 0;
            else if (s_Inspector_CurrentIndex >= s_Inspector_ImagesAndParams.size())
                s_Inspector_CurrentIndex = s_Inspector_ImagesAndParams.size() - 1;

            if (s_Inspector_CurrentIndex < s_Inspector_ImagesAndParams.size())
            {
                auto& imageAndParams = s_Inspector_ImagesAndParams[s_Inspector_CurrentIndex];

                // Compute image display size from available space
                float emSize = ImGui::GetFontSize();
                bool showOptionsColumn = imageAndParams.Params.ShowOptionsPanel && !imageAndParams.Params.ShowOptionsInTooltip;
                float optionsWidth = showOptionsColumn ? emSize * 19.f : 0.f;
                // Reserve space for zoom buttons, pixel info, color widget, etc.
                float imageInfoHeight = emSize * 7.f;

                ImVec2 avail = ImGui::GetContentRegionAvail();
                ImVec2 imageSize(
                    std::max(1.f, avail.x - optionsWidth - emSize),
                    std::max(1.f, avail.y - imageInfoHeight));
                imageAndParams.Params.ImageDisplaySize = Size((int)imageSize.x, (int)imageSize.y);
                gInspectorImageSize = imageSize;

                priv_Inspector_CleanImagesParams(imageSize);
                ImmVision::Image(imageAndParams.Label, imageAndParams.Image, &imageAndParams.Params);
            }
        }

        ImageWidgets::s_CollapsingHeader_CacheState_Sync = false;
    }

    void Inspector_ClearImages()
    {
        s_Inspector_ImagesAndParams.clear();
        sInspectorImageTextureCache.ClearImagesCache();
        s_Inspector_CurrentIndex = 0;
    }

}
