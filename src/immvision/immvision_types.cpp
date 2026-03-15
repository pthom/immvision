#include "immvision/immvision_types.h"
#include <cstring>   // memcpy
#include <stdexcept>
#include <cmath>     // fabs
#include <algorithm> // clamp

namespace ImmVision
{
    // =========================================================================
    // ImageDepth helpers
    // =========================================================================

    size_t ImageDepthSize(ImageDepth depth)
    {
        switch (depth)
        {
            case ImageDepth::uint8:   return 1;
            case ImageDepth::int8:    return 1;
            case ImageDepth::uint16:  return 2;
            case ImageDepth::int16:   return 2;
            case ImageDepth::int32:   return 4;
            case ImageDepth::float32: return 4;
            case ImageDepth::float64: return 8;
        }
        throw std::runtime_error("ImageDepthSize: unknown depth");
    }

    std::string ImageDepthName(ImageDepth depth)
    {
        switch (depth)
        {
            case ImageDepth::uint8:   return "uint8";
            case ImageDepth::int8:    return "int8";
            case ImageDepth::uint16:  return "uint16";
            case ImageDepth::int16:   return "int16";
            case ImageDepth::int32:   return "int32";
            case ImageDepth::float32: return "float32";
            case ImageDepth::float64: return "float64";
        }
        return "???";
    }

    bool ImageDepthIsFloat(ImageDepth depth)
    {
        return depth == ImageDepth::float32 || depth == ImageDepth::float64;
    }

#ifdef IMMVISION_HAS_OPENCV
    static ImageDepth cv_depth_to_image_depth(int cv_depth)
    {
        switch (cv_depth)
        {
            case CV_8U:  return ImageDepth::uint8;
            case CV_8S:  return ImageDepth::int8;
            case CV_16U: return ImageDepth::uint16;
            case CV_16S: return ImageDepth::int16;
            case CV_32S: return ImageDepth::int32;
            case CV_32F: return ImageDepth::float32;
            case CV_64F: return ImageDepth::float64;
            default:
                throw std::runtime_error("cv_depth_to_image_depth: unsupported cv depth");
        }
    }

    static int image_depth_to_cv_depth(ImageDepth depth)
    {
        switch (depth)
        {
            case ImageDepth::uint8:   return CV_8U;
            case ImageDepth::int8:    return CV_8S;
            case ImageDepth::uint16:  return CV_16U;
            case ImageDepth::int16:   return CV_16S;
            case ImageDepth::int32:   return CV_32S;
            case ImageDepth::float32: return CV_32F;
            case ImageDepth::float64: return CV_64F;
        }
        throw std::runtime_error("image_depth_to_cv_depth: unknown depth");
    }
#endif

    // =========================================================================
    // Matrix33d
    // =========================================================================

    Matrix33d::Matrix33d()
    {
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                m[r][c] = (r == c) ? 1.0 : 0.0;
    }

    Matrix33d Matrix33d::eye()
    {
        return Matrix33d();  // default ctor is identity
    }

    Matrix33d Matrix33d::inv() const
    {
        // 3x3 matrix inverse using Cramer's rule
        const auto& a = m;
        double det =
            a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) -
            a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0]) +
            a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);

        if (std::fabs(det) < 1e-15)
            throw std::runtime_error("Matrix33d::inv: singular matrix");

        double inv_det = 1.0 / det;
        Matrix33d r;
        r.m[0][0] =  (a[1][1] * a[2][2] - a[1][2] * a[2][1]) * inv_det;
        r.m[0][1] = -(a[0][1] * a[2][2] - a[0][2] * a[2][1]) * inv_det;
        r.m[0][2] =  (a[0][1] * a[1][2] - a[0][2] * a[1][1]) * inv_det;
        r.m[1][0] = -(a[1][0] * a[2][2] - a[1][2] * a[2][0]) * inv_det;
        r.m[1][1] =  (a[0][0] * a[2][2] - a[0][2] * a[2][0]) * inv_det;
        r.m[1][2] = -(a[0][0] * a[1][2] - a[0][2] * a[1][0]) * inv_det;
        r.m[2][0] =  (a[1][0] * a[2][1] - a[1][1] * a[2][0]) * inv_det;
        r.m[2][1] = -(a[0][0] * a[2][1] - a[0][1] * a[2][0]) * inv_det;
        r.m[2][2] =  (a[0][0] * a[1][1] - a[0][1] * a[1][0]) * inv_det;
        return r;
    }

    Matrix33d Matrix33d::operator*(const Matrix33d& rhs) const
    {
        Matrix33d r;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                r.m[i][j] = 0.0;
                for (int k = 0; k < 3; k++)
                    r.m[i][j] += m[i][k] * rhs.m[k][j];
            }
        return r;
    }

    bool Matrix33d::operator==(const Matrix33d& o) const
    {
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (m[r][c] != o.m[r][c])
                    return false;
        return true;
    }

