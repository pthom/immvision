#include "immvision/image_navigator.h"
#include "immvision/internal/gl_texture.h"
#include "immvision/internal/internal_icons.h"
#include "immvision/internal/imgui_imm.h"
#include "immvision/internal/cv_drawing_utils.h"
#include "immvision/internal/portable_file_dialogs.h"
#include "imgui.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <map>
#include <vector>
#include <iostream>

namespace ImmVision
{
    namespace ZoomMatrix
    {
        using ZoomMatrixType = cv::Matx33d;

        ZoomMatrixType Identity()
        {
            return cv::Matx33d::eye();
        }

        ZoomMatrixType ComputeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            mat(0, 2) = zoomCenter.x * (1. - zoomRatio);
            mat(1, 2) = zoomCenter.y * (1. - zoomRatio);
            return mat;
        }

        ZoomMatrixType ComputePanMatrix(const ImVec2 & dragDelta, double currentZoom)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 2) = dragDelta.x / currentZoom;
            mat(1, 2) = dragDelta.y / currentZoom;
            return mat;
        }

        ZoomMatrixType MakeScaleOne(cv::Size imageSize, cv::Size viewportSize)
        {
            ZoomMatrixType r = Identity();
            r(0, 2) = (viewportSize.width / 2 - imageSize.width / 2);
            r(1, 2) = (viewportSize.height / 2 - imageSize.height / 2);
            return r;
        }

        cv::Matx23d ZoomMatrixToM23(const cv::Matx33d &m)
        {
            cv::Matx23d r;
            for (int y = 0; y < 2; y++)
                for (int x = 0; x < 3; x++)
                    r(y, x) = m(y, x);
            return r;
        }

        ZoomMatrixType MakeFullView(cv::Size imageSize, cv::Size viewportSize)
        {
            ZoomMatrixType r = Identity();

            double zoom;
            {
                double k_image = (double)imageSize.width / (double)imageSize.height;
                double k_viewport = (double)viewportSize.width / (double)viewportSize.height;
                if (k_image > k_viewport)
                    zoom = (double)viewportSize.width / (double)imageSize.width;
                else
                    zoom = (double)viewportSize.height / (double)imageSize.height;
            }

            r(0, 0) = zoom;
            r(1, 1) = zoom;

            return r;
        }

        bool IsEqual(const ZoomMatrixType & v1, const ZoomMatrixType & v2)
        {
            for (int j = 0; j < 3; j++)
                for (int i = 0; i < 3; i++)
                    if (fabs(v2(j, i) - v1(j, i)) > 1E-6)
                        return false;
            return true;
        }

        cv::Point2d Apply(const ZoomMatrixType& zoomMatrix, const cv::Point2d &p)
        {
            cv::Matx31d pMat(p.x, p.y, 1.);
            cv::Matx31d rMat = zoomMatrix * pMat;
            cv::Point2d r(rMat(0, 0), rMat(1, 0));
            return r;
        }

        ZoomMatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const ZoomMatrixType& oldZoomMatrix,
            const cv::Size& oldDisplaySize, const cv::Size& newDisplaySize)
        {
            if (oldDisplaySize.empty() || newDisplaySize.empty())
                return oldZoomMatrix;

            ZoomMatrixType zoomMatrix;

            auto fnImageCenter = [](const cv::Size s) {
                return cv::Point2d((double)s.width / 2.f, (double)s.height / 2.);
            };

            double newZoomFactor;
            {
                double oldZoomFactor = oldZoomMatrix(0, 0);
                double kx = (double)newDisplaySize.width / (double)oldDisplaySize.width;
                double ky = (double)newDisplaySize.height / (double)oldDisplaySize.height;
                double k = std::min(kx, ky);
                newZoomFactor = oldZoomFactor * k;
            }

            zoomMatrix = ZoomMatrixType::eye();
            zoomMatrix(0, 0) = zoomMatrix(1, 1) = newZoomFactor;

            cv::Point2d translation;
            {
                cv::Point2d oldDisplayCenter_Zoomed = fnImageCenter(oldDisplaySize);
                cv::Point2d oldDisplayCenter_Image = ZoomMatrix::Apply(oldZoomMatrix.inv(), oldDisplayCenter_Zoomed);

                cv::Point2d newDisplayCenter_Zoomed_Wanted = fnImageCenter(newDisplaySize);
                cv::Point2d newDisplayCenter_Zoomed_Now = ZoomMatrix::Apply(zoomMatrix, oldDisplayCenter_Image);
                translation = newDisplayCenter_Zoomed_Wanted - newDisplayCenter_Zoomed_Now;
            }

            zoomMatrix(0, 2) = translation.x;
            zoomMatrix(1, 2) = translation.y;

            return zoomMatrix;
        }


    } // namespace ZoomMatrix

    cv::Matx33d MakeZoomMatrix(
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        const cv::Size displayedImageSize
        )
    {
        auto mat = cv::Matx33d::eye();
        mat(0, 0) = zoomRatio;
        mat(1, 1) = zoomRatio;
        double dx = (double)displayedImageSize.width / 2. - zoomRatio * zoomCenter.x;
        double dy = (double)displayedImageSize.height / 2. - zoomRatio * zoomCenter.y;
        mat(0, 2) = dx;
        mat(1, 2) = dy;
        return mat;
    }


    namespace ColorAdjustmentsUtils
    {
        static bool IsNone(const ColorAdjustments& a)
        {
            return (a.Factor == 1.) && (a.Delta == 0.);
        }

        static bool IsEqual(const ColorAdjustments& v1, const ColorAdjustments& v2)
        {
            if (fabs(v2.Factor - v1.Factor) > 1E-6)
                return false;
            if (fabs(v2.Delta - v1.Delta) > 1E-6)
                return false;
            return true;
        }

        static cv::Mat Adjust(const ColorAdjustments& a, const cv::Mat &image)
        {
            if (IsNone(a))
                return image;
            else return
                    image * a.Factor + a.Delta;
        }

        ColorAdjustments ComputeInitialImageAdjustments(const cv::Mat& m)
        {
            ColorAdjustments r;
            if ((m.depth() == CV_32F) || (m.depth() == CV_64F))
            {
                std::vector<double> minima, maxima;
                std::vector<cv::Mat> channels;
                cv::split(m, channels);
                for (const cv::Mat&channel: channels)
                {
                    double min, max;
                    cv::minMaxLoc(channel, &min, &max);
                    minima.push_back(min);
                    maxima.push_back(max);
                }

                double min = *std::min_element(minima.begin(), minima.end());
                double max = *std::max_element(maxima.begin(), maxima.end());
                r.Factor = 1. / (max - min);
                r.Delta = -min * r.Factor;
            }
            return r;
        }

    } // namespace ImageAdjustmentsUtils


    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const cv::Mat& m)
        {
            std::map<int, std::string> depthNames
                {
                    { CV_8U, "CV_8U" },
                    { CV_8S, "CV_8S" },
                    { CV_16U, "CV_16U" },
                    { CV_16S, "CV_16S" },
                    { CV_32S, "CV_32S"},
                    { CV_32F, "CV_32F"},
                    { CV_64F, "CV_64F"},
                    { CV_16F, "CV_16F"}
                };
            return depthNames.at(m.depth()) + "C" + std::to_string(m.channels());
        }

        std::string _MatInfo(const cv::Mat &m)
        {
            return _MatTypeName(m) + " " + std::to_string(m.cols) + "x" + std::to_string(m.rows);
        }


        template<typename T>
        std::string ShowVecValue(const T& v)
        {
            char buffer_color[300];
            sprintf(buffer_color, "%.5G", v);
            return std::string(buffer_color);
        }
        template<>
        std::string ShowVecValue(const unsigned char& v)
        {
            return std::to_string((int)v);
        }
        std::string JoinStrings(const std::vector<std::string>&v, char separator)
        {
            std::string r;
            for (size_t i = 0; i < v.size(); ++ i)
            {
                r += v[i];
                if (i < v.size() - 1)
                    r += separator;
            }
            return r;
        }
        template<typename _Tp, int cn>
        std::string ShowVecValues(const cv::Vec<_Tp, cn>& v, char separator = ',', bool add_paren = true)
        {
            std::string r;
            std::vector<std::string> val_strs;
            for (int i = 0; i < cn; ++i)
                val_strs.push_back(ShowVecValue(v[i]));

            r = JoinStrings(val_strs, separator);
            if (add_paren)
                r = std::string("(") + r + ")";
            return r;
        }

        std::string MatPixelColorInfo(const cv::Mat & m, int x, int y, char separator = ',', bool add_paren = true)
        {
            auto fnShowVecValues = [separator, add_paren](const auto& v) {
                return ShowVecValues(v, separator, add_paren);
            };
            if (!cv::Rect(cv::Point(0, 0), m.size()).contains(cv::Point(x, y)))
                return "";
            if (m.type() == CV_64FC4)
                return fnShowVecValues(m.at<cv::Vec4d>(y, x));
            else if (m.type() == CV_64FC3)
                return fnShowVecValues(m.at<cv::Vec3d>(y, x));
            else if (m.type() == CV_64FC2)
                return fnShowVecValues(m.at<cv::Vec2d>(y, x));
            else if (m.type() == CV_64FC1)
                return ShowVecValue(m.at<double>(y, x));
            else if (m.type() == CV_32FC4)
                return fnShowVecValues(m.at<cv::Vec4f>(y, x));
            else if (m.type() == CV_32FC3)
                return fnShowVecValues(m.at<cv::Vec3f>(y, x));
            else if (m.type() == CV_32FC2)
                return fnShowVecValues(m.at<cv::Vec2f>(y, x));
            else if (m.type() == CV_32FC1)
                return ShowVecValue(m.at<float>(y, x));
            else if (m.type() == CV_8UC4)
                return fnShowVecValues(m.at<cv::Vec4b>(y, x));
            else if (m.type() == CV_8UC3)
                return fnShowVecValues(m.at<cv::Vec3b>(y, x));
            else if (m.type() == CV_8UC2)
                return fnShowVecValues(m.at<cv::Vec2b>(y, x));
            else if (m.type() == CV_8UC1)
                return ShowVecValue(m.at<unsigned char>(y, x));
            else
                assert("Unhandled matrix type !");

            return "";
        }

    } // namespace MatrixInfoUtils


    namespace ImageNavigatorUtils
    {
        void InitializeMissingParams(ImageNavigatorParams* params, const cv::Mat& image)
        {
            if (ColorAdjustmentsUtils::IsNone(params->ColorAdjustments))
                params->ColorAdjustments = ColorAdjustmentsUtils::ComputeInitialImageAdjustments(image);
            if (params->ZoomMatrix == cv::Matx33d::eye())
                params->ZoomMatrix = ZoomMatrix::MakeFullView(image.size(), params->ImageDisplaySize);
        }

        cv::Mat DrawWatchedPixels(const cv::Mat& image, const ImageNavigatorParams& params)
        {
            cv::Mat r = image.clone();

            std::vector<std::pair<size_t, cv::Point2d>> visiblePixels;
            {
                for (size_t i = 0; i < params.WatchedPixels.size(); ++i)
                {
                    cv::Point w = params.WatchedPixels[i];
                    cv::Point2d p = ZoomMatrix::Apply(params.ZoomMatrix, w);
                    if (cv::Rect(cv::Point(0, 0), params.ImageDisplaySize).contains(p))
                        visiblePixels.push_back({i, p});
                }
            }

            for (const auto& kv : visiblePixels)
            {
                CvDrawingUtils::draw_named_feature(
                    r,         // img
                    kv.second, // position,
                    std::to_string(kv.first),       // name
                    cv::Scalar(255, 255, 255, 255), // color
                    true, // add_cartouche
                    4.,   // size
                    2.5,  // size_hole
                    1     // thickness
                    );
            }

            return r;
        }

        cv::Mat DrawGrid(const cv::Mat& image, const ImageNavigatorParams& params)
        {
            double alpha = 0.23;
            cv::Scalar gridColor(0, 255, 255, 255);
            double x_spacing = (double) params.ZoomMatrix(0, 0);
            double y_spacing = (double) params.ZoomMatrix(1, 1);
            double x_start = (double) params.ZoomMatrix(0, 2) - 0.5 * x_spacing;
            double y_start = (double) params.ZoomMatrix(1, 2) - 0.5 * y_spacing;
            cv::Mat imageWithGrid = CvDrawingUtils::add_grid_to_image(
                image,
                x_start, y_start,
                x_spacing, y_spacing,
                gridColor,
                alpha);
            return imageWithGrid;
        };

        cv::Mat DrawValuesOnZoomedPixels(const cv::Mat& drawingImage, const cv::Mat& valuesImage,
                                         const ImageNavigatorParams& params, bool drawPixelCoords)
        {
            assert(drawingImage.type() == CV_8UC4);

            cv::Mat r = drawingImage;
            cv::Point tl, br;
            {
                cv::Point2d tld = ZoomMatrix::Apply(params.ZoomMatrix.inv(), cv::Point2d(0., 0.));
                cv::Point2d brd = ZoomMatrix::Apply(params.ZoomMatrix.inv(),
                                                   cv::Point2d((double)params.ImageDisplaySize.width,
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

                    cv::Point2d position = ZoomMatrix::Apply(params.ZoomMatrix, cv::Point2d((double)x, (double )y));

                    cv::Scalar textColor;
                    {
                        cv::Scalar white(255, 255, 255, 255);
                        cv::Scalar black(0, 0, 0, 255);
                        cv::Vec4b backgroundColor(0., 0., 0., 0.);
                        if ( cv::Rect(cv::Point(), drawingImage.size()).contains({(int)position.x, (int)position.y}))
                            backgroundColor = drawingImage.at<cv::Vec4b>((int)position.y, (int)position.x);
                        double luminance = backgroundColor[2] * 0.2126 + backgroundColor[1] * 0.7152 + backgroundColor[0] * 0.0722;
                        if (luminance > 170.)
                            textColor = black;
                        else
                            textColor = white;
                    }
                    CvDrawingUtils::text(
                        r,
                        position,
                        pixelInfo,
                        textColor,
                        true, // center_around_point
                        false, // add_cartouche
                        0.3,  //fontScale
                        1     //int thickness
                        );
                }
            }
            return r;
        };


        void BlitImageNavigatorTexture(
            const ImageNavigatorParams& params,
            const cv::Mat& image,
            GlTextureCv* outTexture
            )
        {
            if (image.empty())
                return;

            cv::Mat finalImage = image.clone();

            // Selected channels
            if (finalImage.channels() > 1 && (params.SelectedChannel >= 0) && (params.SelectedChannel < image.channels()))
            {
                std::vector<cv::Mat> channels;
                cv::split(image, channels);
                finalImage = channels[params.SelectedChannel];
            }

            // Convert to BGR
            {
                if (finalImage.type() == CV_8UC3 && !params.IsColorOrderBGR)
                    cv::cvtColor(finalImage, finalImage, cv::COLOR_RGB2BGR);
                if (finalImage.type() == CV_8UC4 && !params.IsColorOrderBGR)
                    cv::cvtColor(finalImage, finalImage, cv::COLOR_RGBA2BGRA);
            }

            // Alpha checkerboard
            {
                if (finalImage.type() == CV_8UC4 && params.ShowAlphaChannelCheckerboard)
                {
                    cv::Mat background = CvDrawingUtils::make_alpha_channel_checkerboard_image(image.size());
                    finalImage = CvDrawingUtils::overlay_alpha_image_precise(background, finalImage, 1.);
                }
            }

            // Color adjustments
            finalImage = ColorAdjustmentsUtils::Adjust(params.ColorAdjustments, finalImage);

            // Zoom
            {
                cv::Mat imageZoomed;
                cv::warpAffine(finalImage, imageZoomed,
                               ZoomMatrix::ZoomMatrixToM23(params.ZoomMatrix), params.ImageDisplaySize, cv::INTER_NEAREST);
                finalImage = imageZoomed;
            }

            // Convert to RGBA
            finalImage = CvDrawingUtils::converted_to_rgba_image(finalImage);
            assert(finalImage.type() == CV_8UC4);

            // Draw grid
            double gridMinZoomFactor = 7.;
            double zoomFactor = (double)params.ZoomMatrix(0, 0);
            if (params.ShowGrid && zoomFactor >= gridMinZoomFactor)
                finalImage = DrawGrid(finalImage, params);

            // Draw Pixel Values
            double drawPixelvaluesMinZoomFactor = (image.depth() == CV_8U) ? 36. : 48.;
            if (params.DrawValuesOnZoomedPixels && zoomFactor > drawPixelvaluesMinZoomFactor)
            {
                double drawPixelCoordsMinZoomFactor = 60.;
                bool drawPixelCoords = zoomFactor > drawPixelCoordsMinZoomFactor;
                finalImage = DrawValuesOnZoomedPixels(finalImage, image, params, drawPixelCoords);
            }

            // Draw Watched Pixels
            if (params.HighlightWatchedPixels && (! params.WatchedPixels.empty()))
                finalImage = DrawWatchedPixels(finalImage, params);

            outTexture->BlitMat(finalImage);
        }

        bool operator==(const ColorAdjustments& v1, const ColorAdjustments& v2)
        {
            if (fabs(v2.Factor - v1.Factor) > 1E-6)
                return false;
            if (fabs(v2.Delta - v1.Delta) > 1E-6)
                return false;
            return true;
        }

        bool ShallRefreshTexture(const ImageNavigatorParams& v1, const ImageNavigatorParams& v2)
        {
            if (v1.ImageDisplaySize != v2.ImageDisplaySize)
                return true;
            if (! ZoomMatrix::IsEqual(v1.ZoomMatrix, v2.ZoomMatrix))
                return true;
            if (! (v1.ColorAdjustments == v2.ColorAdjustments))
                return true;
            if (v1.ShowGrid != v2.ShowGrid)
                return true;
            if (v1.SelectedChannel != v2.SelectedChannel)
                return true;
            if (v1.ShowAlphaChannelCheckerboard != v2.ShowAlphaChannelCheckerboard)
                return true;
            if (v1.IsColorOrderBGR != v2.IsColorOrderBGR)
                return true;
            if (v1.WatchedPixels.size() != v2.WatchedPixels.size())
                return true;
            if (v1.HighlightWatchedPixels != v2.HighlightWatchedPixels)
                return true;
            if (v1.DrawValuesOnZoomedPixels != v2.DrawValuesOnZoomedPixels)
                return true;
            return false;
        }

        cv::Point2d DisplayTexture_TrackMouse(const GlTextureCv& texture, ImVec2 displaySize)
        {
            ImVec2 imageTopLeft = ImGui::GetCursorScreenPos();
            texture.Draw_DisableDragWindow(displaySize);
            bool isImageHovered = ImGui::IsItemHovered();
            ImVec2 mouse = ImGui::GetMousePos();
            if (isImageHovered)
                return cv::Point2d((double)(mouse.x - imageTopLeft.x), (double)(mouse.y - imageTopLeft.y));
            else
                return cv::Point2d(-1., -1.);
        }

        void ShowImageInfo(const cv::Mat &image, double zoomFactor)
        {
            std::string info = MatrixInfoUtils::_MatInfo(image);
            ImGui::Text("%s - Zoom:%.3lf", info.c_str(), zoomFactor);
        }


        void ShowPixelColorWidget(
            const cv::Mat &image,
            cv::Point pt,
            const ImageNavigatorParams& params)
        {
            bool isInImage = cv::Rect(cv::Point(0, 0), image.size()).contains((pt));
            auto UCharToFloat = [](int v) { return (float)((float) v / 255.f); };
            auto Vec3bToImVec4 = [&UCharToFloat, &params](cv::Vec3b v) {
                return params.IsColorOrderBGR ?
                            ImVec4(UCharToFloat(v[2]), UCharToFloat(v[1]), UCharToFloat(v[0]), UCharToFloat(255))
                        :   ImVec4(UCharToFloat(v[0]), UCharToFloat(v[1]), UCharToFloat(v[2]), UCharToFloat(255));
            };
            auto Vec4bToImVec4 = [&UCharToFloat, &params](cv::Vec4b v) {
                return params.IsColorOrderBGR ?
                            ImVec4(UCharToFloat(v[2]), UCharToFloat(v[1]), UCharToFloat(v[0]), UCharToFloat(v[3]))
                       :    ImVec4(UCharToFloat(v[0]), UCharToFloat(v[1]), UCharToFloat(v[2]), UCharToFloat(v[3]));
            };

            bool done = false;
            std::string id = std::string("##pixelcolor_") + std::to_string(pt.x) + "," + std::to_string(pt.y);
            if (image.depth() == CV_8U)
            {
                ImGuiColorEditFlags editFlags =
                    ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_AlphaPreviewHalf
                    | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Uint8;
                if (!isInImage)
                {
                    // ColorEdit4 introduces a strange line spacing on the next group
                    // which cannot be simulated with ImGui::Dummy
                    // => we add a dummy one (hopefully black on a black background)
                    float dummyColor[4]{0.f, 0.f, 0.f, 255.f};
                    ImGui::SetNextItemWidth(1.f);
                    ImGui::ColorEdit4(id.c_str(), dummyColor, ImGuiColorEditFlags_NoInputs );
                    done = true;
                }
                else if (image.channels() == 3)
                {
                    cv::Vec3b col = image.at<cv::Vec3b>(pt.y, pt.x);
                    ImVec4 colorAsImVec = Vec3bToImVec4(col);
                    ImGui::SetNextItemWidth(150.f);
                    ImGui::ColorEdit3(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
                else if (image.channels() == 4)
                {
                    cv::Vec4b col = image.at<cv::Vec4b>(pt.y, pt.x);
                    ImVec4 colorAsImVec = Vec4bToImVec4(col);
                    ImGui::SetNextItemWidth(200.f);
                    ImGui::ColorEdit4(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
            }
            if (! done)
            {
                 std::string pixelInfo = MatrixInfoUtils::MatPixelColorInfo(image, pt.x, pt.y);
                 ImGui::Text("%s", pixelInfo.c_str());
            }
        }


        class ImageNavigatorTextureCache
        {
        public:
            // members
            struct CachedData
            {
                ImageNavigatorParams* ImageNavigatorParams = nullptr;
                GlTextureCv GlTextureCv;
                ImVec2 LastDragDelta;
                std::vector<char> FilenameEditBuffer = std::vector<char>(1000, '\0');
                bool   IsMouseDragging = false;
                struct ImageNavigatorParams  PreviousParams;
            };

            void UpdateCache(const cv::Mat& image, ImageNavigatorParams* params, bool refresh)
            {
                params->ImageDisplaySize = ImGuiImm::ComputeDisplayImageSize(params->ImageDisplaySize, image.size());

                bool needsRefreshTexture = refresh;

                if (mCache.find(&image) == mCache.end())
                {
                    InitializeMissingParams(params, image);
                    needsRefreshTexture = true;
                }
                mCache[&image].ImageNavigatorParams = params;

                auto& cachedData = mCache.at(&image);

                ImageNavigatorParams oldParams = cachedData.PreviousParams;
                *cachedData.ImageNavigatorParams = *params;

                if (ShallRefreshTexture(oldParams, *params))
                    needsRefreshTexture = true;
                if ((!oldParams.ImageDisplaySize.empty()) && (oldParams.ImageDisplaySize != params->ImageDisplaySize))
                    params->ZoomMatrix = ZoomMatrix::UpdateZoomMatrix_DisplaySizeChanged(
                        oldParams.ZoomMatrix, oldParams.ImageDisplaySize, params->ImageDisplaySize);
                if (needsRefreshTexture)
                    BlitImageNavigatorTexture(*params, image, &cachedData.GlTextureCv);

                if (! ZoomMatrix::IsEqual(oldParams.ZoomMatrix, params->ZoomMatrix))
                    UpdateLinkedZooms(image);
                if (! ColorAdjustmentsUtils::IsEqual(oldParams.ColorAdjustments, params->ColorAdjustments))
                    UpdateLinkedColorAdjustments(image);

                cachedData.PreviousParams = *params;
            }

            CachedData& GetCache(const cv::Mat& image)
            {
                assert(mCache.find(&image) != mCache.end());
                return mCache.at(&image);
            }

        private:
            // Methods
            void UpdateLinkedZooms(const cv::Mat& image)
            {
                assert(mCache.find(&image) != mCache.end());
                std::string key = mCache[&image].ImageNavigatorParams->ZoomKey;
                if (key.empty())
                    return;
                ZoomMatrix::ZoomMatrixType newZoom = mCache[&image].ImageNavigatorParams->ZoomMatrix;
                for (auto& kv : mCache)
                    if ( (kv.second.ImageNavigatorParams->ZoomKey == key) && (kv.first != &image))
                        kv.second.ImageNavigatorParams->ZoomMatrix = newZoom;
            }
            void UpdateLinkedColorAdjustments(const cv::Mat& image)
            {
                assert(mCache.find(&image) != mCache.end());
                std::string key = mCache[&image].ImageNavigatorParams->ColorAdjustmentsKey;
                if (key.empty())
                    return;
                ColorAdjustments newColorAdjustments = mCache[&image].ImageNavigatorParams->ColorAdjustments;
                for (auto& kv : mCache)
                    if ( (kv.second.ImageNavigatorParams->ColorAdjustmentsKey == key) && (kv.first != &image))
                        kv.second.ImageNavigatorParams->ColorAdjustments = newColorAdjustments;
            }

            std::map<const cv::Mat*, CachedData> mCache;
        };
        ImageNavigatorTextureCache gImageNavigatorTextureCache;
    } // namespace ImageNavigatorUtils


    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams* params,
        bool refresh)
    {
        if (image.empty())
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "%s -> empty image !!!", params->Legend.c_str());
            return cv::Point2d();
        }

        ImageNavigatorUtils::gImageNavigatorTextureCache.UpdateCache(image, params, refresh);
        auto &cache = ImageNavigatorUtils::gImageNavigatorTextureCache.GetCache(image);

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
                ImGui::Text("");

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
                    ImageNavigatorUtils::ShowPixelColorWidget(image, watchedPixel, *params);

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
                params->WatchedPixels.erase(params->WatchedPixels.begin() + (size_t)idxToRemove);

            ImGui::Checkbox("Highlight Watched Pixels", &params->HighlightWatchedPixels);
        };
        auto fnWatchedPixels_Draw = [&params](const cv::Mat& m) -> cv::Mat
        {
            if (! params->HighlightWatchedPixels)
                return m;
            return m;
        };

        //
        // Lambdas / Options & Adjustments
        //
        auto fnOptionsInnerGui = [&params, &cache, &image, &fnWatchedPixels_Gui, &wasWatchedPixelAdded, &fnPanelTitle]()
        {
            // We create a dummy table in order to force the collapsing headers width
            float optionsWidth = 330.f;
            ImGui::BeginTable("##OptionsTable", 1, 0, ImVec2(optionsWidth, 0.f));
            ImGui::TableNextColumn();

            auto fnMyCollapsingHeader = [&fnPanelTitle](const char *name)
            {
                //ImVec2 lastPanelSize = ImGuiImm::GroupPanel_FlagBorder_LastKnownSize(fnPanelTitle().c_str());
                //return ImGuiImm::CollapsingHeaderFixedWidth(name, lastPanelSize.x - 35.f);
                return ImGui::CollapsingHeader(name);
            };

            // Adjust float values
            bool hasAdjustFloatValues = true; // ((image.depth() == CV_32F) || (image.depth() == CV_64F));
            if (hasAdjustFloatValues && fnMyCollapsingHeader("Adjust"))
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
                        params->ColorAdjustments = ColorAdjustments();
            }

            // Watched Pixels
            if (wasWatchedPixelAdded)
                ImGui::SetNextItemOpen(wasWatchedPixelAdded);
            if (fnMyCollapsingHeader("Watched Pixels"))
                fnWatchedPixels_Gui();

            // Image display options
            if (fnMyCollapsingHeader("Image Display"))
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
            if (fnMyCollapsingHeader("Options"))
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
            }

            // Save Image
            if (fnMyCollapsingHeader("Save"))
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
                    char *filename = cache.FilenameEditBuffer.data();
                    ImGui::SetNextItemWidth(200.f);
                    ImGui::InputText("##filename", filename, 1000);
                    //ImGui::SetNextItemWidth(200.f);
                    ImGui::Text("The image will be saved in the current folder");
                    ImGui::Text("with a format corresponding to the extension");
                    if (ImGui::SmallButton("save"))
                        cv::imwrite(filename, image);
                }
            }

            ImGui::NewLine();
            if (ImGui::Checkbox("Show Options in tooltip window", &params->ShowOptionsInTooltip))
            {
                if (!params->ShowOptionsInTooltip) // We were in a tooltip when clicking
                    params->ShowOptions = true;
            }

            ImGui::EndTable();

        };
        auto fnToggleShowOptions = [&params]()
        {
            if (params->ShowOptionsInTooltip)
                ImGui::OpenPopup("Options");
            else
                params->ShowOptions = !params->ShowOptions;
        };
        auto fnOptionGui = [&params, &fnOptionsInnerGui]()
        {
            if (params->ShowOptionsInTooltip)
            {
                if (ImGui::BeginPopup("Options"))
                {
                    fnOptionsInnerGui();
                    ImGui::EndPopup();
                }
            }
            else if (params->ShowOptions)
            {
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("Options");
                fnOptionsInnerGui();
                ImGui::EndGroup();
            }
        };

        //
        // Lambdas / Handle Zoom
        //
        // Mouse dragging
        auto fnHandleMouseDragging = [&cache, &params]()
        {
            ZoomMatrix::ZoomMatrixType& zoomMatrix = params->ZoomMatrix;

            int mouseDragButton = 0;
            bool isMouseDraggingInside = ImGui::IsMouseDragging(mouseDragButton) && ImGui::IsItemHovered();
            if (isMouseDraggingInside)
                cache.IsMouseDragging = true;
            if (! ImGui::IsMouseDown(mouseDragButton))
            {
                cache.IsMouseDragging = false;
                cache.LastDragDelta = ImVec2(0.f, 0.f);
            }
            if (cache.IsMouseDragging && params->PanWithMouse )
            {
                ImVec2 dragDelta = ImGui::GetMouseDragDelta(mouseDragButton);
                ImVec2 dragDeltaDelta(dragDelta.x - cache.LastDragDelta.x, dragDelta.y - cache.LastDragDelta.y);
                zoomMatrix = zoomMatrix * ZoomMatrix::ComputePanMatrix(dragDeltaDelta, zoomMatrix(0, 0));
                cache.LastDragDelta = dragDelta;
            }
        };
        auto fnShowZoomButtons = [&params, &image]()
        {
            if (params->ShowZoomButtons)
            {
                ZoomMatrix::ZoomMatrixType& zoomMatrix = params->ZoomMatrix;

                cv::Point2d viewportCenter_originalImage = ZoomMatrix::Apply(
                    zoomMatrix.inv(),
                    cv::Point2d (
                        (double)params->ImageDisplaySize.width / 2.f,
                        (double)params->ImageDisplaySize.height / 2.f)
                );

                {
                    ImGui::PushButtonRepeat(true);
                    if (Icons::IconButton(Icons::IconType::ZoomPlus))
                        zoomMatrix = zoomMatrix * ZoomMatrix::ComputeZoomMatrix(viewportCenter_originalImage, 1.1);

                    ImGui::SameLine();

                    if (Icons::IconButton(Icons::IconType::ZoomMinus))
                        zoomMatrix = zoomMatrix * ZoomMatrix::ComputeZoomMatrix(viewportCenter_originalImage, 1. / 1.1);

                    ImGui::PopButtonRepeat();
                }
                ImGui::SameLine();
                // Scale1 & Full View Zoom  buttons
                {
                    auto scaleOneZoomInfo = ZoomMatrix::MakeScaleOne(image.size(), params->ImageDisplaySize);
                    auto fullViewZoomInfo = ZoomMatrix::MakeFullView(image.size(), params->ImageDisplaySize);
                    if (Icons::IconButton(
                        Icons::IconType::ZoomScaleOne,
                        ZoomMatrix::IsEqual(zoomMatrix, scaleOneZoomInfo)) // disabled flag
                        )
                        zoomMatrix = scaleOneZoomInfo;

                    ImGui::SameLine();

                    if (Icons::IconButton(
                        Icons::IconType::ZoomFullView,
                        ZoomMatrix::IsEqual(zoomMatrix,fullViewZoomInfo)) // disabled flag
                        )
                        zoomMatrix = fullViewZoomInfo;
                }
            }

        };

        //
        // Lambda / Show image
        //
        auto fnShowImage = [&params, &cache]()
        {
            cv::Point2d mouseLocation = ImageNavigatorUtils::DisplayTexture_TrackMouse(cache.GlTextureCv, ImVec2((float)params->ImageDisplaySize.width, (float)params->ImageDisplaySize.height));
            cv::Point2d mouseLocation_originalImage =
                ImGui::IsItemHovered() ? ZoomMatrix::Apply(params->ZoomMatrix.inv(), mouseLocation) : cv::Point2d(-1., -1.);
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
            ImageNavigatorUtils::ShowPixelColorWidget(image, mouseLoc, *params);
        };

        //
        // GUI
        //
        ImGui::PushID("##ImageNavigator"); ImGui::PushID(&image);
        cv::Point2d mouseLocation_originalImage;

        // BeginGroupPanel
        bool drawBorder = params->ShowLegendBorder || (! params->ShowOptionsInTooltip);
        ImGuiImm::BeginGroupPanel_FlagBorder(fnPanelTitle().c_str(), drawBorder);
        {
            ImGui::BeginGroup();
            // Show image
            mouseLocation_originalImage = fnShowImage();
            // Add Watched Pixel on double click
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
                fnWatchedPixels_Add(mouseLocation_originalImage);

            // Handle Mouse
            fnHandleMouseDragging();

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
                ImageNavigatorUtils::ShowImageInfo(image, params->ZoomMatrix(0, 0));
            if (params->ShowPixelInfo)
                fnShowPixelInfo(mouseLocation_originalImage);
            ImGui::EndGroup();

            // Show Options
            fnOptionGui();
        }
        ImGuiImm::EndGroupPanel_FlagBorder();
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