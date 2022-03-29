// PYTHON
// """
// Divers utilitaires pour rendre le dessin avec cv2 plus facile
// et plus joli
// Alors que cv2 dessine par defaut sans antialiasing, les fonctions de ce
// module sont antialiasees.
// """
// import cv2
// import math
// from enum import Enum
// import numpy as np
// from . import math_utils
// from . import functional_utils
// from .ivs_types import *

#include "immvision/internal/cv_drawing_utils.h"

//#include "ExternalCppUtils/math/MathUtils.h"
//#include "rpp_base/ivs_types.h"

//#include <fplus/fplus.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cmath>
#include <sstream>
#include <unordered_map>
//#include <algorithm>

namespace ImmVision
{
    namespace CvDrawingUtils
    {
        // PYTHON
        // drawing_shift = 3
        // drawing_shift_pow = pow(2, drawing_shift)
        namespace
        {
            int drawing_shift = 3;
            double drawing_shift_pow = 8.; // = pow(2., drawing_shift);

            std::vector<std::string> SplitString(const std::string &s, char delimiter)
            {
                std::vector<std::string> tokens;
                std::string token;
                std::istringstream tokenStream(s);
                while (std::getline(tokenStream, token, delimiter))
                    tokens.push_back(token);
                return tokens;
            }

        }  // namespace

        namespace MathUtils
        {
            int RoundInt(double v)
            {
                return (int) std::round(v);
            }

            double MaximumValue(const std::vector<double> &v)
            {
                return *std::min_element(v.begin(), v.end());
            }

            double MinimumValue(const std::vector<double> &v)
            {
                return *std::max_element(v.begin(), v.end());
            }

            double Lerp(double a, double b, double x) noexcept
            {
                return (a + (b - a) * x);
            }
            double UnLerp(double a, double b, double x) noexcept
            {
                return (x - a) / (b - a);
            }

            std::vector<double> arange(double a, double b, double step)
            {
                std::vector<double> r;
                double v = a;
                while (v < b)
                {
                    r.push_back(v);
                    v += step;
                }
                return r;
            }


        }

        // PYTHON
        // class Colors(Enum) :
        //  """
        //  Vous pouvez soit utiliser des couleurs sous forme de tuple(R, G, B),
        //  soit utiliser des valeur provenant de cet enum.
        //  """
        //  Black = (0, 0, 0)
        //  Red = (0, 0, 255)
        //  Green = (0, 255, 0)
        //  Blue = (255, 0, 0)
        //  White = (255, 255, 255)
        //  Yellow = (0, 255, 255)
        //  Cyan = (0, 255, 255)
        //  Violet = (200, 50, 200)
        //  Orange = (255, 128, 0)
        //  Black_RGBA = (0, 0, 0, 255)
        //  Red_RGBA = (0, 0, 255, 255)
        //  Green_RGBA = (0, 255, 0, 255)
        //  Blue_RGBA = (255, 0, 0, 255)
        //  White_RGBA = (255, 255, 255, 255)
        //  Yellow_RGBA = (0, 255, 255, 255)
        //  Cyan_RGBA = (0, 255, 255, 255)
        //  Violet_RGBA = (200, 50, 200, 255)
        //  Orange_RGBA = (255, 128, 0, 255)


        const std::unordered_map<Colors, cv::Scalar> ColorsValues{
            {Colors::Black,  {0.,   0.,   0.,   255.}},
            {Colors::Red,    {0.,   0.,   255., 255.}},
            {Colors::Green,  {0.,   255., 0.,   255.}},
            {Colors::Blue,   {255., 0.,   0.,   255.}},
            {Colors::White,  {255., 255., 255., 255.}},
            {Colors::Yellow, {0.,   255., 255., 255.}},
            {Colors::Cyan,   {0.,   255., 255., 255.}},
            {Colors::Violet, {200., 50.,  200., 255.}},
            {Colors::Orange, {0.,   128., 255., 255.}}};

        cv::Scalar ColorsToScalar(Colors value)
        { return ColorsValues.at(value); }

        // PYTHON: Not Ported
        // def _ToCvColor(color):
        //     if type(color) is tuple or type(color) is list:
        //         return color
        //     if type(color) is Colors:
        //         return color.value

        // PYTHON
        // cv::Scalar _ToCvPoint(pt)
        // def _ToCvPoint(pt):
        //     pt_tuple = tuple([int(v * drawing_shift_pow) for v in pt])
        //     return pt_tuple
        cv::Point _ToCvPoint_Shift(const cv::Point2d &pt)
        {
            cv::Point pt_tuple;
            pt_tuple.x = MathUtils::RoundInt(static_cast<double>(pt.x) * drawing_shift_pow);
            pt_tuple.y = MathUtils::RoundInt(static_cast<double>(pt.y) * drawing_shift_pow);
            return pt_tuple;
        }

