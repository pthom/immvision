// """
// portage de internal_libs/base/cv_drawing_utils.py
// Divers utilitaires pour rendre le dessin avec cv2 plus facile
// et plus joli
// Alors que cv2 dessine par defaut sans antialiasing, les fonctions de ce
// module sont antialiasees.
// """
#pragma once
//#include "rpp_base/ivs_types.h"
//#include "rpp_base/math_utils.h"

#include <opencv2/core/core.hpp>

namespace ImmVision
{
    using Image_RGB = cv::Mat;
    using Image_RGBA = cv::Mat;


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

        Image_RGB overlay_alpha_image_precise(const Image_RGB &background_rgb,
                                              const Image_RGBA &overlay_rgba,
                                              double alpha);


        void line(cv::Mat &image,
                  const cv::Point2d &a,
                  const cv::Point2d &b,
                  cv::Scalar color,
                  int thickness = 1);

        // slow !!! Do not use multiple in time intensive parts
        void line_semi_transparent_slow(cv::Mat &image, const cv::Point2d &a, const cv::Point2d  &b, cv::Scalar color, double alpha, int thickness = 1);


        void ellipse(Image_RGB &image,
                     const cv::Point2d &center,
                     const cv::Size2d &size,
                     const cv::Scalar &color,
                     double angle = 0.,
                     double start_angle = 0.,
                     double end_angle = 360.,
                     int thickness = 1);

        void circle(Image_RGB &image,
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
                                int thickness = 1);

        cv::Mat stack_images_verticaly(const cv::Mat &img1, const cv::Mat &img2);

        cv::Mat stack_images_horizontaly(const cv::Mat &img1, const cv::Mat &img2);

        Image_RGB add_grid_to_image(
            const Image_RGB &image,
            double x_spacing = -1., double y_spacing = -1.,
            cv::Scalar color = Yellow(), double alpha = 0.4);


    }  // namespace CvDrawingUtils
}  // namespace ImmVision
