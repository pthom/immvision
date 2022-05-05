#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "immvision/image.h"
#include "immvision/internal/drawing/internal_icons.h"
#include "immvision/imgui_imm.h"
#include "immvision/internal/misc/portable_file_dialogs.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/cv/color_adjustment_utils.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/imgui/image_widgets.h"
#include "immvision/internal/image_cache.h"
#include "immvision/internal/misc/panic.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <map>
#include <vector>
#include <iostream>

namespace ImmVision
{
    void ClearTextureCache()
    {
        ImageCache::gImageTextureCache.ClearImagesCache();
        Icons::ClearIconsTextureCache();
    }


    void Image(const std::string& label_id, const cv::Mat& image, ImageParams* params)
    {
        // Note: although this function is long, it is well organized, and it behaves almost like a class
        // with members = (cv::Mat& image, ImageParams* params).
        //
        // - it begins by defining a set a lambdas that display various widgets
        //   for the different zones of the Gui (those lambdas are named fnXXXX)
        //
        // - the core of the function is only a few lines long and begins after the line
        //    "Core of the function below"
        //
        // If your editor is able to collapse the lambda definitions, you will see the structure.

        using CachedParams = ImageCache::ImageTextureCache::CachedParams;
        using CachedImages = ImageCache::ImageTextureCache::CachedImages;

        //
        // Lambda / is Label visible
        //
        auto fnIsLabelVisible = [&label_id]() -> bool {
            if (label_id.empty())
                return false;
            if (label_id.find("##") == 0)
                return false;
            return true;
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
                params->ShowOptionsPanel = true;
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
                    ImageWidgets::ShowPixelColorWidget(image, watchedPixel, *params);

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

            ImGui::Checkbox("Add Watched Pixel on double click", &params->AddWatchedPixelOnDoubleClick);
            ImGui::Checkbox("Highlight Watched Pixels", &params->HighlightWatchedPixels);
        };

        //
        // Lambdas / Colormap
        //
        auto fnColormap = [&params, &image]()
        {
            cv::Rect roi;
            Colormap::ShowColormapsGui(
                image,
                roi,
                & params->ColorAdjustments);
        };


        //
        // Lambdas / Options & Adjustments
        //
        auto fnOptionsInnerGui = [&params, &image, &fnWatchedPixels_Gui, &wasWatchedPixelAdded, &fnColormap](
                CachedParams & cacheParams)
        {
            float optionsWidth = 330.f;
            // Group with fixed width, so that Collapsing headers stop at optionsWidth
            ImGuiImm::BeginGroupFixedWidth(optionsWidth);

            // Colormap
            if (Colormap::CanColormap(image) && ImageWidgets::CollapsingHeader_OptionalCacheState("Colormap"))
                fnColormap();


            // Adjust float values
            bool hasAdjustFloatValues = true; // ((image.depth() == CV_32F) || (image.depth() == CV_64F));

            if (hasAdjustFloatValues && ImageWidgets::CollapsingHeader_OptionalCacheState("Adjust"))
            {
                ImGui::PushItemWidth(200.);
                ImGuiImm::SliderAnyFloatLogarithmic(
                    "k", &params->ColorAdjustments.Factor,
                    0.001, 32.);
                ImGui::PushItemWidth(200.);
                ImGuiImm::SliderAnyFloatLogarithmic(
                    "Delta", &params->ColorAdjustments.Delta,
                    -255., 255.);

                if (ImGui::Button("Default"))
                    params->ColorAdjustments = ColorAdjustmentsUtils::ComputeInitialImageAdjustments(image);
                ImGui::SameLine();
                 if (ImGui::Button("No Adjustment"))
                        params->ColorAdjustments = ColorAdjustmentsValues();
            }

            // Watched Pixels
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Watched Pixels", wasWatchedPixelAdded))
                fnWatchedPixels_Gui();

            // Image display options
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Image Display"))
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

            // Image display options
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Options"))
            {
                {
                    ImGuiImm::BeginGroupPanel("Image display options");
                    ImGui::Checkbox("Show image info", &params->ShowImageInfo);
                    ImGui::Checkbox("Show pixel info", &params->ShowPixelInfo);
                    ImGui::Checkbox("Show zoom buttons", &params->ShowZoomButtons);
                    ImGuiImm::EndGroupPanel();
                }

                ImGui::Checkbox("Pan with mouse", &params->PanWithMouse);
                ImGui::Checkbox("Zoom with mouse wheel", &params->ZoomWithMouseWheel);

                ImGui::Separator();
                if (ImGui::Checkbox("Show Options in tooltip window", &params->ShowOptionsInTooltip))
                {
                    if (!params->ShowOptionsInTooltip) // We were in a tooltip when clicking
                        params->ShowOptionsPanel = true;
                }
            }

            // Save Image
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Save"))
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
                params->ShowOptionsPanel = !params->ShowOptionsPanel;
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
            else if (params->ShowOptionsPanel)
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
            ZoomPanTransform::MatrixType& zoomMatrix = params->ZoomPanMatrix;

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
                params->ZoomPanMatrix = params->ZoomPanMatrix * ZoomPanTransform::ComputeZoomMatrix(mouseLocation, exp(zoomRatio));
            }
        };
        auto fnShowZoomButtons = [&params, &image]()
        {
            if (params->ShowZoomButtons)
            {
                ZoomPanTransform::MatrixType& zoomMatrix = params->ZoomPanMatrix;

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
        auto fnShowImage = [&params](CachedImages & cacheImages) ->  MouseInformation
        {
            cv::Point2d mouseLocation = ImageWidgets::DisplayTexture_TrackMouse(
                    *cacheImages.GlTexture,
                    ImVec2((float)params->ImageDisplaySize.width, (float)params->ImageDisplaySize.height));

            MouseInformation mouseInfo;
            if (ImGui::IsItemHovered())
            {
                mouseInfo.IsMouseHovering = true;
                mouseInfo.MousePosition = ZoomPanTransform::Apply(params->ZoomPanMatrix.inv(), mouseLocation);
                mouseInfo.MousePosition_Displayed = mouseLocation;
            }
            return mouseInfo;
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
            ImageWidgets::ShowPixelColorWidget(image, mouseLoc, *params);
        };

        //
        // Lambda / Show full Gui
        //
        auto fnShowFullGui = [&](CachedParams& cacheParams, CachedImages &cacheImages) -> MouseInformation
        {

            ImGui::BeginGroup();
            // Show image
            auto mouseInfo = fnShowImage(cacheImages);
            // Add Watched Pixel on double click
            if (   params->AddWatchedPixelOnDoubleClick
                && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
                && ImGui::IsItemHovered())
                fnWatchedPixels_Add(mouseInfo.MousePosition);

            // Handle Mouse
            fnHandleMouseDragging(cacheParams);
            fnHandleMouseWheel(mouseInfo.MousePosition);

            // Zoom+ / Zoom- buttons
            fnShowZoomButtons();
            // adjust button
            if (params->ShowOptionsButton)
            {
                if (!params->ShowZoomButtons)
                    ImGui::NewLine();
                ImGuiImm::SameLineAlignRight(20.f, (float)params->ImageDisplaySize.width);
                if (Icons::IconButton(Icons::IconType::AdjustLevels))
                    fnToggleShowOptions();
            }

            // Show infos
            if (params->ShowImageInfo)
                ImageWidgets::ShowImageInfo(image, params->ZoomPanMatrix(0, 0));
            if (params->ShowPixelInfo)
                fnShowPixelInfo(mouseInfo.MousePosition);
            ImGui::EndGroup();

            // Show Options
            fnOptionGui(cacheParams);

            return mouseInfo;
        };
        auto fnShowFullGui_WithBorder = [&](CachedParams& cacheParams, CachedImages &cacheImages) -> MouseInformation
        {
            // BeginGroupPanel
            bool drawBorder =  fnIsLabelVisible();
            std::string title = label_id + "##title";
            ImGuiImm::BeginGroupPanel_FlagBorder(title.c_str(), drawBorder);
            auto mouseInfo = fnShowFullGui(cacheParams, cacheImages);
            ImGuiImm::EndGroupPanel_FlagBorder();
            return mouseInfo;
        };


        /////////////////////////////////////////////////////////////////////////////////////////
        //
        // Core of the function below (there are only lambdas declarations before)
        //
        /////////////////////////////////////////////////////////////////////////////////////////
        if (image.empty())
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f),
                               "%s -> empty image !!!", label_id.c_str());
            params->MouseInfo = MouseInformation();
            return;
        }

        ImGui::PushID(label_id.c_str());
        try
        {
            ImageCache::gImageTextureCache.UpdateCache(label_id, image, params, params->RefreshImage);
            auto &cacheParams = ImageCache::gImageTextureCache.GetCacheParams(label_id);
            auto &cacheImages = ImageCache::gImageTextureCache.GetCacheImages(label_id);
            params->MouseInfo = fnShowFullGui_WithBorder(cacheParams, cacheImages);
        }
        catch(std::exception& e)
        {
            Panic(e);
        }
        catch(...)
        {
            Panic_UnknownCause();
        }
        ImGui::PopID();
    }


    cv::Point2d ImageDisplay(
        const std::string& label_id,
        const cv::Mat& mat,
        const cv::Size& imageDisplaySize,
        bool refreshImage,
        bool showOptionsButton,
        bool isBgrOrBgra)
    {
        static std::map<const cv::Mat *, ImageParams> s_Params;
        if (s_Params.find(&mat) == s_Params.end())
        {
            ImageParams params = showOptionsButton ? ImageParams() : FactorImageParamsDisplayOnly();
            s_Params[&mat] = params;
        }

        ImageParams& cached_params = s_Params.at(&mat);
        {
            cached_params.ShowOptionsButton = showOptionsButton;
            cached_params.ImageDisplaySize = imageDisplaySize;
            cached_params.RefreshImage = refreshImage;
            cached_params.IsColorOrderBGR = isBgrOrBgra;
        }

        Image(label_id, mat, &cached_params);
        return cached_params.MouseInfo.MousePosition;
    }


    ImageParams FactorImageParamsDisplayOnly()
    {
        ImageParams imageParams;
        {
            imageParams.ShowOptionsButton = false;
            imageParams.ShowOptionsPanel = false;
            imageParams.ZoomWithMouseWheel = false;
            imageParams.PanWithMouse = false;
            imageParams.ShowPixelInfo = false;
            imageParams.ShowImageInfo = false;
            imageParams.ShowGrid = false;
            imageParams.ShowAlphaChannelCheckerboard = false;
            imageParams.ShowZoomButtons = false;
            imageParams.AddWatchedPixelOnDoubleClick = false;
        }
        return imageParams;
    }


    std::string VersionInfo()
    {
        char msg[2000];
        snprintf(msg, 2000, "immvision version %s (%s)", IMMVISION_VERSION, __TIMESTAMP__);
        return msg;
    }

    std::vector<std::string> AvailableColormaps()
    {
        return Colormap::AvailableColormaps();
    }

} // namespace ImmVision
