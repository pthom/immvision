#include "immvision/internal/drawing/internal_icons.h"
#include "immvision/internal/cv/cv_drawing_utils.h"
#include "immvision/internal/gl/gl_texture.h"
#include "immvision/image.h"
#include "immvision/imgui_imm.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "imgui_internal.h"
#include <map>


namespace ImmVision
{
    namespace Icons
    {
        static cv::Size iconsSizeDraw(200, 200);
        auto ScalePoint = [](cv::Point2d p) {
            return cv::Point2d(p.x * (double) iconsSizeDraw.width, p.y * (double) iconsSizeDraw.height);
        };
        auto ScaleDouble = [](double v) {
            return v * (double) iconsSizeDraw.width;
        };
        auto ScaleInt = [](double v) {
            return (int) (v * (double) iconsSizeDraw.width + 0.5);
        };

        auto PointFromOther = [](cv::Point2d o, double angleDegree, double distance) {
            double m_pi = 3.14159265358979323846;
            double angleRadian = -angleDegree / 180. * m_pi;
            cv::Point2d r(o.x + cos(angleRadian) * distance, o.y + sin(angleRadian) * distance);
            return r;
        };


        cv::Mat MakeMagnifierImage(IconType iconType)
        {
            using namespace ImmVision;
            cv::Mat m(iconsSizeDraw, CV_8UC4);


            // Transparent background
            m = cv::Scalar(0, 0, 0, 0);

            cv::Scalar color(255, 255, 255, 255);
            double radius = 0.3;
            cv::Point2d center(1. - radius * 1.3, radius * 1.2);
            // Draw shadow
            {
                cv::Point2d decal(radius * 0.1, radius * 0.1);
                cv::Scalar color_shadow(127, 127, 127, 255);

                CvDrawingUtils::circle(
                    m, //image,
                    ScalePoint(center + decal),
                    ScaleDouble(radius), //radius
                    color_shadow,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 1.7) + decal),
                    ScalePoint(PointFromOther(center, 225., radius * 1.03) + decal),
                    color_shadow,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 2.3) + decal),
                    ScalePoint(PointFromOther(center, 225., radius * 1.5) + decal),
                    color_shadow,
                    ScaleInt(0.14)
                );
            }
            // Draw magnifier
            {
                CvDrawingUtils::circle(
                    m, //image,
                    ScalePoint(center),
                    ScaleDouble(radius), //radius
                    color,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 1.7)),
                    ScalePoint(PointFromOther(center, 225., radius * 1.03)),
                    color,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 2.3)),
                    ScalePoint(PointFromOther(center, 225., radius * 1.5)),
                    color,
                    ScaleInt(0.14)
                );
            }

            if (iconType == IconType::ZoomPlus)
            {
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 0., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 180., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 90., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 270., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
            }
            if (iconType == IconType::ZoomMinus)
            {
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 0., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 180., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
            }
            if (iconType == IconType::ZoomScaleOne)
            {
                cv::Point2d a = PointFromOther(center, -90., radius * 0.45);
                cv::Point2d b = PointFromOther(center, 90., radius * 0.45);
                a.x += radius * 0.05;
                b.x += radius * 0.05;
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(a),
                    ScalePoint(b),
                    color,
                    ScaleInt(0.06)
                );
                cv::Point2d c(b.x - radius * 0.2, b.y + radius * 0.2);
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(b),
                    ScalePoint(c),
                    color,
                    ScaleInt(0.06)
                );
            }

            return m;
        }


        cv::Mat MakeFullViewImage()
        {
            cv::Mat m(iconsSizeDraw, CV_8UC4);
            m = cv::Scalar(0, 0, 0, 0);

            cv::Scalar color(255, 255, 255, 255);
            double decal = 0.1;
            double length_x = 0.3, length_y = 0.3;
            for (int y = 0; y <= 1; ++y)
            {
                for (int x = 0; x <= 1; ++x)
                {
                    cv::Point2d corner;

                    corner.x = (x == 0) ? decal : 1. - decal;
                    corner.y = (y == 0) ? decal : 1. - decal;
                    double moveX = (x == 0) ? length_x : -length_x;
                    double moveY = (y == 0) ? length_y : -length_y;
                    cv::Point2d pt_x(corner.x + moveX, corner.y);
                    cv::Point2d pt_y(corner.x, corner.y + moveY);
                    int thickness = ScaleInt(0.09);
                    CvDrawingUtils::line(
                        m,
                        ScalePoint(corner),
                        ScalePoint(pt_x),
                        color,
                        thickness
                    );
                    CvDrawingUtils::line(
                        m,
                        ScalePoint(corner),
                        ScalePoint(pt_y),
                        color,
                        thickness
                    );
                }
            }
            return m;
        }

        cv::Mat MakeAdjustLevelsImage()
        {
            cv::Mat m(iconsSizeDraw, CV_8UC4);
            m = cv::Scalar(0, 0, 0, 0);
            cv::Scalar color(255, 255, 255, 255);

            double yMin = 0.15, yMax = 0.8;
            int nbBars = 3;
            for (int bar = 0; bar < nbBars; ++bar)
            {
                double xBar = (double)bar / ((double)(nbBars) + 0.17) + 0.2;
                cv::Point2d a(xBar, yMin);
                cv::Point2d b(xBar, yMax);
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(a),
                    ScalePoint(b),
                    color,
                    ScaleInt(0.08)
                );

                double barWidth = 0.1;
                double yBar = 0.7 - 0.2 * (double)bar;
                cv::Point2d c(a.x - barWidth / 2., yBar);
                cv::Point2d d(a.x + barWidth / 2., yBar);
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(c),
                    ScalePoint(d),
                    color,
                    ScaleInt(0.16)
                );
            }

            return m;
        }


        static std::map<IconType, std::unique_ptr<GlTextureCv>> sIconsTextureCache;
        //static cv::Size gIconSize(20,  20);

        cv::Size IconSize()
        {
            // Make icons size proportionnal to font size
            float k = ImGui::GetFontSize() / 14.5f;
            int size = int(k * 20.f);
            return {size, size};
        }

        ImTextureID GetIcon(IconType iconType)
        {
            if (sIconsTextureCache.find(iconType) == sIconsTextureCache.end())
            {
                cv::Mat m;
                if (iconType == IconType::ZoomFullView)
                    m = MakeFullViewImage();
                else if (iconType == IconType::AdjustLevels)
                    m = MakeAdjustLevelsImage();
                else
                    m = MakeMagnifierImage(iconType);

                cv::Mat resized = m;
                cv::resize(m, resized, cv::Size(IconSize().width * 2, IconSize().height * 2), 0., 0., cv::INTER_AREA);
                auto texture = std::make_unique<GlTextureCv>(resized, true);
                sIconsTextureCache[iconType] = std::move(texture);
            }
            return sIconsTextureCache[iconType]->mImTextureId;
        }

        bool IconButton(IconType iconType, bool disabled)
        {
            ImGui::PushID((int)iconType);
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImU32 backColorEnabled = ImGui::ColorConvertFloat4ToU32(ImVec4 (1.f, 1.f, 1.f, 1.f));
            ImU32 backColorDisabled = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 0.9f, 0.5f));
            ImU32 backColor = disabled ? backColorDisabled : backColorEnabled;
            if (disabled)
                ImGuiImm::PushDisabled();

            // Cannot use InvisibleButton, since it does not handle "Repeat"
            ImVec2 btnSize(ImGui::GetFontSize() * 1.5f, ImGui::GetFontSize() * 1.5f);
            bool clicked = ImGui::Button("##btn", btnSize);

            ImGui::GetWindowDrawList()->AddImage(
                GetIcon(iconType),
                cursorPos,
                {cursorPos.x + (float)IconSize().width, cursorPos.y + (float)IconSize().height},
                ImVec2(0.f, 0.f),
                ImVec2(1.f, 1.f),
                backColor
                );

            if (disabled)
                ImGuiImm::PopDisabled();
            ImGui::PopID();
            return disabled ? false : clicked;
        }


        void DevelPlaygroundGui()
        {
            static cv::Mat mag = MakeMagnifierImage(IconType::ZoomScaleOne);
            static cv::Mat img = MakeAdjustLevelsImage();

            static ImmVision::ImageParams imageParams1;
            imageParams1.ImageDisplaySize = {400, 400};
            ImmVision::Image("test", mag, &imageParams1);

            ImGui::SameLine();

            static ImmVision::ImageParams imageParams2;
            imageParams2.ImageDisplaySize = {400, 400};
            ImmVision::Image("test2", img, &imageParams2);

            ImVec2 iconSize(15.f, 15.f);
            ImGui::ImageButton(GetIcon(IconType::ZoomScaleOne), iconSize);
            ImGui::ImageButton(GetIcon(IconType::ZoomPlus), iconSize);
            ImGui::ImageButton(GetIcon(IconType::ZoomMinus), iconSize);
            ImGui::ImageButton(GetIcon(IconType::ZoomFullView), iconSize);
            ImGui::ImageButton(GetIcon(IconType::AdjustLevels), iconSize);
        }

        void ClearIconsTextureCache()
        {
            Icons::sIconsTextureCache.clear();
        }

} // namespace Icons


} // namespace ImmVision
