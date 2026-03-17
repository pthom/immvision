#include "immvision/internal/drawing/image_drawing.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/cv/drawing_utils.h"
#include "immvision/internal/cv/matrix_info_utils.h"
#include "immvision/internal/cv/colormap.h"

#include <cassert>
#include <cstring>  // memcpy
#include <cmath>
#include <sstream>
#include <algorithm> // std::count


namespace ImmVision
{
    namespace ImageDrawing
    {
        static float _ImmDrawingFontScaleRatio()
        {
            if (ImGui::GetFontSize() < 20.f)
                return 1.f;
            else
                return 1.5f;
        }

        ImageBuffer DrawWatchedPixels(const ImageBuffer& image, const ImageParams& params)
        {
            ImageBuffer r = image.clone();

            std::vector<std::pair<size_t, Point2d>> visiblePixels;
            {
                for (size_t i = 0; i < params.WatchedPixels.size(); ++i)
                {
                    Point w = params.WatchedPixels[i];
                    Point2d p = ZoomPanTransform::Apply(params.ZoomPanMatrix, Point2d(w));
                    if (Rect(Point(0, 0), params.ImageDisplaySize).contains(Point((int)p.x, (int)p.y)))
                        visiblePixels.push_back({i, p});
                }
            }

            for (const auto& kv : visiblePixels)
            {
                DrawingUtils::draw_named_feature(
                    r,         // img
                    kv.second, // position,
                    std::to_string(kv.first),       // name
                    Color4d(255, 255, 255, 255), // color
                    true, // add_cartouche
                    4.,   // size
                    2.5,  // size_hole
                    1,    // thickness
                    0.4 * (double) _ImmDrawingFontScaleRatio() // font size
                );
            }

            return r;
        }

        void DrawGrid(ImageBuffer& inOutImageRgba, const ImageParams& params)
        {
            double x_spacing = (double) params.ZoomPanMatrix(0, 0);
            double y_spacing = (double) params.ZoomPanMatrix(1, 1);

            double x_start, y_start;
            {
                Point2d origin_unzoomed = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(), Point2d(0., 0.));
                origin_unzoomed = Point2d(std::floor(origin_unzoomed.x) + 0.5, std::floor(origin_unzoomed.y) + 0.5);
                Point2d origin_zoomed = ZoomPanTransform::Apply(params.ZoomPanMatrix, origin_unzoomed);
                x_start = origin_zoomed.x;
                y_start = origin_zoomed.y;
            }
            double x_end = (double)inOutImageRgba.width - 1.;
            double y_end = (double)inOutImageRgba.height - 1.;

            auto lineColor = Color4d(255, 255, 0, 255);
            double alpha = 0.3;
            DrawingUtils::draw_grid(inOutImageRgba, lineColor, alpha, x_spacing, y_spacing, x_start, y_start, x_end, y_end);
        }

