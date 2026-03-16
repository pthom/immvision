#include "immvision/internal/cv/drawing_utils.h"
#include "immvision/internal/misc/string_utils.h"
#include "immvision/internal/misc/math_utils.h"
#include "immvision/internal/misc/parallel_for.h"

#include <cassert>
#include <unordered_map>
#include <type_traits>  // std::is_same_v
#include <limits>       // std::numeric_limits
#include <cmath>        // std::sqrt, std::clamp
#include <stdexcept>    // std::runtime_error

namespace ImmVision
{
    namespace DrawingUtils
    {
        static int drawing_shift = 3;
        static double drawing_shift_pow = 8.; // = pow(2., drawing_shift);


        const std::unordered_map<Colors, Color4d> ColorsValues{
            {Colors::Black,  {0.,   0.,   0.,   255.}},
            {Colors::Red,    {0.,   0.,   255., 255.}},
            {Colors::Green,  {0.,   255., 0.,   255.}},
            {Colors::Blue,   {255., 0.,   0.,   255.}},
            {Colors::White,  {255., 255., 255., 255.}},
            {Colors::Yellow, {0.,   255., 255., 255.}},
            {Colors::Cyan,   {0.,   255., 255., 255.}},
            {Colors::Violet, {200., 50.,  200., 255.}},
            {Colors::Orange, {0.,   128., 255., 255.}}};

        Color4d ColorsToScalar(Colors value)
        { return ColorsValues.at(value); }

        Point _ToPoint_Shift(const Point2d &pt)
        {
            Point pt_tuple;
            pt_tuple.x = MathUtils::RoundInt(static_cast<double>(pt.x) * drawing_shift_pow);
            pt_tuple.y = MathUtils::RoundInt(static_cast<double>(pt.y) * drawing_shift_pow);
            return pt_tuple;
        }

        Point _ToPoint_NoShift(const Point2d &pt)
        {
            Point pt_tuple;
            pt_tuple.x = MathUtils::RoundInt(static_cast<double>(pt.x));
            pt_tuple.y = MathUtils::RoundInt(static_cast<double>(pt.y));
            return pt_tuple;
        }

        Size _ToSize_WithShift(const Size2d s)
        {
            return {MathUtils::RoundInt(static_cast<double>(s.width) * drawing_shift_pow),
                    MathUtils::RoundInt(static_cast<double>(s.height) * drawing_shift_pow)};
        }

        Image_RGB overlay_alpha_image_precise(const ImageBuffer &background_rgb_or_rgba, const Image_RGBA &overlay_rgba, double alpha)
        {
            /*
            cf minute physics brilliant clip "Computer color is broken" :
            https://www.youtube.com/watch?v=LKnqECcg6Gw the RGB values are square rooted by the sensor (in
            order to keep accuracy for lower luminancy), we need to undo this before averaging. This gives
            results that are nicer than photoshop itself !
            */
            assert( (background_rgb_or_rgba.depth == ImageDepth::uint8 && (background_rgb_or_rgba.channels == 3 || background_rgb_or_rgba.channels == 4)));
            assert(overlay_rgba.depth == ImageDepth::uint8 && overlay_rgba.channels == 4);

            int bgChannels = background_rgb_or_rgba.channels;
            ImageBuffer result = ImageBuffer::Zeros(background_rgb_or_rgba.width, background_rgb_or_rgba.height, bgChannels, ImageDepth::uint8);

            parallel_for(0, result.height, [&](int y)
            {
                const uint8_t* bg_row = background_rgb_or_rgba.ptr<uint8_t>(y);
                const uint8_t* ov_row = overlay_rgba.ptr<uint8_t>(y);
                uint8_t* dst_row = result.ptr<uint8_t>(y);

                for (int x = 0; x < result.width; x++)
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
            });

            return result;
        }


        ImageBuffer ToFloatMat(const ImageBuffer &mat_uchar)
        {
            int ch = mat_uchar.channels;
            ImageBuffer mat_float = ImageBuffer::Zeros(mat_uchar.width, mat_uchar.height, ch, ImageDepth::float32);
            for (int y = 0; y < mat_uchar.height; y++)
            {
                const uint8_t* src = mat_uchar.ptr<uint8_t>(y);
                float* dst = mat_float.ptr<float>(y);
                for (int x = 0; x < mat_uchar.width * ch; x++)
                    dst[x] = (float)src[x];
            }
            return mat_float;
        }