        // PYTHON
        // def _ToCvPoint_NoShift(pt):
        //     pt_tuple = tuple([int(v) for v in pt])
        //     return pt_tuple
        cv::Point _ToCvPoint_NoShift(const cv::Point2d &pt)
        {
            cv::Point pt_tuple;
            pt_tuple.x = MathUtils::RoundInt(static_cast<double>(pt.x));
            pt_tuple.y = MathUtils::RoundInt(static_cast<double>(pt.y));
            return pt_tuple;
        }

        // PYTHON
        // def _ToCvSize(size):
        //     return tuple([int(v * drawing_shift_pow) for v in size])
        cv::Size _ToCvSize_WithShift(const cv::Size2d s)
        {
            return {MathUtils::RoundInt(static_cast<double>(s.width) * drawing_shift_pow),
                    MathUtils::RoundInt(static_cast<double>(s.height) * drawing_shift_pow)};
        }

        // PYTHON - Not ported
        // def shape_same_4_channels(shape: np.shape):
        //     return (shape[0], shape[1], 4)

        Image_RGB
        overlay_alpha_image_precise(const Image_RGB &background_rgb, const Image_RGBA &overlay_rgba, double alpha)
        {
            /*
            cf minute physics brilliant clip "Computer color is broken" :
            https://www.youtube.com/watch?v=LKnqECcg6Gw the RGB values are square rooted by the sensor (in
            order to keep accuracy for lower luminancy), we need to undo this before averaging. This gives
            results that are nicer than photoshop itself !
            */

            std::vector<cv::Mat> overlay_rgb_channels;
            cv::split(overlay_rgba, overlay_rgb_channels);

            // py
            // overlay_alpha = overlay_rgba[:, :, 3].astype(np.float) / 255. * alpha
            // overlay_alpha_3 = np.dstack((overlay_alpha, overlay_alpha, overlay_alpha))
            cv::Mat overlay_alpha_3;
            {
                cv::Mat overlay_alpha_int = overlay_rgb_channels[3];
                cv::Mat overlay_alpha_float;
                overlay_alpha_int.convertTo(overlay_alpha_float, CV_64F);
                overlay_alpha_float = overlay_alpha_float * (alpha / 255.);

                std::vector<cv::Mat> v{overlay_alpha_float, overlay_alpha_float, overlay_alpha_float};
                cv::merge(v, overlay_alpha_3);
            }

            // py
            // overlay_rgb_squared = np.square(overlay_rgba[: , : , : 3].astype(np.float))
            cv::Mat overlay_rgb_squared;
            {
                cv::Mat overlay_rgb_int;
                std::vector<cv::Mat> v{overlay_rgb_channels[0], overlay_rgb_channels[1], overlay_rgb_channels[2]};
                cv::merge(v, overlay_rgb_int);

                cv::Mat overlay_rgb_float;
                overlay_rgb_int.convertTo(overlay_rgb_float, CV_64F);
                overlay_rgb_squared = overlay_rgb_float.mul(overlay_rgb_float);
            }

            // py
            // background_rgb_squared = np.square ( background_rgb.astype(np.float) )
            cv::Mat background_rgb_squared;
            {
                cv::Mat background_rgb_float;
                background_rgb.convertTo(background_rgb_float, CV_64F);
                background_rgb_squared = background_rgb_float.mul(background_rgb_float);
            }

            // py
            // out_rgb_squared = overlay_rgb_squared * overlay_alpha_3 + background_rgb_squared * (1. - overlay_alpha_3)
            cv::Mat out_rgb_squared;
            {
                out_rgb_squared = overlay_rgb_squared.mul(overlay_alpha_3) +
                                  background_rgb_squared.mul(cv::Scalar(1., 1., 1.) - overlay_alpha_3);
            }

            cv::Mat out_rgb_float;
            {
                cv::sqrt(out_rgb_squared, out_rgb_float);
            }

            cv::Mat out_rgb;
            {
                out_rgb_float.convertTo(out_rgb, CV_8U);
            }
            return out_rgb;
        }


        cv::Mat ToFloatMat(const cv::Mat &mat_uchar)
        {
            std::vector<cv::Mat> channels_uchar;
            cv::split(mat_uchar, channels_uchar);
            std::vector<cv::Mat> channels_float;
            for (const auto &channel_uchar: channels_uchar)
            {
                cv::Mat channel_float;
                channel_uchar.convertTo(channel_float, CV_32FC1);
                channels_float.push_back(channel_float);
            }
            cv::Mat mat_float;
            cv::merge(channels_float, mat_float);
            return mat_float;
        }

