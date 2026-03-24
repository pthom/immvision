#include "immvision/internal/drawing/draw_list_annotate.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/cv/matrix_info_utils.h"

#include <cassert>
#include <cstring>  // memcpy
#include <cmath>
#include <sstream>
#include <algorithm> // std::count


namespace ImmVision
{

    namespace DrawListAnnotate
    {

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

            bool drawPixelCoords = zoomFactor > 130.;

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
                        // Approximate luminance from source pixel
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
                float em = ImGui::GetFontSize(); // adapt to DPI
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


    } // namespace ImageDrawing

} // namespace ImmVision
