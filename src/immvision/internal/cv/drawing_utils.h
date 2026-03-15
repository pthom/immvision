#pragma once
#include "immvision/immvision_types.h"

namespace ImmVision
{

    namespace DrawingUtils
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

        Color4d ColorsToScalar(Colors value);

        inline Color4d Black()
        { return {0, 0, 0, 255}; }

        inline Color4d Red()
        { return {0, 0, 255, 255}; }

        inline Color4d Green()
        { return {0, 255, 0, 255}; }

        inline Color4d Blue()
        { return {255, 0, 0, 255}; }

        inline Color4d White()
        { return {255, 255, 255, 255}; }

        inline Color4d Yellow()
        { return {0, 255, 255, 255}; }

        inline Color4d Cyan()
        { return {255, 255, 0, 255}; }

        inline Color4d Violet()
        { return {200, 50, 200, 255}; }

        inline Color4d Orange()
        { return {255, 128, 0, 255}; }


        void line(ImageBuffer &image,
                  const Point2d &a,
                  const Point2d &b,
                  Color4d color,
                  int thickness = 1);

        void ellipse(ImageBuffer &image,
                     const Point2d &center,
                     const Size2d &size,
                     const Color4d &color,
                     double angle = 0.,
                     double start_angle = 0.,
                     double end_angle = 360.,
                     int thickness = 1);

        void circle(ImageBuffer &image,
                    const Point2d &center,
                    double radius,
                    Color4d color,
                    int thickness = 1);

        void rectangle(ImageBuffer &image,
                       const Point2d &pt1,
                       const Point2d &pt2,
                       const Color4d &color,
                       bool fill = false,
                       int thickness = 1);


        void rectangle_size(ImageBuffer &img,
                            const Point2d &pt,
                            const Size2d &size,
                            const Color4d &color,
                            bool fill = false,
                            int thickness = 1);

        void text(ImageBuffer &img,
                  const Point2d &position,
                  const std::string &msg,
                  const Color4d &color,
                  bool center_around_point = false,
                  bool add_cartouche = false,
                  double fontScale = 0.4,
                  int thickness = 1);

        void cross_hole(ImageBuffer &img,
                        const Point2d &position,
                        const Color4d &color,
                        double size = 2.,
                        double size_hole = 2.,
                        int thickness = 1);

        void draw_named_feature(ImageBuffer &img,
                                const Point2d &position,
                                const std::string &name,
                                const Color4d &color,
                                bool add_cartouche = false,
                                double size = 3.,
                                double size_hole = 2.,
                                int thickness = 1,
                                double font_scale = 0.4);

        void draw_transparent_pixel(
            ImageBuffer &img_rgba,
            const Point2d &position,
            const Color4d &color,
            double alpha
        );

        void draw_grid(
            ImageBuffer& img_rgba,
            Color4d lineColor,
            double alpha,
            double x_spacing, double y_spacing,
            double x_start, double y_start,
            double x_end, double y_end
        );

        ImageBuffer stack_images_vertically(const ImageBuffer &img1, const ImageBuffer &img2);
        ImageBuffer stack_images_horizontally(const ImageBuffer &img1, const ImageBuffer &img2);

        ImageBuffer make_alpha_channel_checkerboard_image(const Size& size, int squareSize = 30);

        using Image_RGB = ImageBuffer;
        using Image_RGBA = ImageBuffer;

        Image_RGB overlay_alpha_image_precise(const ImageBuffer &background_rgb_or_rgba,
                                              const Image_RGBA &overlay_rgba,
                                              double alpha);
        Image_RGBA converted_to_rgba_image(const ImageBuffer &inputMat, bool isBgrOrder);

    }  // namespace DrawingUtils
}  // namespace ImmVision
