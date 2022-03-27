#include "immvision/image_navigator.h"
#include "immvision/internal/gl_texture.h"
#include "immvision/internal/gl_texture.h"
#include "immvision/internal/imgui_ext.h"
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
    } // namespace ZoomMatrix


    namespace ColorAdjustmentsUtils
    {
        static bool IsNone(const ColorAdjustments& a)
        {
            return (a.Factor == 1.) && (a.Delta == 0.);
        }

        static cv::Mat Adjust(const ColorAdjustments& a, const cv::Mat &image)
        {
            if (IsNone(a))
                return image;
            else return
                    image * a.Factor + a.Delta;
        }

        ColorAdjustments ComputeInitialImageAdjustments(
            const ColorAdjustments& initialAdjustments,
            const cv::Mat& displayedImage)
        {
            if (!IsNone(initialAdjustments))
                return initialAdjustments;

            ColorAdjustments r;
            if ( (displayedImage.channels() == 1) && ((displayedImage.depth() == CV_32F) || (displayedImage.depth() == CV_64F)) )
            {
                double min, max;
                cv::minMaxLoc(displayedImage, &min, &max);
                r.Factor = 1. / (max - min);
                r.Delta = -min;
            }
            return r;
        }

    } // namespace ImageAdjustmentsUtils


    namespace ImageNavigatorUtils
    {
        ImageNavigatorParams InitializeMissingParams(
            const ImageNavigatorParams& params,
            const cv::Mat& image)
        {
            ImageNavigatorParams r = params;
            r.ColorAdjustments = ColorAdjustmentsUtils::ComputeInitialImageAdjustments(
                params.ColorAdjustments, image);
            if (r.ZoomMatrix == cv::Matx33d::eye())
                r.ZoomMatrix = ZoomMatrix::MakeFullView(image.size(), r.ImageSize);
            return r;
        }

        void BlitImageNavigatorTexture(
            const ImageNavigatorParams& params,
            const cv::Mat& image,
            GlTextureCv* outTexture
            )
        {
            cv::Mat imageAdjustedColor = ColorAdjustmentsUtils::Adjust(params.ColorAdjustments, image);
            cv::Mat imageZoomed;

            cv::warpAffine(imageAdjustedColor, imageZoomed,
                           ZoomMatrix::ZoomMatrixToM23(params.ZoomMatrix), params.ImageSize, cv::INTER_NEAREST);

            outTexture->BlitMat(imageZoomed);
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
            if (v1.ImageSize != v2.ImageSize)
                return true;
            if (! ZoomMatrix::IsEqual(v1.ZoomMatrix, v2.ZoomMatrix))
                return true;
            if (! (v1.ColorAdjustments == v2.ColorAdjustments))
                return true;
            return false;
        }


        class ImageNavigatorTextureCache
        {
        public:
            const GlTextureCv & GetTexture(const cv::Mat& image, ImageNavigatorParams& params, bool refresh)
            {
                params.ImageSize = immvision_ImGuiExt::ComputeDisplayImageSize(params.ImageSize, image.size());

                bool needsRefreshTexture = refresh;

                if (mCache.find(&image) == mCache.end())
                {
                    params = InitializeMissingParams(params, image);
                    needsRefreshTexture = true;
                }
                mCache[&image].ImageNavigatorParams = &params;

                auto& cachedData = mCache.at(&image);

                ImageNavigatorParams oldParams = cachedData.OldParams;
                *cachedData.ImageNavigatorParams = params;

                if (ShallRefreshTexture(oldParams, params))
                    needsRefreshTexture = true;
                if (needsRefreshTexture)
                    BlitImageNavigatorTexture(params, image, &cachedData.GlTextureCv);

                if (! ZoomMatrix::IsEqual(oldParams.ZoomMatrix, params.ZoomMatrix))
                    UpdateLinkedZooms(image);

                cachedData.OldParams = params;
                return cachedData.GlTextureCv;
            }

            ImVec2& LastDragDelta(const cv::Mat& image)
            {
                assert(mCache.find(&image) != mCache.end());
                return mCache.at(&image).LastDragDelta;
            }

            char *FilenameEditBuffer(const cv::Mat& image)
            {
                return mCache.at(&image).FilenameEditBuffer.data();
            }

        private:
            // Methods
            void UpdateLinkedZooms(const cv::Mat& image)
            {
                assert(mCache.find(&image) != mCache.end());
                std::string zoomKey = mCache[&image].ImageNavigatorParams->ZoomKey;
                if (zoomKey.empty())
                    return;
                ZoomMatrix::ZoomMatrixType newZoom = mCache[&image].ImageNavigatorParams->ZoomMatrix;
                for (auto& kv : mCache)
                    if ( (kv.second.ImageNavigatorParams->ZoomKey == zoomKey) && (kv.first != &image))
                        kv.second.ImageNavigatorParams->ZoomMatrix = newZoom;
            }

            // members
            struct CachedData
            {
                ImageNavigatorParams* ImageNavigatorParams = nullptr;
                struct ImageNavigatorParams  OldParams;
                GlTextureCv GlTextureCv;
                ImVec2 LastDragDelta;
                std::vector<char> FilenameEditBuffer = std::vector<char>(1000, '\0');
            };
            std::map<const cv::Mat*, CachedData> mCache;
        };
        ImageNavigatorTextureCache gImageNavigatorTextureCache;
    } // namespace ImageNavigatorUtils


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


        std::string _MatPixelColorInfo(const cv::Mat & m, int x, int y)
        {
            if (!cv::Rect(cv::Point(0, 0), m.size()).contains(cv::Point(x, y)))
                return "";
            std::stringstream msg;
            char buffer_color[300];
            if (m.type() == CV_64FC4)
            {
                auto v = m.at<cv::Vec4d>(y, x);
                sprintf(buffer_color, "(%.5G, %.5G, %.5G, %.5G)", v[0], v[1], v[2], v[3]);
            }
            else if (m.type() == CV_64FC3)
            {
                auto v = m.at<cv::Vec3d>(y, x);
                sprintf(buffer_color, "(%.5G, %.5G, %.5G)", v[0], v[1], v[2]);
            }
            else if (m.type() == CV_64FC2)
            {
                auto v = m.at<cv::Vec2d>(y, x);
                sprintf(buffer_color, "(%.5G, %.5G)", v[0], v[1]);
            }
            else if (m.type() == CV_64FC1)
            {
                auto v = m.at<double>(y, x);
                sprintf(buffer_color, "%.5G", v);
            }
            else if (m.type() == CV_32FC4)
            {
                auto v = m.at<cv::Vec4f>(y, x);
                sprintf(buffer_color, "(%.5G, %.5G, %.5G, %.5G)", v[0], v[1], v[2], v[3]);
            }
            else if (m.type() == CV_32FC3)
            {
                auto v = m.at<cv::Vec3f>(y, x);
                sprintf(buffer_color, "(%.5G, %.5G, %.5G)", v[0], v[1], v[2]);
            }
            else if (m.type() == CV_32FC2)
            {
                auto v = m.at<cv::Vec2f>(y, x);
                sprintf(buffer_color, "(%.5G, %.5G)", v[0], v[1]);
            }
            else if (m.type() == CV_32FC1)
            {
                auto v = m.at<float>(y, x);
                sprintf(buffer_color, "%.05G", v);
            }
            else if (m.type() == CV_8UC4)
            {
                auto v = m.at<cv::Vec3b>(y, x);
                sprintf(buffer_color, "(%03u, %03u, %03u, %03u)", (unsigned int)v[0], (unsigned int)v[1], (unsigned int)v[2], (unsigned int)v[3]);
            }
            else if (m.type() == CV_8UC3)
            {
                auto v = m.at<cv::Vec3b>(y, x);
                sprintf(buffer_color, "(%03u, %03u, %03u)", (unsigned int)v[0], (unsigned int)v[1], (unsigned int)v[2]);
            }
            else if (m.type() == CV_8UC2)
            {
                auto v = m.at<cv::Vec2b>(y, x);
                sprintf(buffer_color, "(%03u, %03u)", (unsigned int)v[0], (unsigned int)v[1]);
            }
            else if (m.type() == CV_8UC1)
            {
                auto v = m.at<unsigned char>(y, x);
                sprintf(buffer_color, "%03u", (unsigned int)v);
            }
            else
                buffer_color[0] = '\0';

            msg << " " << buffer_color;
            return msg.str();
        }

        std::string MatPixelInfo(const cv::Mat &image, cv::Point pt)
        {
            std::string info = _MatInfo(image);
            if (cv::Rect(cv::Point(0, 0), image.size()).contains(pt))
            {
                info = info + " X:" + std::to_string(pt.x) + " Y:" + std::to_string(pt.y);
                info = info + " " + _MatPixelColorInfo(image, pt.x, pt.y);
            }
            return info;
        }
    } // namespace MatrixInfoUtils


    namespace ImGuiExt
    {
        const char * LinkedLabel(const char *label, const void* linkedObject)
        {
            //static std::vector<std::string> gLinkedLabels;
            static std::string r;
            std::stringstream ss;
            ss << label << "##" << (size_t)linkedObject;
            //gLinkedLabels.push_back(ss.str());
            //return gLinkedLabels.back().c_str();
            r = ss.str();
            return r.c_str();
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
    } // namespace ImGuiExt


    void ShowPixelColorInfo(const cv::Mat &image, cv::Point pt)
    {
        ImGui::Text("%s", MatrixInfoUtils::MatPixelInfo(image, pt).c_str());

        if (cv::Rect(cv::Point(0, 0), image.size()).contains(pt))
        {
            ImGui::SameLine();
            ImVec2 colorButtonSize(12.f, 12.f);
            if (image.type() == CV_8UC3)
            {
                cv::Vec3b col = image.at<cv::Vec3b>(pt.y, pt.x);
                ImGui::ColorButton(
                    "color",
                    { (float)col[2] / 255.f, (float)col[1] / 255.f, (float)col[0] / 255.f, 1.f },
                    0,
                    colorButtonSize
                    );
            }
            else if (image.type() == CV_8UC4)
            {
                cv::Vec4b col = image.at<cv::Vec4b>(pt.y, pt.x);
                ImGui::ColorButton(
                    "color",
                    { (float)col[2] / 255.f, (float)col[1] / 255.f, (float)col[0] / 255.f, (float)col[3] / 255.f },
                    0,
                    colorButtonSize
                );
            }
            else
                ImGui::NewLine();
        }

    }



#define    ICON_ZOOM_PLUS "(+)"
#define    ICON_ZOOM_MINUS "(-)"
#define    ICON_ZOOM_SCALE_1      "(1)"
#define    ICON_ZOOM_FULLVIEW  "(F)"

    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams& params,
        bool refresh)
    {
        auto UniqueLabel = [&image](const char* label) {
            return ImGuiExt::LinkedLabel(label, &image);
        };

        if (image.empty())
        {
            ImGui::Text("empty image !");
            ImGui::EndGroup();
            return cv::Point2d();
        }

        auto& texture =
            ImageNavigatorUtils::gImageNavigatorTextureCache.GetTexture(image, params, refresh);


        auto WidgetSize = [&params]() -> ImVec2 {
            ImVec2 r((float)params.ImageSize.width, (float)params.ImageSize.height);
            r.y += 80.f;
            if (params.ShowColorAdjustments)
                r.y += 40.;
            if (!params.Legend.empty())
                r.y += 20.;
            return r;
        };

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        int windowFlags = ImGuiWindowFlags_MenuBar;
        ImGui::BeginChild(UniqueLabel("ImageNavigator"), WidgetSize(), true, windowFlags);
        ImGui::PopStyleVar();

        /*
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::MenuItem("Test");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
         */

        if ( ! params.Legend.empty())
            ImGui::Text("%s", params.Legend.c_str());
        
        ImVec2 imageSize(params.ImageSize.width, params.ImageSize.height);
        cv::Point2d mouseLocation = ImGuiExt::DisplayTexture_TrackMouse(texture, imageSize);

        int mouseDragButton = 0;
        bool isItemHovered = ImGui::IsItemHovered();
        bool isMouseDragging = ImGui::IsMouseDragging(mouseDragButton) && isItemHovered;

        ZoomMatrix::ZoomMatrixType& zoomMatrix = params.ZoomMatrix;
        cv::Point2d mouseLocation_originalImage =
            isItemHovered   ?
                ZoomMatrix::Apply(zoomMatrix.inv(), mouseLocation)
            :
                cv::Point2d(-1., -1.);

        cv::Point2d viewportCenter_originalImage = ZoomMatrix::Apply(
            zoomMatrix.inv(),
            cv::Point2d (
                (double)(ImGui::GetItemRectSize().x / 2.f),
                (double)(ImGui::GetItemRectSize().y / 2.f))
        );

        ImVec2 &lastDelta = ImageNavigatorUtils::gImageNavigatorTextureCache.LastDragDelta(image);
        if (!isMouseDragging)
            lastDelta = ImVec2(0.f, 0.f);

        if (isMouseDragging)
        {
            ImVec2 dragDelta = ImGui::GetMouseDragDelta(mouseDragButton);
            ImVec2 dragDeltaDelta(dragDelta.x - lastDelta.x, dragDelta.y - lastDelta.y);
            zoomMatrix =
                zoomMatrix
                * ZoomMatrix::ComputePanMatrix(dragDeltaDelta, zoomMatrix(0, 0));
            lastDelta = dragDelta;
        }

        // Pixel color info
        ShowPixelColorInfo(image, mouseLocation_originalImage);

        // Zoom+ / Zoom- buttons
        {
            ImGui::PushButtonRepeat(true);
            if (ImGui::SmallButton(UniqueLabel(ICON_ZOOM_PLUS " Zoom +")))
                zoomMatrix = zoomMatrix * ZoomMatrix::ComputeZoomMatrix(
                    viewportCenter_originalImage, 1.1);

            ImGui::SameLine();

            if (ImGui::SmallButton(UniqueLabel(ICON_ZOOM_MINUS " Zoom -")))
                zoomMatrix = zoomMatrix * ZoomMatrix::ComputeZoomMatrix(
                    viewportCenter_originalImage, 1. / 1.1);

            ImGui::PopButtonRepeat();
        }
        ImGui::SameLine();
        // Scale1 & Full View Zoom  buttons
        {
            auto scaleOneZoomInfo = ZoomMatrix::MakeScaleOne(image.size(), params.ImageSize);
            auto fullViewZoomInfo = ZoomMatrix::MakeFullView(image.size(), params.ImageSize);
            if (! ZoomMatrix::IsEqual(zoomMatrix, scaleOneZoomInfo))
                if (ImGui::SmallButton(UniqueLabel(ICON_ZOOM_SCALE_1 " Scale 1")))
                    zoomMatrix = scaleOneZoomInfo;

            ImGui::SameLine();

            if (! ZoomMatrix::IsEqual(zoomMatrix,fullViewZoomInfo))
                if (ImGui::SmallButton(UniqueLabel(ICON_ZOOM_FULLVIEW " Full")))
                    zoomMatrix = fullViewZoomInfo;

            ImGui::SameLine();

        }

        // Color Adjustments
        {
            if (!params.ShowColorAdjustments)
            {
                if (ImGui::SmallButton(UniqueLabel("Adjust")))
                    params.ShowColorAdjustments = true;
            }
            else
            {
                ImGui::Text("Adjust");
                ImGui::PushItemWidth(80.);
                immvision_ImGuiExt::SliderDouble(
                    UniqueLabel("k"),
                    &params.ColorAdjustments.Factor,
                    0., 32., "%.3f", ImGuiSliderFlags_Logarithmic);
                ImGui::SameLine();
                ImGui::PushItemWidth(80.);
                immvision_ImGuiExt::SliderDouble(
                    UniqueLabel("Delta"),
                    &params.ColorAdjustments.Delta,
                    0., 255., "%.3f", ImGuiSliderFlags_Logarithmic);
                if (! ColorAdjustmentsUtils::IsNone(params.ColorAdjustments))
                {
                    ImGui::SameLine();
                    if (ImGui::SmallButton(UniqueLabel("reset")))
                        params.ColorAdjustments = ColorAdjustments();
                }


                // Save Image
                {
                    char *filename = ImageNavigatorUtils::gImageNavigatorTextureCache.FilenameEditBuffer(image);
                    ImGui::InputText(UniqueLabel("Filename"), filename, 1000);
                    ImGui::SameLine();
                    if (ImGui::SmallButton(UniqueLabel("save")))
                        cv::imwrite(filename, image);
                }

                ImGui::SameLine();
                if (ImGui::SmallButton(UniqueLabel("hide adjust")))
                    params.ShowColorAdjustments = false;

            }
        }

        ImGui::EndChild();

        return mouseLocation_originalImage;
    }


} // namespace ImmVision