#ifdef IMMVISION_HAS_OPENCV
    Matrix33d::Matrix33d(const cv::Matx33d& mat)
    {
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                m[r][c] = mat(r, c);
    }

    Matrix33d::operator cv::Matx33d() const
    {
        cv::Matx33d r;
        for (int r_ = 0; r_ < 3; r_++)
            for (int c = 0; c < 3; c++)
                r(r_, c) = m[r_][c];
        return r;
    }
#endif

    // =========================================================================
    // ImageBuffer
    // =========================================================================

    size_t ImageBuffer::elemSize() const
    {
        return ImageDepthSize(depth);
    }

    ImageBuffer ImageBuffer::Zeros(int w, int h, int ch, ImageDepth d)
    {
        ImageBuffer buf;
        buf.width = w;
        buf.height = h;
        buf.channels = ch;
        buf.depth = d;
        buf.step = w * ch * ImageDepthSize(d);
        auto owned = std::make_shared<std::vector<uint8_t>>(h * buf.step, 0);
        buf.data = owned->data();
        buf._ref_keeper = owned;  // shared_ptr<void> holds the vector
        return buf;
    }

    ImageBuffer ImageBuffer::clone() const
    {
        if (empty())
            return ImageBuffer();
        ImageBuffer buf = Zeros(width, height, channels, depth);
        // Copy row by row (source may have different stride)
        size_t row_bytes = width * channels * ImageDepthSize(depth);
        for (int y = 0; y < height; y++)
        {
            const uint8_t* src_row = static_cast<const uint8_t*>(data) + y * step;
            uint8_t* dst_row = static_cast<uint8_t*>(buf.data) + y * buf.step;
            std::memcpy(dst_row, src_row, row_bytes);
        }
        return buf;
    }

    void ImageBuffer::fill(const Color4d& color)
    {
        if (empty()) return;
        size_t es = elemSize();
        for (int y = 0; y < height; y++)
        {
            uint8_t* row = static_cast<uint8_t*>(data) + y * step;
            for (int x = 0; x < width; x++)
            {
                uint8_t* pixel = row + x * channels * es;
                for (int c = 0; c < channels; c++)
                {
                    double val = (c < 4) ? color[c] : 0.0;
                    switch (depth)
                    {
                        case ImageDepth::uint8:
                            pixel[c] = (uint8_t)std::clamp(val, 0.0, 255.0);
                            break;
                        case ImageDepth::int8:
                            reinterpret_cast<int8_t*>(pixel)[c] = (int8_t)std::clamp(val, -128.0, 127.0);
                            break;
                        case ImageDepth::uint16:
                            reinterpret_cast<uint16_t*>(pixel)[c] = (uint16_t)std::clamp(val, 0.0, 65535.0);
                            break;
                        case ImageDepth::int16:
                            reinterpret_cast<int16_t*>(pixel)[c] = (int16_t)std::clamp(val, -32768.0, 32767.0);
                            break;
                        case ImageDepth::int32:
                            reinterpret_cast<int32_t*>(pixel)[c] = (int32_t)val;
                            break;
                        case ImageDepth::float32:
                            reinterpret_cast<float*>(pixel)[c] = (float)val;
                            break;
                        case ImageDepth::float64:
                            reinterpret_cast<double*>(pixel)[c] = val;
                            break;
                    }
                }
            }
        }
    }

#ifdef IMMVISION_HAS_OPENCV
    ImageBuffer::ImageBuffer(const cv::Mat& mat)
    {
        // Ensure contiguous memory: clone if needed (e.g. ROI sub-matrices)
        cv::Mat continuous = mat.isContinuous() ? mat : mat.clone();
        data = continuous.data;
        width = continuous.cols;
        height = continuous.rows;
        channels = continuous.channels();
        depth = cv_depth_to_image_depth(continuous.depth());
        step = continuous.step[0];
        // Keep the cv::Mat header alive — its refcount keeps the pixel data alive
        auto mat_copy = std::make_shared<cv::Mat>(continuous);
        _ref_keeper = mat_copy;
    }

    cv::Mat ImageBuffer::to_cv_mat() const
    {
        if (empty())
            return cv::Mat();
        int cv_type = CV_MAKETYPE(image_depth_to_cv_depth(depth), channels);
        return cv::Mat(height, width, cv_type, data, step);
    }

    cv::Mat ImageBuffer::to_cv_mat_clone() const
    {
        return to_cv_mat().clone();
    }
#endif

} // namespace ImmVision