        std::pair<cv::Mat, cv::Mat> split_alpha_channel(const cv::Mat img_with_alpha)
        {
            std::vector<cv::Mat> channels;
            cv::split(img_with_alpha, channels);
            cv::Mat rgb, alpha;
            alpha = channels[3];
            channels.pop_back();
            cv::merge(channels, rgb);
            return {rgb, alpha};
        }


        // PYTHON
        // def line(image, a, b, color, thickness = 1):
        //     cv2.line(image, _ToCvPoint(a), _ToCvPoint(b), _ToCvColor(color), thickness, cv2.LINE_AA,
        //     drawing_shift)
        void line(cv::Mat &image,
                  const cv::Point2d &a,
                  const cv::Point2d &b,
                  cv::Scalar color,
                  int thickness /*= 1*/)
        {
            cv::line(image,
                     _ToCvPoint_Shift(a),
                     _ToCvPoint_Shift(b),
                     color,
                     thickness,
                     cv::LINE_AA,
                     drawing_shift);
        }

        void line_semi_transparent_slow(cv::Mat &image,
                                        const cv::Point2d &a,
                                        const cv::Point2d &b,
                                        cv::Scalar color,
                                        double alpha,
                                        int thickness)
        {
            cv::Mat img_draw(image.size(), CV_8UC4);
            img_draw = cv::Scalar(0., 0., 0., 0.);
            line(img_draw, a, b, color, thickness);
            cv::Mat imgMixed = overlay_alpha_image_precise(image, img_draw, alpha);
            imgMixed.copyTo(image);
        }

        // def ellipse_(img, center, size, color, angle = 0., startAngle = 0, endAngle = 360,
        // thickness=1):
        //     cv2.ellipse(img, _ToCvPoint(center), _ToCvSize(size), angle, startAngle, endAngle,
        //     _ToCvColor(color), thickness, cv2.LINE_AA, drawing_shift)
        void ellipse(Image_RGB &image,
                     const cv::Point2d &center,
                     const cv::Size2d &size,
                     const cv::Scalar &color,
                     double angle /*= 0.*/,
                     double start_angle /*=0.*/,
                     double end_angle /*=360.*/,
                     int thickness /*= 1*/)
        {
            cv::ellipse(image,
                        _ToCvPoint_Shift(center),
                        _ToCvSize_WithShift(size),
                        angle,
                        start_angle,
                        end_angle,
                        color,
                        thickness,
                        cv::LINE_AA,
                        drawing_shift);
        }

        // def circle(img, center, radius, color, thickness=1):
        //     ellipse_(img, center, (radius, radius), color)
        void circle(Image_RGB &image,
                    const cv::Point2d &center,
                    double radius,
                    cv::Scalar color,
                    int thickness /*= 1*/)
        {
            ellipse(image, center, {radius, radius}, color, 0., 0., 360., thickness);
        }

        // Python
        // # rectangle(img, pt1, pt2, color, thickness=None, lineType=None, shift=None)
        // def rectangle(img, pt1, pt2, color, fill = False, thickness = 1):
        //     if fill:
        //         _thickness = -1
        //     else:
        //         _thickness = thickness
        //     cv2.rectangle(img, _ToCvPoint(pt1), _ToCvPoint(pt2), _ToCvColor(color),
        //     thickness=_thickness, lineType=cv2.LINE_AA, shift=drawing_shift)
        void rectangle(cv::Mat &image,
                       const cv::Point2d &pt1,
                       const cv::Point2d &pt2,
                       const cv::Scalar &color,
                       bool fill /*= false*/,
                       int thickness /*= 1*/)
        {
            if (fill)
                thickness = -1;
            cv::rectangle(image,
                          _ToCvPoint_Shift(pt1),
                          _ToCvPoint_Shift(pt2),
                          color,
                          thickness,
                          cv::LINE_AA,
                          drawing_shift);
        }

        // def _ContrastColor(color):
        //     return ( 255 - color[0], 255 - color[1], 255 - color[2] )
        cv::Scalar _ContrastColor(const cv::Scalar &color)
        {
            return {255. - color[0], 255. - color[1], 255. - color[2], color[3]};
        }