        ImageBuffer DrawValuesOnZoomedPixels(const ImageBuffer& drawingImage, const ImageBuffer& valuesImage,
                                         const ImageParams& params, bool drawPixelCoords)
        {
            assert(drawingImage.depth == ImageDepth::uint8 && drawingImage.channels == 4);

            ImageBuffer r = drawingImage;
            Point tl, br;
            {
                Point2d tld = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(), Point2d(0., 0.));
                Point2d brd = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(),
                                                          Point2d((double)params.ImageDisplaySize.width,
                                                                      (double)params.ImageDisplaySize.height));
                tl = { (int)std::floor(tld.x), (int)std::floor(tld.y) };
                br = { (int)std::ceil(brd.x), (int)std::ceil(brd.y) };
            }

            for (int x = tl.x; x <= br.x; x+= 1)
            {
                for (int y = tl.y; y <= br.y; y+= 1)
                {
                    std::string pixelInfo = MatrixInfoUtils::MatPixelColorInfo(valuesImage, x, y, '\n', false);
                    if (drawPixelCoords)
                        pixelInfo = std::string("x:") + std::to_string(x) + "\n" + "y:" + std::to_string(y) + "\n" + pixelInfo;

                    Point2d position = ZoomPanTransform::Apply(params.ZoomPanMatrix, Point2d((double)x, (double )y));

                    Color4d textColor;
                    {
                        Color4d white(255, 255, 255, 255);
                        Color4d black(0, 0, 0, 255);
                        uint8_t bgR = 0, bgG = 0, bgB = 0;
                        if (Rect(Point(0, 0), drawingImage.size()).contains(Point((int)position.x, (int)position.y)))
                        {
                            const uint8_t* bgPixel = drawingImage.ptr<uint8_t>((int)position.y) + (int)position.x * 4;
                            bgR = bgPixel[0]; bgG = bgPixel[1]; bgB = bgPixel[2];
                        }
                        double luminance = bgR * 0.2126 + bgG * 0.7152 + bgB * 0.0722;
                        if (luminance > 170.)
                            textColor = black;
                        else
                            textColor = white;
                    }
                    DrawingUtils::text(
                        r,
                        position,
                        pixelInfo,
                        textColor,
                        true, // center_around_point
                        false, // add_cartouche
                        0.42 * (double) _ImmDrawingFontScaleRatio() ,  //fontScale
                        1     //int thickness
                    );
                }
            }
            return r;
        };


        ImageBuffer MakeSchoolPaperBackground(Size s)
        {
            ImageBuffer mat = ImageBuffer::Zeros(s.width, s.height, 4, ImageDepth::uint8);

            Color4d paperColor(205, 215, 220, 255);
            Color4d lineColor(199, 196, 184, 255);
            mat.fill(paperColor);
            int quadSize = 17;
            for (int y = 0; y < s.height; y+= quadSize)
            {
                uint8_t* linePtr = mat.ptr<uint8_t>(y);
                for (int x = 0; x < s.width; ++x)
                {
                    uint8_t* p = linePtr + x * 4;
                    p[0] = (uint8_t)lineColor[0]; p[1] = (uint8_t)lineColor[1];
                    p[2] = (uint8_t)lineColor[2]; p[3] = (uint8_t)lineColor[3];
                }
            }
            for (int y = 0; y < s.height; y++)
            {
                uint8_t* linePtr = mat.ptr<uint8_t>(y);
                for (int x = 0; x < s.width; x+=quadSize)
                {
                    uint8_t* p = linePtr + x * 4;
                    p[0] = (uint8_t)lineColor[0]; p[1] = (uint8_t)lineColor[1];
                    p[2] = (uint8_t)lineColor[2]; p[3] = (uint8_t)lineColor[3];
                }
            }
            return mat;
        }

        void BlitImageTexture(
            const ImageParams& params,
            const ImageBuffer& image,
            ImageBuffer& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTexture* outTexture
        )
        {
            if (image.empty())
                return;

            ImageBuffer finalImage = image.clone();

            //
            // Adjustements needed before conversion to rgba
            //
            auto fnSelectChannel = [&finalImage, params]()
            {
                // Extract a single channel from a multi-channel image
                if (finalImage.channels > 1 && (params.SelectedChannel >= 0) && (params.SelectedChannel < finalImage.channels))
                {
                    int ch = params.SelectedChannel;
                    int nch = finalImage.channels;
                    ImageBuffer singleChannel = ImageBuffer::Zeros(finalImage.width, finalImage.height, 1, finalImage.depth);
                    for (int y = 0; y < finalImage.height; y++)
                    {
                        const uint8_t* src = finalImage.ptr<uint8_t>(y);
                        uint8_t* dst = singleChannel.ptr<uint8_t>(y);
                        int elemSize = (int)finalImage.elemSize();
                        for (int x = 0; x < finalImage.width; x++)
                            std::memcpy(dst + x * elemSize, src + (x * nch + ch) * elemSize, elemSize);
                    }
                    finalImage = singleChannel;
                }

            };
            auto fnAlphaCheckerboard = [&finalImage, params]()
            {
                if ((finalImage.depth == ImageDepth::uint8 && finalImage.channels == 4) && params.ShowAlphaChannelCheckerboard)
                {
                    ImageBuffer background = DrawingUtils::make_alpha_channel_checkerboard_image(finalImage.size());
                    finalImage = DrawingUtils::overlay_alpha_image_precise(background, finalImage, 1.);
                }
            };
            auto fnMakeBackground = [&params]() -> ImageBuffer
            {
                if (params.ShowSchoolPaperBackground)
                    return MakeSchoolPaperBackground(params.ImageDisplaySize);
                else
                {
                    ImageBuffer m = ImageBuffer::Zeros(params.ImageDisplaySize.width, params.ImageDisplaySize.height, 4, ImageDepth::uint8);
                    m.fill(Color4d(0, 0, 0, 255));
                    return m;
                }

            };

            //
            // Convert to rgba with adjustments if needed
            //
            if (shall_refresh_rgba)
            {
                if (HasColormapParam(params) && Colormap::CanColormap(image))
                    finalImage = Colormap::ApplyColormap(finalImage, params.ColormapSettings); // returns a rgba image
                else
                {
                    fnSelectChannel();
                    fnAlphaCheckerboard();
                    bool is_color_order_bgr = IsUsingBgrColorOrder();
                    finalImage = DrawingUtils::converted_to_rgba_image(finalImage, is_color_order_bgr);
                }
                in_out_rgba_image_cache = finalImage;
                assert(finalImage.depth == ImageDepth::uint8 && finalImage.channels == 4);
            }
            else
            {
                finalImage = in_out_rgba_image_cache;
                assert(finalImage.depth == ImageDepth::uint8 && finalImage.channels == 4);
                assert(!finalImage.empty());
            }

            double gridMinZoomFactor = 12.;
            double zoomFactor = (double)params.ZoomPanMatrix(0, 0);

            //
            // Zoom
            //
            {
                ZoomPanTransform::WarpInterp warpInterp;
                if (zoomFactor < 1.0)
                    warpInterp = ZoomPanTransform::WarpInterp::Area;
                else if (zoomFactor < gridMinZoomFactor)
                    warpInterp = ZoomPanTransform::WarpInterp::Bilinear;
                else
                    warpInterp = ZoomPanTransform::WarpInterp::Nearest;

                ImageBuffer backgroundWithImage = fnMakeBackground();

                ZoomPanTransform::WarpAffineScaleTranslate(
                    finalImage, backgroundWithImage,
                    params.ZoomPanMatrix,
                    warpInterp);
                finalImage = backgroundWithImage;
            }

            //
            // Drawings on final image
            //
            {
                // Draw grid
                if (params.ShowGrid && zoomFactor >= gridMinZoomFactor)
                    DrawGrid(finalImage, params);

                // Draw Pixel Values
                double drawPixelvaluesMinZoomFactor = (image.depth == ImageDepth::uint8) ? 36. : 48.;
                if (params.DrawValuesOnZoomedPixels && zoomFactor > drawPixelvaluesMinZoomFactor)
                {
                    double drawPixelCoordsMinZoomFactor = 80.;
                    bool drawPixelCoords = zoomFactor > drawPixelCoordsMinZoomFactor;
                    finalImage = DrawValuesOnZoomedPixels(finalImage, image, params, drawPixelCoords);
                }

                // Draw Watched Pixels
                if (params.HighlightWatchedPixels && (! params.WatchedPixels.empty()))
                    finalImage = DrawWatchedPixels(finalImage, params);

            }

            //
            // Blit
            //
            outTexture->UpdateFromImage(finalImage, false);
        }

        void UpdateImageTexture(
            const ImageParams& params,
            const ImageBuffer& image,
            ImageBuffer& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTexture* outTexture
        )
        {
            if (image.empty())
                return;

            if (shall_refresh_rgba)
            {
                ImageBuffer finalImage = image.clone();

                if (HasColormapParam(params) && Colormap::CanColormap(image))
                {
                    finalImage = Colormap::ApplyColormap(finalImage, params.ColormapSettings);
                }
                else
                {
                    // Extract a single channel from a multi-channel image
                    if (finalImage.channels > 1 && (params.SelectedChannel >= 0) && (params.SelectedChannel < finalImage.channels))
                    {
                        int ch = params.SelectedChannel;
                        int nch = finalImage.channels;
                        ImageBuffer singleChannel = ImageBuffer::Zeros(finalImage.width, finalImage.height, 1, finalImage.depth);
                        for (int y = 0; y < finalImage.height; y++)
                        {
                            const uint8_t* src = finalImage.ptr<uint8_t>(y);
                            uint8_t* dst = singleChannel.ptr<uint8_t>(y);
                            int elemSize = (int)finalImage.elemSize();
                            for (int x = 0; x < finalImage.width; x++)
                                std::memcpy(dst + x * elemSize, src + (x * nch + ch) * elemSize, elemSize);
                        }
                        finalImage = singleChannel;
                    }

                    bool is_color_order_bgr = IsUsingBgrColorOrder();
                    finalImage = DrawingUtils::converted_to_rgba_image(finalImage, is_color_order_bgr);
                }
                in_out_rgba_image_cache = finalImage;
                assert(finalImage.depth == ImageDepth::uint8 && finalImage.channels == 4);

                // Upload to GPU texture (with mipmap generation via gl_provider)
                outTexture->UpdateFromImage(finalImage, false);
            }
        }

        // Convert image pixel coordinates to screen position using the zoom/pan matrix
        static ImVec2 ImageToScreen(double imgX, double imgY, const Matrix33d& zoomPan, ImVec2 widgetTopLeft)
        {
            return ImVec2(
                widgetTopLeft.x + (float)(zoomPan(0, 0) * imgX + zoomPan(0, 2)),
                widgetTopLeft.y + (float)(zoomPan(1, 1) * imgY + zoomPan(1, 2))
            );
        }

        static void DrawGridOverlay(const ImageParams& params, ImVec2 widgetTopLeft)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImU32 gridColor = IM_COL32(255, 255, 0, 77); // yellow, ~30% alpha

            // Compute visible range in image coordinates
            Point2d tld = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(), Point2d(0., 0.));
            Point2d brd = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(),
                Point2d((double)params.ImageDisplaySize.width, (double)params.ImageDisplaySize.height));

            int firstX = (int)std::floor(tld.x);
            int lastX  = (int)std::ceil(brd.x);
            int firstY = (int)std::floor(tld.y);
            int lastY  = (int)std::ceil(brd.y);

            // Vertical grid lines
            for (int x = firstX; x <= lastX; ++x)
            {
                ImVec2 top = ImageToScreen((double)x, (double)firstY, params.ZoomPanMatrix, widgetTopLeft);
                ImVec2 bot = ImageToScreen((double)x, (double)lastY, params.ZoomPanMatrix, widgetTopLeft);
                dl->AddLine(top, bot, gridColor);
            }
            // Horizontal grid lines
            for (int y = firstY; y <= lastY; ++y)
            {
                ImVec2 left  = ImageToScreen((double)firstX, (double)y, params.ZoomPanMatrix, widgetTopLeft);
                ImVec2 right = ImageToScreen((double)lastX, (double)y, params.ZoomPanMatrix, widgetTopLeft);
                dl->AddLine(left, right, gridColor);
            }
        }

        static void DrawPixelValuesOverlay(const ImageParams& params, const ImageBuffer& image, ImVec2 widgetTopLeft)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            double zoomFactor = params.ZoomPanMatrix(0, 0);

            // Compute visible range in image coordinates
            Point2d tld = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(), Point2d(0., 0.));
            Point2d brd = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(),
                Point2d((double)params.ImageDisplaySize.width, (double)params.ImageDisplaySize.height));
            int firstX = (int)std::floor(tld.x);
            int lastX  = (int)std::ceil(brd.x);
            int firstY = (int)std::floor(tld.y);
            int lastY  = (int)std::ceil(brd.y);

            bool drawPixelCoords = zoomFactor > 80.;

            // Font size scales with the pixel cell size, capped at the ImGui font size
            float cellSize = (float)zoomFactor;
            float fontSize = std::min(cellSize * 0.25f, ImGui::GetFontSize());

            for (int y = firstY; y <= lastY; ++y)
            {
                for (int x = firstX; x <= lastX; ++x)
                {
                    if (x < 0 || y < 0 || x >= image.width || y >= image.height)
                        continue;

                    std::string pixelInfo = MatrixInfoUtils::MatPixelColorInfo(image, x, y, '\n', false);
                    if (drawPixelCoords)
                        pixelInfo = std::string("x:") + std::to_string(x) + "\n" + "y:" + std::to_string(y) + "\n" + pixelInfo;

                    // Position: center of the pixel cell in screen space
                    ImVec2 cellCenter = ImageToScreen((double)x + 0.5, (double)y + 0.5, params.ZoomPanMatrix, widgetTopLeft);

                    // Smart text color: black on light backgrounds, white on dark
                    ImU32 textColor;
                    {
                        // Read pixel color from the RGBA cache would be ideal, but we have the source image.
                        // Approximate luminance from source pixel.
                        double luminance = 128.;
                        if (image.depth == ImageDepth::uint8 && image.channels >= 1)
                        {
                            const uint8_t* p = image.ptr<uint8_t>(y) + x * image.channels;
                            if (image.channels >= 3)
                                luminance = p[0] * 0.2126 + p[1] * 0.7152 + p[2] * 0.0722;
                            else
                                luminance = p[0];
                        }
                        textColor = (luminance > 170.) ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
                    }

                    // Draw each line of the pixel info, centered in the cell
                    std::istringstream stream(pixelInfo);
                    std::string line;
                    int lineIdx = 0;
                    int lineCount = (int)std::count(pixelInfo.begin(), pixelInfo.end(), '\n') + 1;
                    float lineHeight = fontSize + 1.f;
                    float totalHeight = lineHeight * lineCount;
                    float startY = cellCenter.y - totalHeight / 2.f;

                    while (std::getline(stream, line))
                    {
                        ImVec2 textSize = ImGui::CalcTextSize(line.c_str());
                        float scaledWidth = textSize.x * (fontSize / ImGui::GetFontSize());
                        ImVec2 pos(cellCenter.x - scaledWidth / 2.f, startY + lineIdx * lineHeight);
                        dl->AddText(ImGui::GetFont(), fontSize, pos, textColor, line.c_str());
                        lineIdx++;
                    }
                }
            }
        }

        static void DrawWatchedPixelsOverlay(const ImageParams& params, ImVec2 widgetTopLeft)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            float zoom = (float)params.ZoomPanMatrix(0, 0);

            for (size_t i = 0; i < params.WatchedPixels.size(); ++i)
            {
                Point w = params.WatchedPixels[i];
                // Center of the watched pixel in screen space
                ImVec2 center = ImageToScreen((double)w.x + 0.5, (double)w.y + 0.5, params.ZoomPanMatrix, widgetTopLeft);

                // Check if visible in the display area
                if (center.x < widgetTopLeft.x || center.y < widgetTopLeft.y
                    || center.x > widgetTopLeft.x + params.ImageDisplaySize.width
                    || center.y > widgetTopLeft.y + params.ImageDisplaySize.height)
                    continue;

                // Cross-hole marker with black outline + white fill for visibility on any background
                float em = ImGui::GetFontSize(); // trick to adapt to any DPI
                float crossSize = em * 0.4f;
                float holeSize =  em * 0.1f;
                ImU32 outlineColor = IM_COL32(0, 0, 0, 220);
                ImU32 fillColor = IM_COL32(255, 255, 255, 255);

                // Four diagonal lines forming a cross with a hole in the center
                // Each line drawn twice: black outline first, then white on top
                ImVec2 ends[4][2] = {
                    { {center.x - crossSize, center.y - crossSize}, {center.x - holeSize, center.y - holeSize} },
                    { {center.x + holeSize, center.y - holeSize},   {center.x + crossSize, center.y - crossSize} },
                    { {center.x - crossSize, center.y + crossSize}, {center.x - holeSize, center.y + holeSize} },
                    { {center.x + holeSize, center.y + holeSize},   {center.x + crossSize, center.y + crossSize} },
                };
                for (auto& e : ends)
                {
                    dl->AddLine(e[0], e[1], outlineColor, 3.f);
                    dl->AddLine(e[0], e[1], fillColor, 1.5f);
                }

                // Label with background, spaced away from the cross
                std::string label = std::to_string(i);
                ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
                float labelGap = crossSize + em * 0.4f;
                ImVec2 labelPos(center.x + labelGap, center.y - textSize.y / 2.f);
                ImVec2 bgPad(3.f, 2.f);
                dl->AddRectFilled(
                    ImVec2(labelPos.x - bgPad.x, labelPos.y - bgPad.y),
                    ImVec2(labelPos.x + textSize.x + bgPad.x, labelPos.y + textSize.y + bgPad.y),
                    IM_COL32(0, 0, 0, 200), 3.f);
                dl->AddRect(
                    ImVec2(labelPos.x - bgPad.x, labelPos.y - bgPad.y),
                    ImVec2(labelPos.x + textSize.x + bgPad.x, labelPos.y + textSize.y + bgPad.y),
                    IM_COL32(255, 255, 255, 150), 3.f);
                dl->AddText(labelPos, IM_COL32(255, 255, 255, 255), label.c_str());
            }
        }

        void DrawAlphaCheckerboardBackground(const ImageParams& params, ImVec2 widgetTopLeft)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            float w = (float)params.ImageDisplaySize.width;
            float h = (float)params.ImageDisplaySize.height;
            ImVec2 tl = widgetTopLeft;

            // Checkerboard with two gray tones (matching the original 30px squares)
            ImU32 colorA = IM_COL32(190, 190, 190, 255);
            ImU32 colorB = IM_COL32(240, 240, 240, 255);
            float squareSize = 30.f;

            // Fill background with one color, then draw squares of the other
            dl->AddRectFilled(tl, ImVec2(tl.x + w, tl.y + h), colorA);
            for (float y = 0; y < h; y += squareSize)
            {
                for (float x = 0; x < w; x += squareSize)
                {
                    int ix = (int)(x / squareSize);
                    int iy = (int)(y / squareSize);
                    if ((ix + iy) % 2 == 0)
                    {
                        float x0 = tl.x + x;
                        float y0 = tl.y + y;
                        float x1 = std::min(x0 + squareSize, tl.x + w);
                        float y1 = std::min(y0 + squareSize, tl.y + h);
                        dl->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), colorB);
                    }
                }
            }
        }

        void DrawSchoolPaperBackground(const ImageParams& params, ImVec2 widgetTopLeft)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            float w = (float)params.ImageDisplaySize.width;
            float h = (float)params.ImageDisplaySize.height;
            ImVec2 tl = widgetTopLeft;
            ImVec2 br(tl.x + w, tl.y + h);

            // Paper background
            ImU32 paperColor = IM_COL32(205, 215, 220, 255);
            ImU32 lineColor  = IM_COL32(199, 196, 184, 255);
            dl->AddRectFilled(tl, br, paperColor);

            // Grid lines every 17 pixels
            float quadSize = 17.f;
            for (float y = tl.y; y < br.y; y += quadSize)
                dl->AddLine(ImVec2(tl.x, y), ImVec2(br.x, y), lineColor);
            for (float x = tl.x; x < br.x; x += quadSize)
                dl->AddLine(ImVec2(x, tl.y), ImVec2(x, br.y), lineColor);
        }

        void DrawAnnotationsOverlay(
            const ImageParams& params,
            const ImageBuffer& image,
            ImVec2 widgetTopLeft)
        {
            double zoomFactor = params.ZoomPanMatrix(0, 0);
            double gridMinZoomFactor = 12.;

            // Clip all annotations to the image display area
            ImVec2 clipMin = widgetTopLeft;
            ImVec2 clipMax(widgetTopLeft.x + (float)params.ImageDisplaySize.width,
                           widgetTopLeft.y + (float)params.ImageDisplaySize.height);
            ImGui::GetWindowDrawList()->PushClipRect(clipMin, clipMax, true);

            // Draw grid
            if (params.ShowGrid && zoomFactor >= gridMinZoomFactor)
                DrawGridOverlay(params, widgetTopLeft);

            // Draw pixel values
            double drawPixelValuesMinZoomFactor = (image.depth == ImageDepth::uint8) ? 36. : 48.;
            if (params.DrawValuesOnZoomedPixels && zoomFactor > drawPixelValuesMinZoomFactor)
                DrawPixelValuesOverlay(params, image, widgetTopLeft);

            // Draw watched pixel markers
            if (params.HighlightWatchedPixels && !params.WatchedPixels.empty())
                DrawWatchedPixelsOverlay(params, widgetTopLeft);

            ImGui::GetWindowDrawList()->PopClipRect();
        }

        bool HasColormapParam(const ImageParams &params)
        {
            return (!params.ColormapSettings.Colormap.empty() || !params.ColormapSettings.internal_ColormapHovered.empty());
        }

    } // namespace ImageDrawing

} // namespace ImmVision
