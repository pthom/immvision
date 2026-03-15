#include "immvision/internal/drawing/image_drawing.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/cv/drawing_utils.h"
#include "immvision/internal/cv/matrix_info_utils.h"
#include "immvision/internal/cv/colormap.h"

#include <cstring>  // memcpy


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

        bool HasColormapParam(const ImageParams &params)
        {
            return (!params.ColormapSettings.Colormap.empty() || !params.ColormapSettings.internal_ColormapHovered.empty());
        }

    } // namespace ImageDrawing

} // namespace ImmVision