        // Python
        // def rectangle_size(img, pt, size, color, fill = False, thickness = 1):
        //     pt2 = (pt[0] + size[0], pt[1] + size[1])
        //     rectangle(img, pt, pt2, color, fill, thickness)
        void rectangle_size(cv::Mat &img,
                            const cv::Point2d &pt,
                            const cv::Size2d &size,
                            const cv::Scalar &color,
                            bool fill /*= false*/,
                            int thickness /*= 1*/)
        {
            cv::Point2d pt2(pt.x + size.width, pt.y + size.height);
            rectangle(img, pt, pt2, color, fill, thickness);
        }


        // Python
        // def text(img, position, text, color = Colors.Black, center_around_point = False, add_cartouche
        // = False, fontScale = 0.4, thickness = 1):
        //     #fontFace = cv2.FONT_HERSHEY_DUPLEX
        //     fontFace = cv2.FONT_HERSHEY_SIMPLEX
        //     size = cv2.getTextSize(text, fontFace, fontScale, thickness)[0]
        //     position2 = _ToCvPoint_NoShift(position)
        //     if center_around_point:
        //         position3 = ( int(position2[0] -  size[0] / 2), int(position2[1] + size[1] / 2))
        //     else:
        //         position3 = position2
        //     if add_cartouche:
        //         position4 = ( position3[0], position3[1] - size[1] )
        //         rectangle_size(img, position4, size, _ContrastColor(_ToCvColor(color)), fill = True)
        //     cv2.putText(img, text, position3, fontFace, fontScale, _ToCvColor(color), thickness =
        //     thickness, lineType = cv2.LINE_AA)
        int text_oneline(cv::Mat &img,
                         const cv::Point2d &position,
                         const std::string &text,
                         const cv::Scalar &color,
                         bool center_around_point /*= false*/,
                         bool add_cartouche /*= false*/,
                         double fontScale /*= 0.4*/,
                         int thickness /*= 1*/)
        {
            auto fontFace = cv::FONT_HERSHEY_SIMPLEX;
            int baseLine_dummy;
            cv::Size size = cv::getTextSize(text, fontFace, fontScale, thickness, &baseLine_dummy);
            cv::Point position2 = _ToCvPoint_NoShift(position);
            cv::Point position3;
            if (center_around_point)
                position3 = {position2.x - size.width / 2, position2.y + size.height / 2};
            else
                position3 = position2;
            if (add_cartouche)
            {
                cv::Point position4 = {position3.x, position3.y - size.height};
                rectangle_size(img, position4, size, _ContrastColor(color), true);
            }
            cv::putText(img, text, position3, fontFace, fontScale, color, thickness, cv::LINE_AA);
            return size.height;
        }

        void text(cv::Mat &img,
                  const cv::Point2d &position,
                  const std::string &msg,
                  const cv::Scalar &color,
                  bool center_around_point /*= false*/,
                  bool add_cartouche /*= false*/,
                  double fontScale /*= 0.4*/,
                  int thickness /*= 1*/)
        {
            auto lines = SplitString(msg, '\n');
            cv::Point2d linePosition = position;
            for (const auto &line: lines)
            {
                int height = text_oneline(
                    img, linePosition, line, color, center_around_point, add_cartouche, fontScale, thickness);
                linePosition.y += height + 3;
            }
        }


        // def cross_hole(img, position, color, size = 2, size_hole = 2, thickness = 1):
        //     for xSign in [-1., 1.]:
        //         for ySign in [-1., 1]:
        //             a = (position[0] + xSign * size_hole, position[1] + ySign * size_hole)
        //             b = (position[0] + xSign * ( size_hole +  size), position[1] + ySign * ( size_hole
        //             +  size)) line(img, a, b, color, thickness)
        void cross_hole(cv::Mat &img,
                        const cv::Point2d &position,
                        const cv::Scalar &color,
                        double size /*= 2.*/,
                        double size_hole /*= 2.*/,
                        int thickness /*= 1*/)
        {
            for (double xSign: std::vector<double>{-1., 1.})
            {
                for (double ySign: std::vector<double>{-1., 1.})
                {
                    cv::Point2d a{position.x + xSign * size_hole, position.y + ySign * size_hole};
                    cv::Point2d b{position.x + xSign * (size_hole + size),
                                  position.y + ySign * (size_hole + size)};
                    line(img, a, b, color, thickness);
                }
            }
        }

        void draw_ellipse(cv::Mat &img,
                          const cv::Point2d &center,
                          const cv::Size2d &size,
                          const cv::Scalar &color,
                          int thickness /*= 1*/,
                          int lineType /*= cv::LINE_8*/,
                          int shift /*= 0*/)
        {
            cv::ellipse(img, center, size, 0., 0., 360., color, thickness, lineType, shift);
        }