        std::pair<ImageBuffer, ImageBuffer> split_alpha_channel(const ImageBuffer img_with_alpha)
        {
            assert(img_with_alpha.channels == 4);
            ImageBuffer rgb = ImageBuffer::Zeros(img_with_alpha.width, img_with_alpha.height, 3, ImageDepth::uint8);
            ImageBuffer alpha_ch = ImageBuffer::Zeros(img_with_alpha.width, img_with_alpha.height, 1, ImageDepth::uint8);
            for (int y = 0; y < img_with_alpha.height; y++)
            {
                const uint8_t* src = img_with_alpha.ptr<uint8_t>(y);
                uint8_t* dst_rgb = rgb.ptr<uint8_t>(y);
                uint8_t* dst_a = alpha_ch.ptr<uint8_t>(y);
                for (int x = 0; x < img_with_alpha.width; x++)
                {
                    dst_rgb[x * 3]     = src[x * 4];
                    dst_rgb[x * 3 + 1] = src[x * 4 + 1];
                    dst_rgb[x * 3 + 2] = src[x * 4 + 2];
                    dst_a[x]           = src[x * 4 + 3];
                }
            }
            return {rgb, alpha_ch};
        }

        // Set a pixel in an image with bounds checking
        inline void _SetPixel(ImageBuffer& img, int x, int y, const Color4d& color)
        {
            if (x < 0 || x >= img.width || y < 0 || y >= img.height) return;
            int ch = img.channels;
            uint8_t* p = img.ptr<uint8_t>(y) + x * ch;
            for (int c = 0; c < ch && c < 4; c++)
                p[c] = (uint8_t)std::clamp(color[c], 0.0, 255.0);
        }

        // Bresenham line (no anti-aliasing, integer coordinates)
        void _DrawLineBresenham(ImageBuffer& img, int x0, int y0, int x1, int y1, const Color4d& color, int thickness)
        {
            int dx = std::abs(x1 - x0), dy = std::abs(y1 - y0);
            int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
            int err = dx - dy;
            int half_t = thickness / 2;
            while (true)
            {
                for (int ty = -half_t; ty <= half_t; ty++)
                    for (int tx = -half_t; tx <= half_t; tx++)
                        _SetPixel(img, x0 + tx, y0 + ty, color);
                if (x0 == x1 && y0 == y1) break;
                int e2 = 2 * err;
                if (e2 > -dy) { err -= dy; x0 += sx; }
                if (e2 <  dx) { err += dx; y0 += sy; }
            }
        }

        void line(ImageBuffer &image,
                  const Point2d &a,
                  const Point2d &b,
                  Color4d color,
                  int thickness /*= 1*/)
        {
            _DrawLineBresenham(image,
                MathUtils::RoundInt(a.x), MathUtils::RoundInt(a.y),
                MathUtils::RoundInt(b.x), MathUtils::RoundInt(b.y),
                color, thickness);
        }

        // Midpoint ellipse (no anti-aliasing)
        void _DrawEllipse(ImageBuffer& img, int cx, int cy, int rx, int ry, const Color4d& color, int thickness)
        {
            if (rx <= 0 || ry <= 0) return;
            int half_t = thickness / 2;
            auto plot = [&](int x, int y) {
                for (int ty = -half_t; ty <= half_t; ty++)
                    for (int tx = -half_t; tx <= half_t; tx++)
                        _SetPixel(img, x + tx, y + ty, color);
            };
            // Midpoint ellipse algorithm
            int x = 0, y = ry;
            long long rx2 = (long long)rx * rx, ry2 = (long long)ry * ry;
            long long p1 = ry2 - rx2 * ry + rx2 / 4;
            while (2 * ry2 * x <= 2 * rx2 * y)
            {
                plot(cx + x, cy + y); plot(cx - x, cy + y);
                plot(cx + x, cy - y); plot(cx - x, cy - y);
                x++;
                if (p1 < 0) p1 += 2 * ry2 * x + ry2;
                else { y--; p1 += 2 * ry2 * x - 2 * rx2 * y + ry2; }
            }
            long long p2 = ry2 * (x * 2 + 1) * (x * 2 + 1) / 4 + rx2 * ((long long)y - 1) * (y - 1) - rx2 * ry2;
            while (y >= 0)
            {
                plot(cx + x, cy + y); plot(cx - x, cy + y);
                plot(cx + x, cy - y); plot(cx - x, cy - y);
                y--;
                if (p2 > 0) p2 -= 2 * rx2 * y + rx2;
                else { x++; p2 += 2 * ry2 * x - 2 * rx2 * y + rx2; }
            }
        }

