#include "immvision/internal/cv/cv_drawing_utils.h"
#include "immvision/internal/misc/string_utils.h"
#include "immvision/internal/misc/math_utils.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unordered_map>

#ifndef CV_16F // for old versions of OpenCV
#define CV_16F 7
#endif


namespace ImmVision
{
    namespace CvDrawingUtils
    {
        namespace
        {
            int drawing_shift = 3;
            double drawing_shift_pow = 8.; // = pow(2., drawing_shift);

        }  // namespace


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

        cv::Point _ToCvPoint_Shift(const cv::Point2d &pt)
        {
            cv::Point pt_tuple;
            pt_tuple.x = MathUtils::RoundInt(static_cast<double>(pt.x) * drawing_shift_pow);
            pt_tuple.y = MathUtils::RoundInt(static_cast<double>(pt.y) * drawing_shift_pow);
            return pt_tuple;
        }

        cv::Point _ToCvPoint_NoShift(const cv::Point2d &pt)
        {
            cv::Point pt_tuple;
            pt_tuple.x = MathUtils::RoundInt(static_cast<double>(pt.x));
            pt_tuple.y = MathUtils::RoundInt(static_cast<double>(pt.y));
            return pt_tuple;
        }

        cv::Size _ToCvSize_WithShift(const cv::Size2d s)
        {
            return {MathUtils::RoundInt(static_cast<double>(s.width) * drawing_shift_pow),
                    MathUtils::RoundInt(static_cast<double>(s.height) * drawing_shift_pow)};
        }

