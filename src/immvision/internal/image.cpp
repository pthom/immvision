#include "immvision/internal/stb/stb_image_write.h"
#include "immvision/internal/stb/stb_image.h"

#include "immvision/image.h"
#include "immvision/internal/drawing/internal_icons.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/imgui_imm.h"
#include "immvision/internal/misc/portable_file_dialogs.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/imgui/image_widgets.h"
#include "immvision/internal/image_cache.h"
#include "immvision/internal/misc/panic.h"
#include "immvision/inspector.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <map>
#include <stack>
#include <vector>
#include <optional>
#include <iostream>

// Image saving uses stb_image_write (supports PNG, BMP, JPG, HDR)

#ifndef IMMVISION_VERSION
#define IMMVISION_VERSION "unknown version"
#endif

namespace ImmVision
{
    // ================================ Color Order stuff ================================

    enum class PrivColorOrder
    {
        RGB,
        BGR
    };

    std::stack<PrivColorOrder> sColorOrderStack;

    void PushColorOrderRgb()
    {
        sColorOrderStack.push(PrivColorOrder::RGB);
    }
    void PushColorOrderBgr()
    {
        sColorOrderStack.push(PrivColorOrder::BGR);
    }
    void PopColorOrder()
    {
        if (sColorOrderStack.empty())
        {
            const char* errorMessage = R"(
Error in ImmVision
==================
PopColorOrder() called too many times. The color order stack is empty.

Ensure that each PushColorOrderRgb()/PushColorOrderBgr() call is paired with a PopColorOrder() call.
)";
            fprintf(stderr, "%s", errorMessage);
            throw std::runtime_error(errorMessage);
        }
        sColorOrderStack.pop();
    }


    void UseRgbColorOrder() { PushColorOrderRgb(); }
    void UseBgrColorOrder() { PushColorOrderBgr(); }

    static bool Priv_IsColorOrderBgr()
    {
        const char* errorMessage = R"(
Error in ImmVision
==================
You must set the image color order before displaying images. At the start of your program, call:
    ImmVision::UseRgbColorOrder() or ImmVision::UseBgrColorOrder() (C++)
or
    immvision.use_rgb_color_order() or immvision.use_bgr_color_order() (Python)

This is a required setup step. (Breaking change - October 2024)
)";
        if (sColorOrderStack.empty())
        {
            fprintf(stderr, "%s", errorMessage);
            throw std::runtime_error(errorMessage);
        }
        return sColorOrderStack.top() == PrivColorOrder::BGR;
    }
    bool IsUsingRgbColorOrder()
    {
        return !Priv_IsColorOrderBgr();
    }
    bool IsUsingBgrColorOrder()
    {
        return Priv_IsColorOrderBgr();
    }
    bool IsColorOrderUndefined()
    {
        return sColorOrderStack.empty();
    }


    // ================================ Rest ================================

    // With Image and ImageDisplay we can rely on the ID stack,
    // since calls to Image & ImageDisplay will have a reproducible id stack
    static bool sDoUseIdStack = true;


    void ClearTextureCache()
    {
        ImageCache::gImageTextureCache.ClearImagesCache();
        Icons::ClearIconsTextureCache();
        Inspector_ClearImages();
        Colormap::ClearColormapsTexturesCache();
    }

    static float FontSizeRatio()
    {
        float r = ImGui::GetFontSize() / 14.5;
        return r;
    }

    void Image(const std::string& label, const ImageBuffer& image, ImageParams* params)
    {
        // Note: although this function is long, it is well organized, and it behaves almost like a class
        // with members = (ImageBuffer& image, ImageParams* params).
        //
        // - it begins by defining a set a lambdas that display various widgets
        //   for the different zones of the Gui (those lambdas are named fnXXXX)
        //
        // - the core of the function is only a few lines long and begins after the line
        //    "Core of the function below"
        //
        // If your editor is able to collapse the lambda definitions, you will see the structure.

        using CachedParams = ImageCache::ImageTextureCache::CachedParams;
        using CachedImages = ImageCache::ImageTextureCache::CachedImageAndTexture;

        //
        // Lambda / is Label visible
        //
        auto fnIsLabelVisible = [&label]() -> bool {
            if (label.empty())
                return false;
            if (label.find("##") == 0)
                return false;
            return true;
        };
        //
        // Lambdas / Watched Pixels
        //
        bool wasWatchedPixelAdded = false;
        auto fnWatchedPixels_Add = [&params, &wasWatchedPixelAdded](const Point2d& pixelDouble)
        {
            Point pixel((int)(pixelDouble.x + .5), (int)(pixelDouble.y + .5));
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
                    Point watchedPixel = params->WatchedPixels[i];
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
        auto fnColormap_Gui = [&params, &image](float availableGuiWidth)
        {
            Rect roi = ZoomPanTransform::VisibleRoi(params->ZoomPanMatrix, params->ImageDisplaySize, image.size());
            Colormap::GuiShowColormapSettingsData(
                image,
                roi,
                availableGuiWidth,
                &params->ColormapSettings);
        };


        //
        // Lambdas / Options & Adjustments
        //
        auto fnSaveImage_Gui = [&image, &params](CachedParams & cacheParams, const ImageBuffer& imageWithColormap)
        {
            bool isFloatImage = ImageDepthIsFloat(image.depth);

            // Swap R and B channels in-place for a uint8 3 or 4 channel image
            auto fnSwapRB = [](const ImageBuffer& src) -> ImageBuffer {
                ImageBuffer dst = src.clone();
                int ch = dst.channels;
                for (int y = 0; y < dst.height; y++) {
                    uint8_t* row = dst.ptr<uint8_t>(y);
                    for (int x = 0; x < dst.width; x++)
                        std::swap(row[x * ch], row[x * ch + 2]);
                }
                return dst;
            };

            auto fnGetImageToSave = [&image, &fnSwapRB]() -> ImageBuffer
            {
                // stb_image_write expects RGB order; swap if source is BGR
                if ((image.depth == ImageDepth::uint8 && (image.channels == 3 || image.channels == 4)) && Priv_IsColorOrderBgr())
                    return fnSwapRB(image);
                return image;
            };
            auto fnGetImageWithColorMapToSave = [&imageWithColormap]() {
                // imageWithColormap is always RGBA (internal rendering pipeline outputs RGB order)
                // Drop alpha: create RGB from RGBA
                ImageBuffer rgb = ImageBuffer::Zeros(imageWithColormap.width, imageWithColormap.height, 3, ImageDepth::uint8);
                for (int y = 0; y < imageWithColormap.height; y++) {
                    const uint8_t* src = imageWithColormap.ptr<uint8_t>(y);
                    uint8_t* dst = rgb.ptr<uint8_t>(y);
                    for (int x = 0; x < imageWithColormap.width; x++) {
                        dst[x * 3]     = src[x * 4];
                        dst[x * 3 + 1] = src[x * 4 + 1];
                        dst[x * 3 + 2] = src[x * 4 + 2];
                    }
                }
                return rgb;
            };

            std::string tooltipSaveRawImage =
                "Saves the raw image\n"
                "Specify the format via the filename extension (.jpg, .png, .hdr, etc)\n"
                "\n"
                "- For uint8 3-channel images, use .jpg, .png, or .bmp\n"
                "- For 4 channel images, prefer to use .png\n"
                "- For float images, use .hdr";

            std::string tooltipExportColormap =
                "Export the colormap image as RGB\n"
                "(Hint: use a lossless format, such as .png or .bmp)";

            bool usePortableFileDialogs = pfd::settings::available();

            auto fnSaveImage = [usePortableFileDialogs](const std::string& filename, const ImageBuffer& imageToSave)
            {
                if (filename.empty() || imageToSave.empty())
                    return;

                auto fnReportError = [&](const std::string& msg) {
                    if (usePortableFileDialogs)
                        pfd::message("Error", msg, pfd::choice::ok, pfd::icon::error);
                    else
                        fprintf(stderr, "%s\n", msg.c_str());
                };

                // Only uint8 images can be saved with stb (except HDR which needs float)
                if (imageToSave.depth != ImageDepth::uint8 && imageToSave.depth != ImageDepth::float32)
                {
                    fnReportError("Cannot save image: only uint8 and float32 images are supported.\n"
                                  "Use .hdr for float32 images, or .png/.jpg/.bmp for uint8 images.");
                    return;
                }

                // Detect format from extension
                std::string ext;
                {
                    auto dot = filename.rfind('.');
                    if (dot != std::string::npos)
                    {
                        ext = filename.substr(dot);
                        for (auto& c : ext) c = (char)tolower(c);
                    }
                }

                int w = imageToSave.width, h = imageToSave.height, ch = imageToSave.channels;
                int ok = 0;
                if (ext == ".hdr" && imageToSave.depth == ImageDepth::float32)
                {
                    ok = stbi_write_hdr(filename.c_str(), w, h, ch,
                                        static_cast<const float*>(imageToSave.data));
                }
                else if (imageToSave.depth == ImageDepth::uint8)
                {
                    const uint8_t* data = static_cast<const uint8_t*>(imageToSave.data);
                    int stride = (int)imageToSave.step;
                    if (ext == ".png")
                        ok = stbi_write_png(filename.c_str(), w, h, ch, data, stride);
                    else if (ext == ".bmp")
                        ok = stbi_write_bmp(filename.c_str(), w, h, ch, data);
                    else if (ext == ".jpg" || ext == ".jpeg")
                        ok = stbi_write_jpg(filename.c_str(), w, h, ch, data, 95);
                    else if (ext == ".tga")
                        ok = stbi_write_tga(filename.c_str(), w, h, ch, data);
                    else
                    {
                        fnReportError("Unsupported format: " + ext + "\nSupported: .png, .bmp, .jpg, .tga, .hdr");
                        return;
                    }
                }
                else
                {
                    fnReportError("Use .hdr extension for float32 images");
                    return;
                }

                if (!ok)
                    fnReportError("Could not save image to: " + filename);
            };

            auto fnAskForFilenameWithPfd = []() -> std::string
            {
                pfd::settings::verbose(true);
                std::string filename = pfd::save_file("Select a file", ".",
                                                      { "Image Files", "*.png *.jpg *.jpeg *.jpg *.bmp *.gif *.hdr *.exr",
                                                        "All Files", "*" }).result();
                return filename;
            };
            auto fnAskForFilenameWithImGui = [&cacheParams]() -> std::string
            {
                char *filename = cacheParams.FilenameEditBuffer.data();
                return filename;
            };
            auto fnAskForFilename = [usePortableFileDialogs, fnAskForFilenameWithImGui, fnAskForFilenameWithPfd]() {
                return usePortableFileDialogs ? fnAskForFilenameWithPfd() : fnAskForFilenameWithImGui();
            };
            auto fnInputFilenameWithImGui = [&cacheParams]()
            {
                ImGui::Text("File name");
                char *filename = cacheParams.FilenameEditBuffer.data();
                ImGui::SetNextItemWidth(200.f * FontSizeRatio());
                ImGui::InputText("##filename", filename, 1000);
                ImGui::Text("The image will be saved in the current folder");
            };

            if (!usePortableFileDialogs)
                fnInputFilenameWithImGui();
            // Save image button
            if (ImGuiImm::ButtonWithTooltip("Save image", tooltipSaveRawImage))
                fnSaveImage(fnAskForFilename(), fnGetImageToSave());
            // For float images, give the possibility to save them with the colormap applied
            if (isFloatImage && ImGuiImm::ButtonWithTooltip("Export colormap image", tooltipExportColormap))
                fnSaveImage(fnAskForFilename(), fnGetImageWithColorMapToSave());

        };

        auto fnImageDisplayOptions_Gui = [&params, &image]()
        {
            ImGui::Checkbox("Show school paper background", &params->ShowSchoolPaperBackground);
            if (image.depth == ImageDepth::uint8 && image.channels == 4)
                ImGui::Checkbox("Show alpha channel checkerboard", &params->ShowAlphaChannelCheckerboard);
            if (image.channels > 1)
            {
                ImGui::Text("Channels: ");
                ImGui::RadioButton("All", &params->SelectedChannel, -1); ImGui::SameLine();
                for (int channel_id = 0; channel_id < image.channels; ++channel_id)
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

        };

        auto fnMiscOptions_Gui = [&params]()
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
            ImGui::Checkbox("Resize keep aspect ratio", &params->ResizeKeepAspectRatio);

            ImGui::Separator();
            if (ImGui::Checkbox("Show Options in tooltip window", &params->ShowOptionsInTooltip))
            {
                if (!params->ShowOptionsInTooltip) // We were in a tooltip when clicking
                    params->ShowOptionsPanel = true;
            }
        };

        auto fnOptionsInnerGui = [&params, &image, &fnWatchedPixels_Gui, &wasWatchedPixelAdded,
                                  &fnColormap_Gui, &fnSaveImage_Gui, &fnImageDisplayOptions_Gui, &fnMiscOptions_Gui]
                                      (CachedParams & cacheParams, const ImageBuffer& imageWithColormap)
        {
            float optionsWidth = 260.f * FontSizeRatio();


            // Group with fixed width, so that Collapsing headers stop at optionsWidth
            ImGuiImm::BeginGroupFixedWidth(optionsWidth);

            // Colormap
            if (Colormap::CanColormap(image) && ImageWidgets::CollapsingHeader_OptionalCacheState("Colormap"))
                fnColormap_Gui(optionsWidth);

            // Watched Pixels
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Watched Pixels", wasWatchedPixelAdded))
                fnWatchedPixels_Gui();

            // Image display options
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Image Display"))
                fnImageDisplayOptions_Gui();

            // Misc options
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Misc"))
                fnMiscOptions_Gui();

            // Save Image
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Save"))
                fnSaveImage_Gui(cacheParams, imageWithColormap);

            ImGuiImm::EndGroupFixedWidth();

        };

        auto fnToggleShowOptions = [&params]()
        {
            if (params->ShowOptionsInTooltip)
                ImGui::OpenPopup("Options");
            else
                params->ShowOptionsPanel = !params->ShowOptionsPanel;
        };

        auto fnOptionGui = [&params, &fnOptionsInnerGui](CachedParams & cacheParams, const ImageBuffer& imageWithColormap)
        {
            if (params->ShowOptionsInTooltip)
            {
                if (ImGui::BeginPopup("Options"))
                {
                    fnOptionsInnerGui(cacheParams, imageWithColormap);
                    ImGui::EndPopup();
                }
            }
            else if (params->ShowOptionsPanel)
            {
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("Options");
                fnOptionsInnerGui(cacheParams, imageWithColormap);
                ImGui::EndGroup();
            }
        };

        //
        // Lambdas / Handle Zoom
        //
        // Mouse dragging
        auto fnHandleMouseDragging = [&params](CachedParams & cacheParams)
        {
            if (cacheParams.IsResizing)
                return;
            Matrix33d zoomMatrix = params->ZoomPanMatrix;

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
                    Point2d((double)dragDeltaDelta.x, (double)dragDeltaDelta.y),
                    zoomMatrix(0, 0));
                cacheParams.LastDragDelta = dragDelta;
            }
            params->ZoomPanMatrix = zoomMatrix;
        };
        auto fnHandleMouseWheel = [&params](const Point2d& mouseLocation)
        {
            if (!params->ZoomWithMouseWheel)
                return;
            ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

            if ((fabs(ImGui::GetIO().MouseWheel) > 0.f) && (ImGui::IsItemHovered()))
            {
                double zoomRatio = (double)ImGui::GetIO().MouseWheel / 4.;
                ImGui::GetIO().MouseWheel = 0.f;

                double currentZoom = params->ZoomPanMatrix(0, 0);
                bool isZoomIn = zoomRatio > 0.;

                bool refuseZoom;
                {
                    if (isZoomIn)
                        refuseZoom = currentZoom > 5000.;
                    else
                        refuseZoom = currentZoom < 0.005;
                }
                if (refuseZoom)
                    return;
                params->ZoomPanMatrix = params->ZoomPanMatrix * ZoomPanTransform::ComputeZoomMatrix(mouseLocation, exp(zoomRatio));
            }
        };
        auto fnShowZoomButtons = [&params, &image]()
        {
            if (params->ShowZoomButtons)
            {
                Matrix33d zoomMatrix = params->ZoomPanMatrix;

                Point2d viewportCenter_originalImage = ZoomPanTransform::Apply(
                    zoomMatrix.inv(),
                    Point2d(
                        (double)params->ImageDisplaySize.width / 2.,
                        (double)params->ImageDisplaySize.height / 2.)
                );

                {
                    Point2d zoomCenter = params->WatchedPixels.empty() ?
                                viewportCenter_originalImage
                            :   Point2d(params->WatchedPixels.back());
                    ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
                    if (Icons::IconButton(Icons::IconType::ZoomPlus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1.1);

                    ImGui::SameLine();

                    if (Icons::IconButton(Icons::IconType::ZoomMinus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1. / 1.1);

                    ImGui::PopItemFlag();
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
                params->ZoomPanMatrix = zoomMatrix;
            }

        };
        //
        // Lambda / Show image
        //
        auto fnShowImage = [&params](const GlTexture& glTexture) ->  MouseInformation
        {
            bool disableDragWindow = params->PanWithMouse;
            Point2d mouseLocation = ImageWidgets::DisplayTexture_TrackMouse(
                    glTexture,
                    ImVec2((float)params->ImageDisplaySize.width, (float)params->ImageDisplaySize.height), disableDragWindow);

            MouseInformation mouseInfo;
            if (ImGui::IsItemHovered())
            {
                mouseInfo.IsMouseHovering = true;
                mouseInfo.MousePosition = ZoomPanTransform::Apply(params->ZoomPanMatrix.inv(), mouseLocation);
                mouseInfo.MousePosition_Displayed = Point((int)mouseLocation.x, (int)mouseLocation.y);
            }
            return mouseInfo;
        };

        //
        // Lambda / Show resize widget in the bottom right corner
        //
        auto fnShowResizeWidget = [&params, &image](CachedParams & cacheParams)
        {
            if (!params->CanResize)
                return;
            ImVec2 imageBottomRight = ImGui::GetItemRectMax();
            float em = ImGui::GetFontSize();
            float size = em * 1.0f;
            ImVec2 br(imageBottomRight.x, imageBottomRight.y);
            ImVec2 bl(br.x - size, br.y);
            ImVec2 tr(br.x, br.y - size);
            ImVec2 tl(br.x - size, br.y - size);

            ImRect zone(tl, br);

            if (!params->PanWithMouse)
            {
                auto cursorPos = ImGui::GetCursorScreenPos();
                ImGui::SetCursorScreenPos(tl);
                ImGui::InvisibleButton("##resize", zone.GetSize());
                ImGui::SetCursorScreenPos(cursorPos);
                ImGui::Dummy(ImVec2(0.f, 0.f));
            }

            bool isMouseHoveringWidget = ImGui::IsMouseHoveringRect(zone.Min, zone.Max);
            ImU32 color = isMouseHoveringWidget ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) : ImGui::GetColorU32(ImGuiCol_Button);
            ImGui::GetWindowDrawList()->AddTriangleFilled(br, bl, tr, color);

            if (!cacheParams.IsResizing)
            {
                if (isMouseHoveringWidget && ImGui::IsMouseDown(0))
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
                    cacheParams.IsResizing = true;
                }
            }
            if (cacheParams.IsResizing)
            {
                if (ImGui::IsMouseDown(0))
                {
                    if (ImGui::GetIO().MouseDelta.x != 0. || ImGui::GetIO().MouseDelta.y != 0.)
                    {
                        params->ImageDisplaySize.width += (int)ImGui::GetIO().MouseDelta.x;
                        params->ImageDisplaySize.height += (int)ImGui::GetIO().MouseDelta.y;

                        if (params->ImageDisplaySize.width < 5)
                            params->ImageDisplaySize.width = 5;
                        if (params->ImageDisplaySize.height < 5)
                            params->ImageDisplaySize.height = 5;

                        if (params->ResizeKeepAspectRatio)
                        {
                            float imageDisplayRatio = (float)params->ImageDisplaySize.width / (float)params->ImageDisplaySize.height;
                            float imageRatio = (float)image.width / (float)image.height;
                            if (imageDisplayRatio > imageRatio)
                                params->ImageDisplaySize.width = (int)((float)params->ImageDisplaySize.height * imageRatio);
                            else
                                params->ImageDisplaySize.height = (int)((float)params->ImageDisplaySize.width / imageRatio);
                        }
                    }
                }
                else
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
                    cacheParams.IsResizing = false;
                }
            }
        };


        //
        // Lambda / Show pixel info
        //
        auto fnShowPixelInfo = [&image, &params](const Point2d& mouseLocation)
        {
            Point mouseLoc =
                mouseLocation.x >= 0. ?
                        Point((int)(mouseLocation.x + 0.5), (int)(mouseLocation.y + 0.5))
                    :   Point(-1, -1)
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
            auto mouseInfo = fnShowImage(*cacheImages.mGlTexture);
            fnShowResizeWidget(cacheParams);
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
            fnOptionGui(cacheParams, cacheImages.mImageRgbaCache);

            return mouseInfo;
        };
        auto fnShowFullGui_WithBorder = [&](CachedParams& cacheParams, CachedImages &cacheImages) -> MouseInformation
        {
            // BeginGroupPanel
            bool drawBorder =  fnIsLabelVisible();
            std::string title = label + "##title";
            if (drawBorder)
                ImGuiImm::BeginGroupPanel_FlagBorder(title.c_str(), drawBorder);
            else
                ImGui::BeginGroup();
            auto mouseInfo = fnShowFullGui(cacheParams, cacheImages);
            if (drawBorder)
                ImGuiImm::EndGroupPanel_FlagBorder();
            else
                ImGui::EndGroup();
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
                               "%s -> empty image !!!", label.c_str());
            params->MouseInfo = MouseInformation();
            return;
        }

        ImGui::PushID(label.c_str());
        try
        {
            auto id = ImageCache::gImageTextureCache.GetID(label, sDoUseIdStack);
            bool isNewImage = ImageCache::gImageTextureCache.UpdateCache(id, image, params, params->RefreshImage);
            auto &cacheParams = ImageCache::gImageTextureCache.GetCacheParams(id);
            auto &cacheImages = ImageCache::gImageTextureCache.GetCacheImageAndTexture(id);
            params->MouseInfo = fnShowFullGui_WithBorder(cacheParams, cacheImages);

            // Handle Colormap
            Rect roi = ZoomPanTransform::VisibleRoi(params->ZoomPanMatrix, params->ImageDisplaySize, image.size());
            if (isNewImage || params->RefreshImage)
                Colormap::InitStatsOnNewImage(image, roi, &params->ColormapSettings);
            if (! ZoomPanTransform::IsEqual(cacheParams.PreviousParams.ZoomPanMatrix, params->ZoomPanMatrix))
                Colormap::UpdateRoiStatsInteractively(image, roi, &params->ColormapSettings);
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


    Point2d ImageDisplay(
        const std::string& label_id,
        const ImageBuffer& image,
        const Size& imageDisplaySize,
        bool refreshImage,
        bool showOptionsButton
        )
    {
        ImGuiID id = ImGui::GetID(label_id.c_str());
        static std::map<ImGuiID, ImageParams> s_Params;
        if (s_Params.find(id) == s_Params.end())
        {
            ImageParams params = showOptionsButton ? ImageParams() : FactorImageParamsDisplayOnly();
            s_Params[id] = params;
        }

        ImageParams& params = s_Params.at(id);
        {
            params.ShowOptionsButton = showOptionsButton;
            params.ImageDisplaySize = imageDisplaySize;
            params.RefreshImage = refreshImage;

            Size displayedSize = ImGuiImm::ComputeDisplayImageSize(imageDisplaySize, image.size());
            params.ZoomPanMatrix = ZoomPanTransform::MakeFullView(image.size(), displayedSize);
        }

        Image(label_id, image, &params);
        return params.MouseInfo.MousePosition;
    }


    static std::map<ImGuiID, ImVec2> s_ImageDisplayResizable_Sizes;

    IMMVISION_API Point2d ImageDisplayResizable(
        const std::string& label_id,
        const ImageBuffer& image,
        ImVec2* size,
        bool refreshImage,
        bool resizable,
        bool showOptionsButton
    )
    {
        if (size == nullptr)
        {
            ImGuiID id = ImGui::GetID(label_id.c_str());
            if (s_ImageDisplayResizable_Sizes.find(id) == s_ImageDisplayResizable_Sizes.end())
                s_ImageDisplayResizable_Sizes[id] = ImVec2(0, 0);
            size = &s_ImageDisplayResizable_Sizes[id];
        }

        ImGuiID id = ImGui::GetID(label_id.c_str());
        static std::map<ImGuiID, ImageParams> s_Params;
        if (s_Params.find(id) == s_Params.end())
        {
            ImageParams params = showOptionsButton ? ImageParams() : FactorImageParamsDisplayOnly();
            s_Params[id] = params;
        }

        Size imageDisplaySize = Size((int)size->x, (int)size->y);
        ImageParams& params = s_Params.at(id);
        {
            params.ShowOptionsButton = showOptionsButton;
            params.ImageDisplaySize = imageDisplaySize;
            params.CanResize = resizable;
            params.RefreshImage = refreshImage;

            Size displayedSize = ImGuiImm::ComputeDisplayImageSize(imageDisplaySize, image.size());
            params.ZoomPanMatrix = ZoomPanTransform::MakeFullView(image.size(), displayedSize);
        }
        std::string hiddenLabel = std::string("##") + label_id;
        Image(hiddenLabel, image, &params);

        *size = ImVec2((float)params.ImageDisplaySize.width, (float)params.ImageDisplaySize.height);
        return params.MouseInfo.MousePosition;
    }


    ImageParams FactorImageParamsDisplayOnly()
    {
        ImageParams imageParams;
        {
            imageParams.ShowOptionsButton = false;
            imageParams.ShowOptionsPanel = false;
            imageParams.ZoomWithMouseWheel = false;
            imageParams.PanWithMouse = false;
            imageParams.CanResize = false;
            imageParams.ResizeKeepAspectRatio = true;
            imageParams.ShowPixelInfo = false;
            imageParams.ShowImageInfo = false;
            imageParams.ShowGrid = false;
            imageParams.ShowAlphaChannelCheckerboard = false;
            imageParams.ShowSchoolPaperBackground = false;
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


    ImageBuffer GetCachedRgbaImage(const std::string& label)
    {
        auto id = ImageCache::gImageTextureCache.GetID(label, sDoUseIdStack);
        return ImageCache::gImageTextureCache.GetCacheImageAndTexture(id).mImageRgbaCache;
    }

    ImageParams::~ImageParams()
    {
        if (ImageCache::gImageTextureCacheAlive)
            ImageCache::gImageTextureCache.OnDeleteImageParams(this);
    }

    ImageBuffer ImRead(const std::string& filename)
    {
        int w, h, channels;
        // Try HDR first
        if (stbi_is_hdr(filename.c_str()))
        {
            float* data = stbi_loadf(filename.c_str(), &w, &h, &channels, 0);
            if (!data)
                return ImageBuffer();
            ImageBuffer buf = ImageBuffer::Zeros(w, h, channels, ImageDepth::float32);
            std::memcpy(buf.data, data, (size_t)w * h * channels * sizeof(float));
            stbi_image_free(data);
            return buf;
        }
        // Standard uint8 image
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &channels, 0);
        if (!data)
            return ImageBuffer();
        ImageBuffer buf = ImageBuffer::Zeros(w, h, channels, ImageDepth::uint8);
        std::memcpy(buf.data, data, (size_t)w * h * channels);
        stbi_image_free(data);
        return buf;
    }

} // namespace ImmVision
