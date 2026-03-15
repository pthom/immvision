#include "immvision/internal/cv/cv_drawing_utils.h"
#include "immvision/internal/misc/string_utils.h"
#include "immvision/internal/misc/math_utils.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unordered_map>
#include <type_traits>  // std::is_same_v
#include <limits>       // std::numeric_limits
#include <cmath>        // std::sqrt, std::clamp

#ifndef CV_16F // for old versions of OpenCV
#define CV_16F 7
#endif


namespace ImmVision
{
    namespace CvDrawingUtils
    {
        static int drawing_shift = 3;
        static double drawing_shift_pow = 8.; // = pow(2., drawing_shift);


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

            int bgChannels = background_rgb_or_rgba.channels();
            int outType = background_rgb_or_rgba.type(); // preserve input type (3 or 4 channels)
            cv::Mat result(background_rgb_or_rgba.rows, background_rgb_or_rgba.cols, outType);

            for (int y = 0; y < result.rows; y++)
            {
                const uint8_t* bg_row = background_rgb_or_rgba.ptr<uint8_t>(y);
                const uint8_t* ov_row = overlay_rgba.ptr<uint8_t>(y);
                uint8_t* dst_row = result.ptr<uint8_t>(y);

                for (int x = 0; x < result.cols; x++)
                {
                    // Read overlay RGBA
                    double ov_r = ov_row[x * 4];
                    double ov_g = ov_row[x * 4 + 1];
                    double ov_b = ov_row[x * 4 + 2];
                    double ov_a = ov_row[x * 4 + 3] * alpha / 255.0;

                    // Read background RGB (skip alpha if present)
                    double bg_r = bg_row[x * bgChannels];
                    double bg_g = bg_row[x * bgChannels + 1];
                    double bg_b = bg_row[x * bgChannels + 2];

                    // Gamma-correct alpha blending: blend in squared (linear) space, then sqrt
                    double inv_a = 1.0 - ov_a;
                    double out_r = std::sqrt(ov_r * ov_r * ov_a + bg_r * bg_r * inv_a);
                    double out_g = std::sqrt(ov_g * ov_g * ov_a + bg_g * bg_g * inv_a);
                    double out_b = std::sqrt(ov_b * ov_b * ov_a + bg_b * bg_b * inv_a);

                    dst_row[x * bgChannels]     = (uint8_t)std::clamp(out_r, 0.0, 255.0);
                    dst_row[x * bgChannels + 1] = (uint8_t)std::clamp(out_g, 0.0, 255.0);
                    dst_row[x * bgChannels + 2] = (uint8_t)std::clamp(out_b, 0.0, 255.0);
                    if (bgChannels == 4)
                        dst_row[x * 4 + 3] = 255; // full opacity for output
                }
            }

            return result;
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

        // Convert a value of any depth to uint8 (0-255 range)
        template<typename T>
        inline uint8_t to_uint8(T value)
        {
            if constexpr (std::is_same_v<T, uint8_t>)
                return value;
            else if constexpr (std::is_same_v<T, int8_t>)
                return (uint8_t)((int)value + 128);
            else if constexpr (std::is_same_v<T, uint16_t>)
                return (uint8_t)(value / 257);  // maps 0-65535 -> 0-255
            else if constexpr (std::is_same_v<T, int16_t>)
                return (uint8_t)(((int)value + 32768) / 257);
            else if constexpr (std::is_same_v<T, int32_t>)
            {
                // Scale full int32 range to 0-255
                double v = ((double)value - (double)std::numeric_limits<int32_t>::min())
                           * 255.0 / ((double)std::numeric_limits<int32_t>::max() - (double)std::numeric_limits<int32_t>::min());
                return (uint8_t)std::clamp(v, 0.0, 255.0);
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                double v = (double)value * 255.0;
                return (uint8_t)std::clamp(v, 0.0, 255.0);
            }
            else
                return 0;
        }

        // Convert any cv::Mat to RGBA uint8, with per-pixel loops (no cv::cvtColor)
        // Note: isBgrOrder only applies to uint8 images (OpenCV convention).
        // Float/integer images are always treated as RGB order.
        template<typename T>
        cv::Mat converted_to_rgba_typed(const cv::Mat& mat, int nbChannels, bool isBgrOrder)
        {
            // BGR swap only applies to uint8 images; non-uint8 depths are always RGB
            bool swapRB = isBgrOrder && std::is_same_v<T, uint8_t>;

            cv::Mat rgba(mat.rows, mat.cols, CV_8UC4);
            for (int y = 0; y < mat.rows; y++)
            {
                const T* src = mat.ptr<T>(y);
                uint8_t* dst = rgba.ptr<uint8_t>(y);
                for (int x = 0; x < mat.cols; x++)
                {
                    uint8_t r, g, b, a;
                    if (nbChannels == 1)
                    {
                        uint8_t v = to_uint8(src[x]);
                        r = g = b = v;
                        a = 255;
                    }
                    else if (nbChannels == 2)
                    {
                        r = to_uint8(src[x * 2]);
                        g = to_uint8(src[x * 2 + 1]);
                        b = 0;
                        a = 255;
                    }
                    else if (nbChannels == 3)
                    {
                        uint8_t c0 = to_uint8(src[x * 3]);
                        uint8_t c1 = to_uint8(src[x * 3 + 1]);
                        uint8_t c2 = to_uint8(src[x * 3 + 2]);
                        if (swapRB) { r = c2; g = c1; b = c0; }
                        else        { r = c0; g = c1; b = c2; }
                        a = 255;
                    }
                    else // nbChannels == 4
                    {
                        uint8_t c0 = to_uint8(src[x * 4]);
                        uint8_t c1 = to_uint8(src[x * 4 + 1]);
                        uint8_t c2 = to_uint8(src[x * 4 + 2]);
                        uint8_t c3 = to_uint8(src[x * 4 + 3]);
                        if (swapRB) { r = c2; g = c1; b = c0; }
                        else        { r = c0; g = c1; b = c2; }
                        a = c3;
                    }
                    dst[x * 4]     = r;
                    dst[x * 4 + 1] = g;
                    dst[x * 4 + 2] = b;
                    dst[x * 4 + 3] = a;
                }
            }
            return rgba;
        }

        Image_RGBA converted_to_rgba_image(const cv::Mat &inputMat, bool isBgrOrder)
        {
            cv::Mat mat = inputMat.isContinuous() ? inputMat : inputMat.clone();
            int nbChannels = mat.channels();

            switch (mat.depth())
            {
                case CV_8U:  return converted_to_rgba_typed<uint8_t>(mat, nbChannels, isBgrOrder);
                case CV_8S:  return converted_to_rgba_typed<int8_t>(mat, nbChannels, isBgrOrder);
                case CV_16U: return converted_to_rgba_typed<uint16_t>(mat, nbChannels, isBgrOrder);
                case CV_16S: return converted_to_rgba_typed<int16_t>(mat, nbChannels, isBgrOrder);
                case CV_32S: return converted_to_rgba_typed<int32_t>(mat, nbChannels, isBgrOrder);
                case CV_32F: return converted_to_rgba_typed<float>(mat, nbChannels, isBgrOrder);
                case CV_64F: return converted_to_rgba_typed<double>(mat, nbChannels, isBgrOrder);
                default:     throw std::runtime_error("converted_to_rgba_image: unsupported depth");
            }
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