        Image_RGB overlay_alpha_image_precise(const cv::Mat &background_rgb_or_rgba, const Image_RGBA &overlay_rgba, double alpha)
        {
            /*
            cf minute physics brilliant clip "Computer color is broken" :
            https://www.youtube.com/watch?v=LKnqECcg6Gw the RGB values are square rooted by the sensor (in
            order to keep accuracy for lower luminancy), we need to undo this before averaging. This gives
            results that are nicer than photoshop itself !
            */
            assert( (background_rgb_or_rgba.type() == CV_8UC3) || (background_rgb_or_rgba.type() == CV_8UC4));
            assert(overlay_rgba.type() == CV_8UC4);

            cv::Mat background_rgb;
            {
                if (background_rgb_or_rgba.channels() == 4)
                    cv::cvtColor(background_rgb_or_rgba, background_rgb, cv::COLOR_BGRA2BGR);
                else if (background_rgb_or_rgba.channels() == 3)
                    background_rgb = background_rgb_or_rgba;
                else
                    throw("Only CV_8UC3 or CV_8UC4 background are supported!");
            }


            std::vector<cv::Mat> overlay_rgb_channels;
            cv::split(overlay_rgba, overlay_rgb_channels);

            cv::Mat overlay_alpha_3;
            {
                cv::Mat overlay_alpha_int = overlay_rgb_channels[3];
                cv::Mat overlay_alpha_float;
                overlay_alpha_int.convertTo(overlay_alpha_float, CV_64F);
                overlay_alpha_float = overlay_alpha_float * (alpha / 255.);

                std::vector<cv::Mat> v{overlay_alpha_float, overlay_alpha_float, overlay_alpha_float};
                cv::merge(v, overlay_alpha_3);
            }

            cv::Mat overlay_rgb_squared;
            {
                cv::Mat overlay_rgb_int;
                std::vector<cv::Mat> v{overlay_rgb_channels[0], overlay_rgb_channels[1], overlay_rgb_channels[2]};
                cv::merge(v, overlay_rgb_int);

                cv::Mat overlay_rgb_float;
                overlay_rgb_int.convertTo(overlay_rgb_float, CV_64F);
                overlay_rgb_squared = overlay_rgb_float.mul(overlay_rgb_float);
            }

            cv::Mat background_rgb_squared;
            {
                cv::Mat background_rgb_float;
                background_rgb.convertTo(background_rgb_float, CV_64F);
                background_rgb_squared = background_rgb_float.mul(background_rgb_float);
            }

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

            if (background_rgb_or_rgba.type() == CV_8UC3)
                return out_rgb;
            else // background_rgb_or_rgba.type() == CV_8UC4
            {
                cv::Mat out_rgba;
                cv::cvtColor(out_rgb, out_rgba, cv::COLOR_BGR2BGRA);
                return out_rgba;
            }
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

        void ellipse(cv::Mat &image,
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

        void circle(cv::Mat &image,
                    const cv::Point2d &center,
                    double radius,
                    cv::Scalar color,
                    int thickness /*= 1*/)
        {
            ellipse(image, center, {radius, radius}, color, 0., 0., 360., thickness);
        }

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

        cv::Scalar _ContrastColor(const cv::Scalar &color)
        {
            return {255. - color[0], 255. - color[1], 255. - color[2], color[3]};
        }

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

        double _text_line_height(double fontScale, int thickness)
        {
            auto fontFace = cv::FONT_HERSHEY_SIMPLEX;
            int baseLine_dummy;
            cv::Size size = cv::getTextSize("ABC", fontFace, fontScale, thickness, &baseLine_dummy);
            return (double)size.height;
        }

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
            auto lines = StringUtils::SplitString(msg, '\n');

            double line_height = _text_line_height(fontScale, thickness) + 3.;
            cv::Point2d linePosition = position;
            linePosition.y -= line_height * (double)(lines.size() - 1.) / 2.;
            for (const auto &line: lines)
            {
                text_oneline(
                    img, linePosition, line, color, center_around_point, add_cartouche, fontScale, thickness);
                linePosition.y += line_height;
            }
        }

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

        void draw_named_feature(cv::Mat &img,
                                const cv::Point2d &position,
                                const std::string &name,
                                const cv::Scalar &color,
                                bool add_cartouche /*= false*/,
                                double size /*= 3.*/,
                                double size_hole /*= 2.*/,
                                int thickness /*= 1*/,
                                double font_scale /*= 0.4*/)
        {
            if (add_cartouche)
                for (auto x : std::vector<double>{-1., 1.})
                    for (auto y : std::vector<double>{-1., 1.})
                        cross_hole(img, position + cv::Point2d(x, y), _ContrastColor(color), size, size_hole, thickness);

            cross_hole(img, position, color, size, size_hole, thickness);
            double delta_y = size_hole + size + 6.;
            cv::Point2d text_position = {position.x, position.y - delta_y};
            text(img, text_position, name, color, true, add_cartouche, font_scale);
        }

        cv::Mat stack_images_vertically(const cv::Mat &img1, const cv::Mat &img2)
        {
            cv::Mat img(cv::Size(img1.cols, img1.rows + img2.rows), img1.type());
            img1.copyTo(img(cv::Rect(0, 0, img1.cols, img1.rows)));
            img2.copyTo(img(cv::Rect(0, img1.rows, img2.cols, img2.rows)));
            return img;
        }

        cv::Mat stack_images_horizontally(const cv::Mat &img1, const cv::Mat &img2)
        {
            cv::Mat img(cv::Size(img1.cols + img2.cols, img1.rows), img1.type());
            img1.copyTo(img(cv::Rect(0, 0, img1.cols, img1.rows)));
            img2.copyTo(img(cv::Rect(img1.cols, 0, img2.cols, img2.rows)));
            return img;
        }

        auto is_depth_unsigned_integer = [](int depth) {
            return ((depth == CV_8U) || (depth == CV_16U));
        };
        auto is_depth_signed_integer = [](int depth) {
            return ((depth == CV_8S) || (depth == CV_16S) || (depth == CV_32S));
        };
        auto is_depth_integer = [](int depth) {
            return is_depth_signed_integer(depth) || is_depth_unsigned_integer(depth);
        };
        auto is_depth_integer_not_uchar(int depth) {
            return is_depth_integer(depth) && (depth != CV_8U);
        }
        auto is_depth_float = [](int depth) {
            return ((depth == CV_16F) || (depth == CV_32F) || (depth == CV_64F));
        };

        Image_RGBA converted_to_rgba_image(const cv::Mat &inputMat, bool isBgrOrBgra)
        {

            cv::Mat mat = inputMat;

            if (!inputMat.isContinuous())
                mat = inputMat.clone();
            if (is_depth_integer_not_uchar(mat.depth()))
            {
                cv::Mat m64;
                inputMat.convertTo(m64, CV_64F);
                mat = m64;
            }


            cv::Mat mat_rgba;
            int nbChannels = mat.channels();
            if (nbChannels == 1)
            {
                int depth = mat.depth(); (void)depth;
                if (mat.depth() == CV_8U)
                    cv::cvtColor(mat, mat_rgba, cv::COLOR_GRAY2BGRA);
                else if (is_depth_float(depth))
                {
                    cv::Mat grey_uchar;
                    cv::Mat float_times_255 = mat * 255.;
                    float_times_255.convertTo(grey_uchar, CV_8UC1);
                    cv::cvtColor(grey_uchar, mat_rgba, cv::COLOR_GRAY2BGRA);
                }
            }
            else if (nbChannels == 2)
            {
                // Add a third channel, with values = 0
                cv::Mat mat3Channels_lastZero;
                {
                    std::vector<cv::Mat> channels;
                    cv::split(inputMat, channels);
                    cv::Mat channel3(channels.front().size(), channels.front().type());
                    channel3 = cv::Scalar(0., 0., 0., 0.);
                    channels.push_back(channel3);
                    cv::merge(channels, mat3Channels_lastZero);
                }
                if ( mat.depth() == CV_8U)
                    cv::cvtColor(mat3Channels_lastZero, mat_rgba, cv::COLOR_BGR2BGRA);
                else if ((mat.depth() == CV_16F) || (mat.depth() == CV_32F) || (mat.depth() == CV_64F))
                {
                    cv::Mat grey_uchar;
                    cv::Mat float_times_255 = mat3Channels_lastZero * 255.;
                    float_times_255.convertTo(grey_uchar, CV_8UC3);
                    cv::cvtColor(grey_uchar, mat_rgba, cv::COLOR_BGR2BGRA);
                }
            }
            else if (nbChannels == 3)
            {
                if (mat.depth() == CV_8U && isBgrOrBgra)
                    cv::cvtColor(mat, mat_rgba, cv::COLOR_BGR2RGBA);
                else if (mat.depth() == CV_8U && !isBgrOrBgra)
                    cv::cvtColor(mat, mat_rgba, cv::COLOR_RGB2RGBA);
                else if ((mat.depth() == CV_16F) || (mat.depth() == CV_32F) || (mat.depth() == CV_64F))
                {
                    cv::Mat grey_uchar;
                    cv::Mat float_times_255 = mat * 255.;
                    float_times_255.convertTo(grey_uchar, CV_8UC3);
                    cv::cvtColor(grey_uchar, mat_rgba, cv::COLOR_RGB2RGBA);
                }
                else
                    throw std::runtime_error("unsupported image format");
            }
            else if (nbChannels == 4)
            {
                if (mat.depth() == CV_8U && isBgrOrBgra)
                    cv::cvtColor(mat, mat_rgba, cv::COLOR_BGRA2RGBA);
                else if (mat.depth() == CV_8U && !isBgrOrBgra)
                    mat_rgba = mat;
                else if ((mat.depth() == CV_16F) || (mat.depth() == CV_32F) || (mat.depth() == CV_64F))
                {
                    cv::Mat grey_uchar;
                    cv::Mat float_times_255 = mat * 255.;
                    float_times_255.convertTo(grey_uchar, CV_8UC4);
                    grey_uchar.copyTo(mat_rgba);
                }
                else
                    throw std::runtime_error("unsupported image format");
            }
            return mat_rgba;
        }

        cv::Mat make_alpha_channel_checkerboard_image(const cv::Size& size, int squareSize)
        {
            cv::Mat r(size, CV_8UC3);
            for (int x = 0; x < size.width; x++)
            {
                for (int y = 0; y < size.height; y++)
                {
                    uchar colorValue = ((x / squareSize + y / squareSize) % 2 == 0) ? 102 : 152;
                    r.at<cv::Vec3b>(y, x) = cv::Vec3b(colorValue, colorValue, colorValue);
                }
            }
            return r;
        }


        void draw_transparent_pixel(
            cv::Mat &img_rgba,
            const cv::Point2d &position,
            const cv::Scalar &color,
            double alpha
        )
        {
            assert(img_rgba.type() == CV_8UC4);

            auto fnLerpScalar = [](cv::Scalar c1, cv::Scalar c2, double k)
            {
                auto fnLerp = [](double x1, double x2, double k2) {
                    return x1 + k2 * (x2 - x1);
                };
                cv::Scalar r(
                    fnLerp(c1[0], c2[0], k),
                    fnLerp(c1[1], c2[1], k),
                    fnLerp(c1[2], c2[2], k),
                    fnLerp(c1[3], c2[3], k)
                );
                return r;
            };

            double xFloor = (int)position.x;
            double kx0 = 1. - (position.x - xFloor);
            double kx1 = 1. - kx0;
            double yFloor = (int)position.y;
            double ky0 = 1. - (position.y - yFloor);
            double ky1 = 1. - ky0;

            std::vector<std::pair<cv::Point2d, double>> positionAndKs {
                { cv::Point2d(0., 0.), kx0 * ky0 },
                { cv::Point2d(1., 0.), kx1 * ky0 },
                { cv::Point2d(0., 1.), kx0 * ky1 },
                { cv::Point2d(1., 1.), kx1 * ky1 }
            };

            cv::Rect roi(cv::Point(0, 0), img_rgba.size());
            for (const auto& kv: positionAndKs)
            {
                cv::Point pos;
                {
                    cv::Point2d delta = kv.first;
                    pos = cv::Point((int)(position.x + delta.x), (int)(position.y + delta.y));
                }
                double k = kv.second;

                if (!roi.contains(pos))
                    continue;

                cv::Scalar oldColor = img_rgba.at<cv::Vec4b>(pos.y, pos.x);
                cv::Scalar dstColor = fnLerpScalar(oldColor, color, alpha * k);
                img_rgba.at<cv::Vec4b>(pos.y, pos.x) = dstColor;
            }
        }


        void draw_grid(
            cv::Mat& img_rgba,
            cv::Scalar lineColor,
            double alpha,
            double x_spacing, double y_spacing,
            double x_start, double y_start,
            double x_end, double y_end
            )
        {
            assert(img_rgba.type() == CV_8UC4);

            for (double y = y_start; y < y_end; y+= y_spacing)
                for (double x = 0.; x < x_end; x+= 1.)
                    draw_transparent_pixel(img_rgba, cv::Point2d(x, y), lineColor, alpha);
            for (double x = x_start; x < x_end; x+= x_spacing)
                for (double y = 0.; y < y_end; y+= 1.)
                    draw_transparent_pixel(img_rgba, cv::Point2d(x, y), lineColor, alpha);
        }

    }  // namespace CvDrawingUtils
}  // namespace ImmVision