        void ellipse(ImageBuffer &image,
                     const Point2d &center,
                     const Size2d &size,
                     const Color4d &color,
                     double angle /*= 0.*/,
                     double start_angle /*=0.*/,
                     double end_angle /*=360.*/,
                     int thickness /*= 1*/)
        {
            (void)angle; (void)start_angle; (void)end_angle; // simplified: always full ellipse
            _DrawEllipse(image,
                MathUtils::RoundInt(center.x), MathUtils::RoundInt(center.y),
                MathUtils::RoundInt(size.width), MathUtils::RoundInt(size.height),
                color, thickness);
        }

        void circle(ImageBuffer &image,
                    const Point2d &center,
                    double radius,
                    Color4d color,
                    int thickness /*= 1*/)
        {
            ellipse(image, center, {radius, radius}, color, 0., 0., 360., thickness);
        }

        void rectangle(ImageBuffer &image,
                       const Point2d &pt1,
                       const Point2d &pt2,
                       const Color4d &color,
                       bool fill /*= false*/,
                       int thickness /*= 1*/)
        {
            int x0 = MathUtils::RoundInt(pt1.x), y0 = MathUtils::RoundInt(pt1.y);
            int x1 = MathUtils::RoundInt(pt2.x), y1 = MathUtils::RoundInt(pt2.y);
            if (x0 > x1) std::swap(x0, x1);
            if (y0 > y1) std::swap(y0, y1);

            if (fill)
            {
                for (int y = y0; y <= y1; y++)
                    for (int x = x0; x <= x1; x++)
                        _SetPixel(image, x, y, color);
            }
            else
            {
                _DrawLineBresenham(image, x0, y0, x1, y0, color, thickness);
                _DrawLineBresenham(image, x1, y0, x1, y1, color, thickness);
                _DrawLineBresenham(image, x1, y1, x0, y1, color, thickness);
                _DrawLineBresenham(image, x0, y1, x0, y0, color, thickness);
            }
        }

        Color4d _ContrastColor(const Color4d &color)
        {
            return {255. - color[0], 255. - color[1], 255. - color[2], color[3]};
        }

        void rectangle_size(ImageBuffer &img,
                            const Point2d &pt,
                            const Size2d &size,
                            const Color4d &color,
                            bool fill /*= false*/,
                            int thickness /*= 1*/)
        {
            Point2d pt2(pt.x + size.width, pt.y + size.height);
            rectangle(img, pt, pt2, color, fill, thickness);
        }

        // =====================================================================
        // Tiny 5x7 bitmap font for pixel value annotations
        // Covers ASCII 32-127 (digits, letters, punctuation)
        // Each glyph is stored as 7 bytes (rows), 5 bits per row (MSB = left)
        // =====================================================================
        // =====================================================================
        // 8x13 bitmap font (classic X11 "fixed" style)
        // Covers ASCII 32-127. Each glyph is 8 bits wide x 13 rows.
        // =====================================================================
        namespace BitmapFont
        {
            static const int GLYPH_W = 8;
            static const int GLYPH_H = 13;

