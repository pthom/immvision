#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "immvision/image_navigator.h"
#include "immvision/internal/drawing/internal_icons.h"
#include "immvision/internal/imgui/imgui_imm.h"
#include "immvision/internal/sys/portable_file_dialogs.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/cv/color_adjustment_utils.h"
#include "immvision/internal/imgui/image_navigator_widgets.h"
#include "immvision/internal/image_navigator_cache.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <map>
#include <vector>
#include <iostream>

namespace ImmVision
{
    void ClearNavigatorTextureCache()
    {
        ImageNavigatorCache::gImageNavigatorTextureCache.ClearImagesCache();
    }

    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams* params,
        bool refresh)
    {
        using CachedParams = ImageNavigatorCache::ImageNavigatorTextureCache::CachedParams;
        using CachedImages = ImageNavigatorCache::ImageNavigatorTextureCache::CachedImages;

        //
        // Lambda / panel Title
        //
        auto fnPanelTitle = [&params, &image]()
        {
            std::string panelTitle;
            {
                if (params->ShowLegendBorder)
                    panelTitle = params->Legend;
                panelTitle += "##ImageNavigator_" + std::to_string((size_t)&image);
            }
            return panelTitle;
        };

        //
        // Lambdas / Watched Pixels
        //
        bool wasWatchedPixelAdded = false;
        auto fnWatchedPixels_Add = [&params, &wasWatchedPixelAdded](const cv::Point2d& pixelDouble)
        {
            cv::Point pixel((int)(pixelDouble.x + .5), (int)(pixelDouble.y + .5));
            params->WatchedPixels.push_back(pixel);

            wasWatchedPixelAdded = true;
            if (! params->ShowOptionsInTooltip)
                params->ShowOptions = true;
        };
        auto fnWatchedPixels_Gui = [&params, &image]()
        {
            ImGui::Text("Double click the image...");

            int idxToRemove = -1;

            if (! params->WatchedPixels.empty())
            {
                ImGui::BeginTable("WatchedPixels", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX);
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                ImGui::TableNextColumn();
                ImGui::Text("#");
                ImGui::TableNextColumn();
                ImGui::Text("(x,y)");
                ImGui::TableNextColumn();
                ImGui::Text("Color");
                ImGui::TableNextColumn();
                ImGui::Text("%s", "");

                for (size_t i = 0; i < params->WatchedPixels.size(); ++i)
                {
                    cv::Point watchedPixel = params->WatchedPixels[i];
                    ImGui::TableNextRow();

                    // index
                    ImGui::TableNextColumn();
                    ImGui::Text("#%i: ", (int)i);

                    // (x,y)
                    ImGui::TableNextColumn();
                    std::string posStr = std::string("(") + std::to_string(watchedPixel.x) + "," + std::to_string(watchedPixel.y) +")";
                    ImGui::Text("%s", posStr.c_str());

                    // Show Color Cell
                    ImGui::TableNextColumn();
                    ImageNavigatorWidgets::ShowPixelColorWidget(image, watchedPixel, *params);

                    // Actions
                    ImGui::TableNextColumn();
                    std::string lblRemove = "x##" + std::to_string(i);
                    if (ImGui::SmallButton(lblRemove.c_str()))
                        idxToRemove = (int) i;
                    ImGui::SameLine();
                }
                ImGui::EndTable();
            }
            if (idxToRemove >= 0)
                params->WatchedPixels.erase(params->WatchedPixels.begin() + (std::ptrdiff_t)idxToRemove);

            ImGui::Checkbox("Highlight Watched Pixels", &params->HighlightWatchedPixels);
        };

        //
        // Lambdas / Options & Adjustments
        //
        auto fnOptionsInnerGui = [&params, &image, &fnWatchedPixels_Gui, &wasWatchedPixelAdded](
                CachedParams & cacheParams)
        {
            float optionsWidth = 330.f;
            // Group with fixed width, so that Collapsing headers stop at optionsWidth
            ImGuiImm::BeginGroupFixedWidth(optionsWidth);

            // Adjust float values
            bool hasAdjustFloatValues = true; // ((image.depth() == CV_32F) || (image.depth() == CV_64F));

            if (hasAdjustFloatValues && ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Adjust"))
            {
                ImGui::PushItemWidth(200.);
                ImGuiImm::SliderDouble(
                    "k", &params->ColorAdjustments.Factor,
                    0., 32., "%.3f", ImGuiSliderFlags_Logarithmic);
                ImGui::PushItemWidth(200.);
                ImGuiImm::SliderDouble(
                    "Delta", &params->ColorAdjustments.Delta,
                    -255., 255., "%.3f", ImGuiSliderFlags_Logarithmic);

                if (ImGui::Button("Default"))
                    params->ColorAdjustments = ColorAdjustmentsUtils::ComputeInitialImageAdjustments(image);
                ImGui::SameLine();
                 if (ImGui::Button("No Adjustment"))
                        params->ColorAdjustments = ColorAdjustmentsValues();
            }

            // Watched Pixels
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Watched Pixels", wasWatchedPixelAdded))
                fnWatchedPixels_Gui();

            // Image display options
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Image Display"))
            {
                if (image.type() == CV_8UC3 || image.type() == CV_8UC4)
                {
                    ImGui::Text("Color Order");
                    ImGui::SameLine();
                    int v = params->IsColorOrderBGR ? 0 : 1;
                    ImGui::RadioButton("RGB", &v, 1);
                    ImGui::SameLine();
                    ImGui::RadioButton("BGR", &v, 0);
                    params->IsColorOrderBGR = (v == 0);
                }
                if (image.type() == CV_8UC4)
                    ImGui::Checkbox("Show alpha channel checkerboard", &params->ShowAlphaChannelCheckerboard);
                if (image.channels() > 1)
                {
                    ImGui::Text("Channels: ");
                    ImGui::RadioButton("All", &params->SelectedChannel, -1); ImGui::SameLine();
                    for (int channel_id = 0; channel_id < image.channels(); ++channel_id)
                    {
                        ImGui::RadioButton(std::to_string(channel_id).c_str(), &params->SelectedChannel, channel_id);
                        ImGui::SameLine();
                    }
                    ImGui::NewLine();
                }
                {
                    ImGuiImm::BeginGroupPanel("High zoom options");
                    ImGui::Checkbox("Grid", &params->ShowGrid);
                    ImGui::Checkbox("Draw values on pixels", &params->DrawValuesOnZoomedPixels);
                    ImGuiImm::EndGroupPanel();
                }

            }

            //Navigator display options
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Options"))
            {
                {
                    ImGuiImm::BeginGroupPanel("Navigator display options");
                    ImGui::Checkbox("Show image info", &params->ShowImageInfo);
                    ImGui::Checkbox("Show pixel info", &params->ShowPixelInfo);
                    ImGui::Checkbox("Show zoom buttons", &params->ShowZoomButtons);
                    ImGui::Checkbox("Show legend border", &params->ShowLegendBorder);
                    ImGuiImm::EndGroupPanel();
                }

                ImGui::Checkbox("Pan with mouse", &params->PanWithMouse);
                ImGui::Checkbox("Zoom with mouse wheel", &params->ZoomWithMouseWheel);

                ImGui::Separator();
                if (ImGui::Checkbox("Show Options in tooltip window", &params->ShowOptionsInTooltip))
                {
                    if (!params->ShowOptionsInTooltip) // We were in a tooltip when clicking
                        params->ShowOptions = true;
                }
            }

            // Save Image
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Save"))
            {
                // Use portable_file_dialogs if available
                if (pfd::settings::available())
                {
                    if (ImGui::Button("Save Image"))
                    {
                        pfd::settings::verbose(true);
                        std::string filename = pfd::save_file("Select a file", ".",
                                                              { "Image Files", "*.png *.jpg *.jpeg *.jpg *.bmp *.gif *.exr",
                                                                "All Files", "*" }).result();
                        if (!filename.empty())
                        {
                            try
                            {
                                cv::imwrite(filename, image);
                            }
                            catch(const cv::Exception& e)
                            {
                                std::string errorMessage = std::string("Could not save image\n") + e.err.c_str();
                                pfd::message("Error", errorMessage, pfd::choice::ok, pfd::icon::error);
                            }
                        }
                    }
                }
                else
                {
                    ImGui::Text("File name");
                    char *filename = cacheParams.FilenameEditBuffer.data();
                    ImGui::SetNextItemWidth(200.f);
                    ImGui::InputText("##filename", filename, 1000);
                    //ImGui::SetNextItemWidth(200.f);
                    ImGui::Text("The image will be saved in the current folder");
                    ImGui::Text("with a format corresponding to the extension");
                    if (ImGui::SmallButton("save"))
                        cv::imwrite(filename, image);
                }
            }

            ImGuiImm::EndGroupFixedWidth();

        };
        auto fnToggleShowOptions = [&params]()
        {
            if (params->ShowOptionsInTooltip)
                ImGui::OpenPopup("Options");
            else
                params->ShowOptions = !params->ShowOptions;
        };
        auto fnOptionGui = [&params, &fnOptionsInnerGui](CachedParams & cacheParams)
        {
            if (params->ShowOptionsInTooltip)
            {
                if (ImGui::BeginPopup("Options"))
                {
                    fnOptionsInnerGui(cacheParams);
                    ImGui::EndPopup();
                }
            }
            else if (params->ShowOptions)
            {
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("Options");
                fnOptionsInnerGui(cacheParams);
                ImGui::EndGroup();
            }
        };

        //
        // Lambdas / Handle Zoom
        //
        // Mouse dragging
        auto fnHandleMouseDragging = [&params](CachedParams & cacheParams)
        {
            ZoomPanTransform::MatrixType& zoomMatrix = params->ZoomMatrix;

            int mouseDragButton = 0;
            bool isMouseDraggingInside = ImGui::IsMouseDragging(mouseDragButton) && ImGui::IsItemHovered();
            if (isMouseDraggingInside)
                cacheParams.IsMouseDragging = true;
            if (! ImGui::IsMouseDown(mouseDragButton))
            {
                cacheParams.IsMouseDragging = false;
                cacheParams.LastDragDelta = ImVec2(0.f, 0.f);
            }
            if (cacheParams.IsMouseDragging && params->PanWithMouse )
            {
                ImVec2 dragDelta = ImGui::GetMouseDragDelta(mouseDragButton);
                ImVec2 dragDeltaDelta(dragDelta.x - cacheParams.LastDragDelta.x, dragDelta.y - cacheParams.LastDragDelta.y);
                zoomMatrix = zoomMatrix * ZoomPanTransform::ComputePanMatrix(
                    cv::Point2d((double)dragDeltaDelta.x, (double)dragDeltaDelta.y),
                    zoomMatrix(0, 0));
                cacheParams.LastDragDelta = dragDelta;
            }
        };
        auto fnHandleMouseWheel = [&params](const cv::Point2d& mouseLocation)
        {
            if (!params->ZoomWithMouseWheel)
                return;
            ImGui::SetItemUsingMouseWheel();

            if ((fabs(ImGui::GetIO().MouseWheel) > 0.f) && (ImGui::IsItemHovered()))
            {
                double zoomRatio = (double)ImGui::GetIO().MouseWheel / 4.;
                params->ZoomMatrix = params->ZoomMatrix * ZoomPanTransform::ComputeZoomMatrix(mouseLocation, exp(zoomRatio));
            }
        };
        auto fnShowZoomButtons = [&params, &image]()
        {
            if (params->ShowZoomButtons)
            {
                ZoomPanTransform::MatrixType& zoomMatrix = params->ZoomMatrix;

                cv::Point2d viewportCenter_originalImage = ZoomPanTransform::Apply(
                    zoomMatrix.inv(),
                    cv::Point2d (
                        (double)params->ImageDisplaySize.width / 2.,
                        (double)params->ImageDisplaySize.height / 2.)
                );

                {
                    cv::Point2d zoomCenter = params->WatchedPixels.empty() ?
                                viewportCenter_originalImage
                            :   cv::Point2d(params->WatchedPixels.back());
                    ImGui::PushButtonRepeat(true);
                    if (Icons::IconButton(Icons::IconType::ZoomPlus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1.1);

                    ImGui::SameLine();

                    if (Icons::IconButton(Icons::IconType::ZoomMinus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1. / 1.1);

                    ImGui::PopButtonRepeat();
                }
                ImGui::SameLine();
                // Scale1 & Full View Zoom  buttons
                {
                    auto scaleOneZoomInfo = ZoomPanTransform::MakeScaleOne(image.size(), params->ImageDisplaySize);
                    auto fullViewZoomInfo = ZoomPanTransform::MakeFullView(image.size(), params->ImageDisplaySize);
                    if (Icons::IconButton(
                        Icons::IconType::ZoomScaleOne,
                        ZoomPanTransform::IsEqual(zoomMatrix, scaleOneZoomInfo)) // disabled flag
                        )
                        zoomMatrix = scaleOneZoomInfo;

                    ImGui::SameLine();

                    if (Icons::IconButton(
                        Icons::IconType::ZoomFullView,
                        ZoomPanTransform::IsEqual(zoomMatrix, fullViewZoomInfo)) // disabled flag
                        )
                        zoomMatrix = fullViewZoomInfo;
                }
            }

        };

        //
        // Lambda / Show image
        //
        auto fnShowImage = [&params](CachedImages & cacheImages)
        {
            cv::Point2d mouseLocation = ImageNavigatorWidgets::DisplayTexture_TrackMouse(
                    *cacheImages.GlTexture,
                    ImVec2((float)params->ImageDisplaySize.width, (float)params->ImageDisplaySize.height));

            cv::Point2d mouseLocation_originalImage =
                ImGui::IsItemHovered() ? ZoomPanTransform::Apply(params->ZoomMatrix.inv(), mouseLocation) : cv::Point2d(-1., -1.);
            return mouseLocation_originalImage;
        };


        //
        // Lambda / Show pixel info
        //
        auto fnShowPixelInfo = [&image, &params](const cv::Point2d& mouseLocation)
        {
            cv::Point mouseLoc =
                mouseLocation.x >= 0. ?
                        cv::Point((int)(mouseLocation.x + 0.5), (int)(mouseLocation.y + 0.5))
                    :   cv::Point(-1, -1)
                    ;
            if (mouseLoc.x >= 0)
            {
                ImGui::Text("(%i,%i)", mouseLoc.x, mouseLoc.y);
                ImGui::SameLine();
            }
            ImageNavigatorWidgets::ShowPixelColorWidget(image, mouseLoc, *params);
        };

        //
        // Lambda / Show full Gui
        //
        auto fnShowFullGui = [&](CachedParams& cacheParams, CachedImages &cacheImages)
        {

            ImGui::BeginGroup();
            // Show image
            cv::Point2d mouseLocation_originalImage = fnShowImage(cacheImages);
            // Add Watched Pixel on double click
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
                fnWatchedPixels_Add(mouseLocation_originalImage);

            // Handle Mouse
            fnHandleMouseDragging(cacheParams);
            fnHandleMouseWheel(mouseLocation_originalImage);

            // Zoom+ / Zoom- buttons
            fnShowZoomButtons();
            // adjust button
            {
                if (!params->ShowZoomButtons)
                    ImGui::NewLine();
                ImGuiImm::SameLineAlignRight(20.f, (float)params->ImageDisplaySize.width);
                if (Icons::IconButton(Icons::IconType::AdjustLevels))
                    fnToggleShowOptions();
            }

            // Show infos
            if (params->ShowImageInfo)
                ImageNavigatorWidgets::ShowImageInfo(image, params->ZoomMatrix(0, 0));
            if (params->ShowPixelInfo)
                fnShowPixelInfo(mouseLocation_originalImage);
            ImGui::EndGroup();

            // Show Options
            fnOptionGui(cacheParams);

            return mouseLocation_originalImage;
        };
        auto fnShowFullGui_WithBorder = [&](CachedParams& cacheParams, CachedImages &cacheImages)
        {
            // BeginGroupPanel
            bool drawBorder = params->ShowLegendBorder;
            ImGuiImm::BeginGroupPanel_FlagBorder(fnPanelTitle().c_str(), drawBorder);
            cv::Point2d mouseLocation_originalImage = fnShowFullGui(cacheParams, cacheImages);
            ImGuiImm::EndGroupPanel_FlagBorder();
            return mouseLocation_originalImage;
        };


        /////////////////////////////////////////////////////////////////////////////////////////
        //
        // Core of the function below (there are only lambdas declarations before)
        //
        /////////////////////////////////////////////////////////////////////////////////////////
        if (image.empty())
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f),
                               "%s -> empty image !!!", params->Legend.c_str());
            return cv::Point2d();
        }

        ImageNavigatorCache::gImageNavigatorTextureCache.UpdateCache(image, params, refresh);
        auto &cacheParams = ImageNavigatorCache::gImageNavigatorTextureCache.GetCacheParams(image);
        auto &cacheImages = ImageNavigatorCache::gImageNavigatorTextureCache.GetCacheImages(image);

        ImGui::PushID("##ImageNavigator"); ImGui::PushID(&image);
        cv::Point2d mouseLocation_originalImage = fnShowFullGui_WithBorder(cacheParams, cacheImages);
        ImGui::PopID(); ImGui::PopID();

        return mouseLocation_originalImage;
    }


    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        const cv::Size& imageDisplaySize,
        const std::string& legend,
        bool refreshImage,
        bool showOptionsWhenAppearing,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey
    )
    {
        static std::map<const cv::Mat *, ImageNavigatorParams> s_Params;
        if (s_Params.find(&image) == s_Params.end())
        {
            ImageNavigatorParams params;
            params.ImageDisplaySize = imageDisplaySize;
            params.Legend = legend;
            params.ShowOptions = showOptionsWhenAppearing;
            params.ZoomKey = zoomKey;
            params.ColorAdjustmentsKey = colorAdjustmentsKey;
            s_Params[&image] = params;
        }

        ImageNavigatorParams* params = & s_Params.at(&image);

        return ImageNavigator(image, params, refreshImage);
    }


} // namespace ImmVision