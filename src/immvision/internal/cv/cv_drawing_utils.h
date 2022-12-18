#pragma once
#include <opencv2/core/core.hpp>

namespace ImmVision
{

    namespace CvDrawingUtils
    {
        enum class Colors
        {
            Black,
            Red,
            Green,
            Blue,
            White,
            Yellow,
            Cyan,
            Violet,
            Orange
        };

        cv::Scalar ColorsToScalar(Colors value);

        inline cv::Scalar Black()
        { return {0, 0, 0, 255}; }

        inline cv::Scalar Red()
        { return {0, 0, 255, 255}; }

        inline cv::Scalar Green()
        { return {0, 255, 0, 255}; }

        inline cv::Scalar Blue()
        { return {255, 0, 0, 255}; }

        inline cv::Scalar White()
        { return {255, 255, 255, 255}; }

        inline cv::Scalar Yellow()
        { return {0, 255, 255, 255}; }

        inline cv::Scalar Cyan()
        { return {255, 255, 0, 255}; }

        inline cv::Scalar Violet()
        { return {200, 50, 200, 255}; }

        inline cv::Scalar Orange()
        { return {255, 128, 0, 255}; }


        void line(cv::Mat &image,
                  const cv::Point2d &a,
                  const cv::Point2d &b,
                  cv::Scalar color,
                  int thickness = 1);

        void ellipse(cv::Mat &image,
                     const cv::Point2d &center,
                     const cv::Size2d &size,
                     const cv::Scalar &color,
                     double angle = 0.,
                     double start_angle = 0.,
                     double end_angle = 360.,
                     int thickness = 1);

        void circle(cv::Mat &image,
                    const cv::Point2d &center,
                    double radius,
                    cv::Scalar color,
                    int thickness = 1);

        void rectangle(cv::Mat &image,
                       const cv::Point2d &pt1,
                       const cv::Point2d &pt2,
                       const cv::Scalar &color,
                       bool fill = false,
                       int thickness = 1);


        void rectangle_size(cv::Mat &img,
                            const cv::Point2d &pt,
                            const cv::Size2d &size,
                            const cv::Scalar &color,
                            bool fill = false,
                            int thickness = 1);

        void text(cv::Mat &img,
                  const cv::Point2d &position,
                  const std::string &msg,
                  const cv::Scalar &color,
                  bool center_around_point = false,
                  bool add_cartouche = false,
                  double fontScale = 0.4,
                  int thickness = 1);

        void cross_hole(cv::Mat &img,
                        const cv::Point2d &position,
                        const cv::Scalar &color,
                        double size = 2.,
                        double size_hole = 2.,
                        int thickness = 1);

        void draw_named_feature(cv::Mat &img,
                                const cv::Point2d &position,
                                const std::string &name,
                                const cv::Scalar &color,
                                bool add_cartouche = false,
                                double size = 3.,
                                double size_hole = 2.,
                                int thickness = 1,
                                double font_scale = 0.4);

        void draw_transparent_pixel(
            cv::Mat &img_rgba,
            const cv::Point2d &position,
            const cv::Scalar &color,
            double alpha
        );

        void draw_grid(
            cv::Mat& img_rgba,
            cv::Scalar lineColor,
            double alpha,
            double x_spacing, double y_spacing,
            double x_start, double y_start,
            double x_end, double y_end
        );

        cv::Mat stack_images_vertically(const cv::Mat &img1, const cv::Mat &img2);
        cv::Mat stack_images_horizontally(const cv::Mat &img1, const cv::Mat &img2);

        cv::Mat make_alpha_channel_checkerboard_image(const cv::Size& size, int squareSize = 30);

        using Image_RGB = cv::Mat;
        using Image_RGBA = cv::Mat;

        Image_RGB overlay_alpha_image_precise(const cv::Mat &background_rgb_or_rgba,
                                              const Image_RGBA &overlay_rgba,
                                              double alpha);
        Image_RGBA converted_to_rgba_image(const cv::Mat &inputMat, bool isBgrOrBgra);

    }  // namespace CvDrawingUtils
}  // namespace ImmVision