            // Font data: 96 glyphs (ASCII 32-127), 13 bytes each
            // Based on the classic 8x13 fixed-width bitmap font
            // clang-format off
            static const uint8_t glyphs[96][13] = {
                {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 ' '
                {0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00}, // 33 '!'
                {0x00,0x00,0x66,0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 34 '"'
                {0x00,0x00,0x00,0x6C,0xFE,0x6C,0x6C,0x6C,0xFE,0x6C,0x00,0x00,0x00}, // 35 '#'
                {0x00,0x18,0x18,0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x18,0x18,0x00}, // 36 '$'
                {0x00,0x00,0x00,0xC2,0xC6,0x0C,0x18,0x30,0x66,0xC6,0x86,0x00,0x00}, // 37 '%'
                {0x00,0x00,0x38,0x6C,0x6C,0x38,0x76,0xDC,0xCC,0xCC,0x76,0x00,0x00}, // 38 '&'
                {0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 39 '''
                {0x00,0x00,0x0C,0x18,0x30,0x30,0x30,0x30,0x30,0x18,0x0C,0x00,0x00}, // 40 '('
                {0x00,0x00,0x30,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x30,0x00,0x00}, // 41 ')'
                {0x00,0x00,0x00,0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00,0x00,0x00}, // 42 '*'
                {0x00,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,0x00,0x00}, // 43 '+'
                {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00}, // 44 ','
                {0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00}, // 45 '-'
                {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00}, // 46 '.'
                {0x00,0x00,0x02,0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00,0x00,0x00}, // 47 '/'
                {0x00,0x00,0x7C,0xC6,0xCE,0xDE,0xF6,0xE6,0xC6,0xC6,0x7C,0x00,0x00}, // 48 '0'
                {0x00,0x00,0x18,0x38,0x78,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00}, // 49 '1'
                {0x00,0x00,0x7C,0xC6,0x06,0x0C,0x18,0x30,0x60,0xC6,0xFE,0x00,0x00}, // 50 '2'
                {0x00,0x00,0x7C,0xC6,0x06,0x06,0x3C,0x06,0x06,0xC6,0x7C,0x00,0x00}, // 51 '3'
                {0x00,0x00,0x0C,0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x1E,0x00,0x00}, // 52 '4'
                {0x00,0x00,0xFE,0xC0,0xC0,0xFC,0x06,0x06,0x06,0xC6,0x7C,0x00,0x00}, // 53 '5'
                {0x00,0x00,0x38,0x60,0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0x7C,0x00,0x00}, // 54 '6'
                {0x00,0x00,0xFE,0xC6,0x06,0x0C,0x18,0x30,0x30,0x30,0x30,0x00,0x00}, // 55 '7'
                {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0xC6,0xC6,0xC6,0x7C,0x00,0x00}, // 56 '8'
                {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7E,0x06,0x06,0x0C,0x78,0x00,0x00}, // 57 '9'
                {0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00}, // 58 ':'
                {0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x30,0x00,0x00}, // 59 ';'
                {0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00}, // 60 '<'
                {0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // 61 '='
                {0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00}, // 62 '>'
                {0x00,0x00,0x7C,0xC6,0xC6,0x0C,0x18,0x18,0x00,0x18,0x18,0x00,0x00}, // 63 '?'
                {0x00,0x00,0x7C,0xC6,0xC6,0xDE,0xDE,0xDE,0xDC,0xC0,0x7C,0x00,0x00}, // 64 '@'
                {0x00,0x00,0x10,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,0x00}, // 65 'A'
                {0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x66,0x66,0x66,0xFC,0x00,0x00}, // 66 'B'
                {0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xC0,0xC2,0x66,0x3C,0x00,0x00}, // 67 'C'
                {0x00,0x00,0xF8,0x6C,0x66,0x66,0x66,0x66,0x66,0x6C,0xF8,0x00,0x00}, // 68 'D'
                {0x00,0x00,0xFE,0x66,0x62,0x68,0x78,0x68,0x62,0x66,0xFE,0x00,0x00}, // 69 'E'
                {0x00,0x00,0xFE,0x66,0x62,0x68,0x78,0x68,0x60,0x60,0xF0,0x00,0x00}, // 70 'F'
                {0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xDE,0xC6,0x66,0x3A,0x00,0x00}, // 71 'G'
                {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0xC6,0x00,0x00}, // 72 'H'
                {0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00}, // 73 'I'
                {0x00,0x00,0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00,0x00}, // 74 'J'
                {0x00,0x00,0xE6,0x66,0x6C,0x6C,0x78,0x6C,0x6C,0x66,0xE6,0x00,0x00}, // 75 'K'
                {0x00,0x00,0xF0,0x60,0x60,0x60,0x60,0x60,0x62,0x66,0xFE,0x00,0x00}, // 76 'L'
                {0x00,0x00,0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0xC6,0xC6,0x00,0x00}, // 77 'M'
                {0x00,0x00,0xC6,0xE6,0xF6,0xFE,0xDE,0xCE,0xC6,0xC6,0xC6,0x00,0x00}, // 78 'N'
                {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00}, // 79 'O'
                {0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x60,0x60,0x60,0xF0,0x00,0x00}, // 80 'P'
                {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xD6,0xDE,0x7C,0x0C,0x0E,0x00}, // 81 'Q'
                {0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x6C,0x66,0x66,0xE6,0x00,0x00}, // 82 'R'
                {0x00,0x00,0x7C,0xC6,0xC6,0x60,0x38,0x0C,0xC6,0xC6,0x7C,0x00,0x00}, // 83 'S'
                {0x00,0x00,0x7E,0x7E,0x5A,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00}, // 84 'T'
                {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00}, // 85 'U'
                {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00,0x00}, // 86 'V'
                {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xD6,0xD6,0xFE,0xEE,0xC6,0x00,0x00}, // 87 'W'
                {0x00,0x00,0xC6,0xC6,0x6C,0x38,0x38,0x6C,0xC6,0xC6,0xC6,0x00,0x00}, // 88 'X'
                {0x00,0x00,0x66,0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x3C,0x00,0x00}, // 89 'Y'
                {0x00,0x00,0xFE,0xC6,0x8C,0x18,0x30,0x60,0xC2,0xC6,0xFE,0x00,0x00}, // 90 'Z'
                {0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,0x00}, // 91 '['
                {0x00,0x00,0x80,0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00,0x00,0x00}, // 92 '\'
                {0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00}, // 93 ']'
                {0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 94 '^'
                {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00}, // 95 '_'
                {0x00,0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 96 '`'
                {0x00,0x00,0x00,0x00,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0x76,0x00,0x00}, // 97 'a'
                {0x00,0x00,0xE0,0x60,0x60,0x78,0x6C,0x66,0x66,0x66,0xDC,0x00,0x00}, // 98 'b'
                {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC0,0xC0,0xC6,0x7C,0x00,0x00}, // 99 'c'
                {0x00,0x00,0x1C,0x0C,0x0C,0x3C,0x6C,0xCC,0xCC,0xCC,0x76,0x00,0x00}, // 100 'd'
                {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xFE,0xC0,0xC6,0x7C,0x00,0x00}, // 101 'e'
                {0x00,0x00,0x1C,0x36,0x30,0x30,0x7C,0x30,0x30,0x30,0x78,0x00,0x00}, // 102 'f'
                {0x00,0x00,0x00,0x00,0x00,0x76,0xCC,0xCC,0xCC,0x7C,0x0C,0xCC,0x78}, // 103 'g'
                {0x00,0x00,0xE0,0x60,0x60,0x6C,0x76,0x66,0x66,0x66,0xE6,0x00,0x00}, // 104 'h'
                {0x00,0x00,0x18,0x18,0x00,0x38,0x18,0x18,0x18,0x18,0x3C,0x00,0x00}, // 105 'i'
                {0x00,0x00,0x06,0x06,0x00,0x0E,0x06,0x06,0x06,0x06,0x66,0x66,0x3C}, // 106 'j'
                {0x00,0x00,0xE0,0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00,0x00}, // 107 'k'
                {0x00,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00}, // 108 'l'
                {0x00,0x00,0x00,0x00,0x00,0xEC,0xFE,0xD6,0xD6,0xD6,0xD6,0x00,0x00}, // 109 'm'
                {0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x66,0x00,0x00}, // 110 'n'
                {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00}, // 111 'o'
                {0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,0x66,0x7C,0x60,0x60,0xF0}, // 112 'p'
                {0x00,0x00,0x00,0x00,0x00,0x76,0xCC,0xCC,0xCC,0x7C,0x0C,0x0C,0x1E}, // 113 'q'
                {0x00,0x00,0x00,0x00,0x00,0xDC,0x76,0x60,0x60,0x60,0xF0,0x00,0x00}, // 114 'r'
                {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0x70,0x1C,0xC6,0x7C,0x00,0x00}, // 115 's'
                {0x00,0x00,0x10,0x30,0x30,0xFC,0x30,0x30,0x30,0x36,0x1C,0x00,0x00}, // 116 't'
                {0x00,0x00,0x00,0x00,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00}, // 117 'u'
                {0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00,0x00}, // 118 'v'
                {0x00,0x00,0x00,0x00,0x00,0xC6,0xD6,0xD6,0xFE,0xEE,0xC6,0x00,0x00}, // 119 'w'
                {0x00,0x00,0x00,0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00,0x00}, // 120 'x'
                {0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xC6,0xC6,0x7E,0x06,0x0C,0xF8}, // 121 'y'
                {0x00,0x00,0x00,0x00,0x00,0xFE,0x8C,0x18,0x30,0x62,0xFE,0x00,0x00}, // 122 'z'
                {0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x0E,0x00,0x00}, // 123 '{'
                {0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x00,0x00}, // 124 '|'
                {0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,0x18,0x18,0x70,0x00,0x00}, // 125 '}'
                {0x00,0x00,0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 126 '~'
                {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 127 DEL
            };
            // clang-format on

            // Get glyph width and height for a string (in pixels, before scaling)
            inline Size getTextSize(const std::string& text)
            {
                return {(int)text.size() * GLYPH_W, GLYPH_H};
            }

            // Draw a string onto an ImageBuffer at integer position
            inline void putText(ImageBuffer& img, const std::string& text, Point pos,
                                const Color4d& color, int scale = 1)
            {
                for (size_t ci = 0; ci < text.size(); ci++)
                {
                    int c = (unsigned char)text[ci];
                    if (c < 32 || c > 127) c = '?';
                    const uint8_t* glyph = glyphs[c - 32];
                    int gx0 = pos.x + (int)ci * GLYPH_W * scale;
                    for (int gy = 0; gy < GLYPH_H; gy++)
                    {
                        uint8_t row = glyph[gy];
                        for (int gx = 0; gx < GLYPH_W; gx++)
                        {
                            if (row & (0x80 >> gx))
                            {
                                for (int sy = 0; sy < scale; sy++)
                                    for (int sx = 0; sx < scale; sx++)
                                        _SetPixel(img, gx0 + gx * scale + sx, pos.y + gy * scale + sy, color);
                            }
                        }
                    }
                }
            }
        } // namespace BitmapFont

        double _text_line_height(double fontScale, int /*thickness*/)
        {
            // fontScale=0.4 (default) -> scale=1, height=13px
            // fontScale=0.8 -> scale=2, height=26px
            int scale = std::max(1, MathUtils::RoundInt(fontScale * 2.5));
            return (double)(BitmapFont::GLYPH_H * scale);
        }

        int text_oneline(ImageBuffer &img,
                         const Point2d &position,
                         const std::string &text,
                         const Color4d &color,
                         bool center_around_point /*= false*/,
                         bool add_cartouche /*= false*/,
                         double fontScale /*= 0.4*/,
                         int /*thickness = 1*/)
        {
            int scale = std::max(1, MathUtils::RoundInt(fontScale * 2.5));
            Size size = BitmapFont::getTextSize(text);
            size.width *= scale;
            size.height *= scale;

            int pad = scale; // 1px padding at each side, scaled
            Point pos = _ToPoint_NoShift(position);
            if (center_around_point)
                pos = {pos.x - (size.width + 2 * pad) / 2, pos.y - size.height / 2};

            if (add_cartouche)
            {
                Point cartPos = {pos.x, pos.y};
                rectangle_size(img, cartPos, Size2d(size.width + 2 * pad, size.height), _ContrastColor(color), true);
            }
            BitmapFont::putText(img, text, {pos.x + 2 * pad, pos.y}, color, scale);
            return size.height;
        }

        void text(ImageBuffer &img,
                  const Point2d &position,
                  const std::string &msg,
                  const Color4d &color,
                  bool center_around_point /*= false*/,
                  bool add_cartouche /*= false*/,
                  double fontScale /*= 0.4*/,
                  int thickness /*= 1*/)
        {
            auto lines = StringUtils::SplitString(msg, '\n');

            double line_height = _text_line_height(fontScale, thickness) + 3.;
            Point2d linePosition = position;
            linePosition.y -= line_height * (double)(lines.size() - 1.) / 2.;
            for (const auto &line: lines)
            {
                text_oneline(
                    img, linePosition, line, color, center_around_point, add_cartouche, fontScale, thickness);
                linePosition.y += line_height;
            }
        }

        void cross_hole(ImageBuffer &img,
                        const Point2d &position,
                        const Color4d &color,
                        double size /*= 2.*/,
                        double size_hole /*= 2.*/,
                        int thickness /*= 1*/)
        {
            for (double xSign: std::vector<double>{-1., 1.})
            {
                for (double ySign: std::vector<double>{-1., 1.})
                {
                    Point2d a{position.x + xSign * size_hole, position.y + ySign * size_hole};
                    Point2d b{position.x + xSign * (size_hole + size),
                                  position.y + ySign * (size_hole + size)};
                    line(img, a, b, color, thickness);
                }
            }
        }

        void draw_ellipse(ImageBuffer &img,
                          const Point2d &center,
                          const Size2d &size,
                          const Color4d &color,
                          int thickness /*= 1*/,
                          int /*lineType*/,
                          int /*shift*/)
        {
            ellipse(img, center, size, color, 0., 0., 360., thickness);
        }

        void draw_named_feature(ImageBuffer &img,
                                const Point2d &position,
                                const std::string &name,
                                const Color4d &color,
                                bool add_cartouche /*= false*/,
                                double size /*= 3.*/,
                                double size_hole /*= 2.*/,
                                int thickness /*= 1*/,
                                double font_scale /*= 0.4*/)
        {
            if (add_cartouche)
                for (auto x : std::vector<double>{-1., 1.})
                    for (auto y : std::vector<double>{-1., 1.})
                        cross_hole(img, position + Point2d(x, y), _ContrastColor(color), size, size_hole, thickness);

            cross_hole(img, position, color, size, size_hole, thickness);
            double delta_y = size_hole + size + 6.;
            Point2d text_position = {position.x, position.y - delta_y};
            text(img, text_position, name, color, true, add_cartouche, font_scale);
        }

        ImageBuffer stack_images_vertically(const ImageBuffer &img1, const ImageBuffer &img2)
        {
            ImageBuffer img = ImageBuffer::Zeros(img1.width, img1.height + img2.height, img1.channels, img1.depth);
            ImageBuffer top = img.subImage(Rect(0, 0, img1.width, img1.height));
            img1.copyTo(top);
            ImageBuffer bottom = img.subImage(Rect(0, img1.height, img2.width, img2.height));
            img2.copyTo(bottom);
            return img;
        }

        ImageBuffer stack_images_horizontally(const ImageBuffer &img1, const ImageBuffer &img2)
        {
            ImageBuffer img = ImageBuffer::Zeros(img1.width + img2.width, img1.height, img1.channels, img1.depth);
            ImageBuffer left = img.subImage(Rect(0, 0, img1.width, img1.height));
            img1.copyTo(left);
            ImageBuffer right = img.subImage(Rect(img1.width, 0, img2.width, img2.height));
            img2.copyTo(right);
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

        // Convert any ImageBuffer to RGBA uint8, with per-pixel loops
        // Note: isBgrOrder only applies to uint8 images (OpenCV convention).
        // Float/integer images are always treated as RGB order.
        template<typename T>
        ImageBuffer converted_to_rgba_typed(const ImageBuffer& mat, int nbChannels, bool isBgrOrder)
        {
            // BGR swap only applies to uint8 images; non-uint8 depths are always RGB
            bool swapRB = isBgrOrder && std::is_same_v<T, uint8_t>;

            ImageBuffer rgba = ImageBuffer::Zeros(mat.width, mat.height, 4, ImageDepth::uint8);
            parallel_for(0, mat.height, [&](int y)
            {
                const T* src = mat.ptr<T>(y);
                uint8_t* dst = rgba.ptr<uint8_t>(y);
                for (int x = 0; x < mat.width; x++)
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
            });
            return rgba;
        }

        Image_RGBA converted_to_rgba_image(const ImageBuffer &inputMat, bool isBgrOrder)
        {
            ImageBuffer mat = inputMat;
            int nbChannels = mat.channels;

            switch (mat.depth)
            {
                case ImageDepth::uint8:   return converted_to_rgba_typed<uint8_t>(mat, nbChannels, isBgrOrder);
                case ImageDepth::int8:    return converted_to_rgba_typed<int8_t>(mat, nbChannels, isBgrOrder);
                case ImageDepth::uint16:  return converted_to_rgba_typed<uint16_t>(mat, nbChannels, isBgrOrder);
                case ImageDepth::int16:   return converted_to_rgba_typed<int16_t>(mat, nbChannels, isBgrOrder);
                case ImageDepth::int32:   return converted_to_rgba_typed<int32_t>(mat, nbChannels, isBgrOrder);
                case ImageDepth::float32: return converted_to_rgba_typed<float>(mat, nbChannels, isBgrOrder);
                case ImageDepth::float64: return converted_to_rgba_typed<double>(mat, nbChannels, isBgrOrder);
                default:     throw std::runtime_error("converted_to_rgba_image: unsupported depth");
            }
        }

        ImageBuffer make_alpha_channel_checkerboard_image(const Size& size, int squareSize)
        {
            ImageBuffer r = ImageBuffer::Zeros(size.width, size.height, 3, ImageDepth::uint8);
            for (int x = 0; x < size.width; x++)
            {
                for (int y = 0; y < size.height; y++)
                {
                    uint8_t colorValue = ((x / squareSize + y / squareSize) % 2 == 0) ? 102 : 152;
                    uint8_t* p = r.ptr<uint8_t>(y) + x * 3;
                    p[0] = colorValue;
                    p[1] = colorValue;
                    p[2] = colorValue;
                }
            }
            return r;
        }


        void draw_transparent_pixel(
            ImageBuffer &img_rgba,
            const Point2d &position,
            const Color4d &color,
            double alpha
        )
        {
            assert(img_rgba.depth == ImageDepth::uint8 && img_rgba.channels == 4);

            auto fnLerpColor4d = [](Color4d c1, Color4d c2, double k)
            {
                auto fnLerp = [](double x1, double x2, double k2) {
                    return x1 + k2 * (x2 - x1);
                };
                Color4d r(
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

            std::vector<std::pair<Point2d, double>> positionAndKs {
                { Point2d(0., 0.), kx0 * ky0 },
                { Point2d(1., 0.), kx1 * ky0 },
                { Point2d(0., 1.), kx0 * ky1 },
                { Point2d(1., 1.), kx1 * ky1 }
            };

            Rect roi(Point(0, 0), img_rgba.size());
            for (const auto& kv: positionAndKs)
            {
                Point pos;
                {
                    Point2d delta = kv.first;
                    pos = Point((int)(position.x + delta.x), (int)(position.y + delta.y));
                }
                double k = kv.second;

                if (!roi.contains(pos))
                    continue;

                uint8_t* pixel = img_rgba.ptr<uint8_t>(pos.y) + pos.x * 4;
                Color4d oldColor(pixel[0], pixel[1], pixel[2], pixel[3]);
                Color4d dstColor = fnLerpColor4d(oldColor, color, alpha * k);
                pixel[0] = (uint8_t)std::clamp(dstColor[0], 0.0, 255.0);
                pixel[1] = (uint8_t)std::clamp(dstColor[1], 0.0, 255.0);
                pixel[2] = (uint8_t)std::clamp(dstColor[2], 0.0, 255.0);
                pixel[3] = (uint8_t)std::clamp(dstColor[3], 0.0, 255.0);
            }
        }


        void draw_grid(
            ImageBuffer& img_rgba,
            Color4d lineColor,
            double alpha,
            double x_spacing, double y_spacing,
            double x_start, double y_start,
            double x_end, double y_end
            )
        {
            assert(img_rgba.depth == ImageDepth::uint8 && img_rgba.channels == 4);

            for (double y = y_start; y < y_end; y+= y_spacing)
                for (double x = 0.; x < x_end; x+= 1.)
                    draw_transparent_pixel(img_rgba, Point2d(x, y), lineColor, alpha);
            for (double x = x_start; x < x_end; x+= x_spacing)
                for (double y = 0.; y < y_end; y+= 1.)
                    draw_transparent_pixel(img_rgba, Point2d(x, y), lineColor, alpha);
        }

    }  // namespace DrawingUtils
}  // namespace ImmVision