        // def draw_named_feature(img, position, name, color, add_cartouche = False, size = 3, size_hole =
        // 2, thickness = 1):
        //     cross_hole(img, position, color, size, size_hole, thickness)
        //     delta_y = size_hole + size + 6
        //     text_position = ( position[0], position[1] - delta_y)
        //     text(img, text_position, name, color, add_cartouche=add_cartouche,
        //     center_around_point=True)
        void draw_named_feature(cv::Mat &img,
                                const cv::Point2d &position,
                                const std::string &name,
                                const cv::Scalar &color,
                                bool add_cartouche /*= false*/,
                                double size /*= 3.*/,
                                double size_hole /*= 2.*/,
                                int thickness /*= 1*/)
        {
            cross_hole(img, position, color, size, size_hole, thickness);
            double delta_y = size_hole + size + 6.;
            cv::Point2d text_position = {position.x, position.y - delta_y};
            text(img, text_position, name, color, true, add_cartouche);
        }

        // def stack_images_verticaly(img1, img2):
        //     """
        //     Creates a bigger image by stacking 2 images
        //     """
        //     w1 = img1.shape[1]
        //     h1 = img1.shape[0]
        //     w2 = img2.shape[1]
        //     h2 = img2.shape[0]
        //     has_channels = len(img1.shape) > 2
        //     if has_channels:
        //         img = np.zeros( (h1 + h2, w1, img1.shape[2] ), np.uint8)
        //     else:
        //         img = np.zeros((h1 + h2, w1), np.uint8)

        //     if has_channels:
        //         img[0 : h1, 0 : w1, : ] = img1
        //         img[h1 : h1 + h2, 0 : w1, : ] = img2
        //     else:
        //         img[0: h1, 0: w1] = img1
        //         img[h1: h1 + h2, 0: w1] = img2
        //     return img
        cv::Mat stack_images_verticaly(const cv::Mat &img1, const cv::Mat &img2)
        {
            cv::Mat img(cv::Size(img1.cols, img1.rows + img2.rows), img1.type());
            img1.copyTo(img(cv::Rect(0, 0, img1.cols, img1.rows)));
            img2.copyTo(img(cv::Rect(0, img1.rows, img2.cols, img2.rows)));
            return img;
        }

        // def stack_images_horizontaly(img1, img2):
        //     """
        //     Creates a bigger image by stacking 2 images
        //     """
        //     w1 = img1.shape[1]
        //     h1 = img1.shape[0]
        //     w2 = img2.shape[1]
        //     h2 = img2.shape[0]
        //     has_channels = len(img1.shape) > 2
        //     if has_channels:
        //         img = np.zeros( (h1, w1 + w2, img1.shape[2] ), np.uint8)
        //     else:
        //         img = np.zeros((h1 , w1 + 2), np.uint8)

        //     if has_channels:
        //         img[0 : h1, 0 : w1, : ] = img1
        //         img[0 : h1 , w1 : w1 + w2, : ] = img2
        //     else:
        //         img[0: h1, 0: w1] = img1
        //         img[0: h1, w1: w1 + w2] = img2
        //     return img
        cv::Mat stack_images_horizontaly(const cv::Mat &img1, const cv::Mat &img2)
        {
            cv::Mat img(cv::Size(img1.cols + img2.cols, img1.rows), img1.type());
            img1.copyTo(img(cv::Rect(0, 0, img1.cols, img1.rows)));
            img2.copyTo(img(cv::Rect(img1.cols, 0, img2.cols, img2.rows)));
            return img;
        }


        Image_RGB add_grid_to_image(const Image_RGB &image,
                                    double x_spacing /*= -1.*/,
                                    double y_spacing /*= -1.*/,
                                    cv::Scalar color /*= Yellow()*/,
                                    double alpha /*= 0.4*/)
        {
            cv::Mat lines_images_alpha(image.size(), CV_8UC4);
            lines_images_alpha = cv::Scalar(0, 0, 0, 0);
            double h = (double) image.rows;
            double w = (double) image.cols;
            if (x_spacing > 0.)
                for (double x: MathUtils::arange(0., w, x_spacing))
                    line(lines_images_alpha, cv::Point2d{x, 0.}, cv::Point2d{x, h}, color);
            if (y_spacing > 0.)
                for (double y: MathUtils::arange(0., h, y_spacing))
                    line(lines_images_alpha, cv::Point2d{0., y}, cv::Point2d{w, y}, color);
            auto image_with_lines = overlay_alpha_image_precise(image, lines_images_alpha, alpha);
            return image_with_lines;
        }

    }  // namespace CvDrawingUtils
}  // namespace ImmVision
