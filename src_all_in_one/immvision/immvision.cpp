// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision_types.cpp                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision_types.h included by src/immvision/immvision_types.cpp          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <cstddef>   // size_t
#include <cstdint>   // uint8_t
#include <memory>    // shared_ptr
#include <vector>
#include <algorithm> // std::min, std::max
#include <cstring>   // memcpy
#include <string>

#ifdef IMMVISION_HAS_OPENCV
#include <opencv2/core.hpp>
#endif

// IMMVISION_API is a marker for public API functions.
#ifndef IMMVISION_API
#define IMMVISION_API
#endif


namespace ImmVision
{
    //
    // ImmVision types
    // ===============
    //
    // ImmVision uses its own lightweight types for images, points, sizes, and matrices.
    // These types do not depend on OpenCV.
    //
    // C++ users:
    //     If OpenCV is available (IMMVISION_HAS_OPENCV is defined), all types provide
    //     implicit conversions to/from their OpenCV equivalents:
    //         ImageBuffer <-> cv::Mat        (zero-copy via ImageBuffer(cv::Mat) and to_cv_mat())
    //         Point       <-> cv::Point      (implicit both ways)
    //         Point2d     <-> cv::Point2d    (implicit both ways)
    //         Size        <-> cv::Size       (implicit both ways)
    //         Matrix33d   <-> cv::Matx33d    (implicit both ways)
    //     This means you can pass cv::Mat, cv::Point, etc. directly to ImmVision functions.
    //
    // Python users:
    //     These types are mapped transparently to native Python types:
    //         ImageBuffer <-> numpy.ndarray
    //         Point       <-> Tuple[int, int]
    //         Point2d     <-> Tuple[float, float]
    //         Size        <-> Tuple[int, int]
    //         Matrix33d   <-> List[List[float]]  (3x3)
    //     You never need to create these types explicitly in Python.
    //

    enum class ImageDepth
    {
        uint8,
        int8,
        uint16,
        int16,
        int32,
        float32,
        float64
    };

    // Returns the size in bytes of a single element of the given depth
    IMMVISION_API size_t ImageDepthSize(ImageDepth depth);
    // Returns the name of the depth as a string ("uint8", "float32", etc.)
    IMMVISION_API std::string ImageDepthName(ImageDepth depth);
    // Returns true if the depth is a floating-point type (float32 or float64)
    IMMVISION_API bool ImageDepthIsFloat(ImageDepth depth);


    // 2D integer point. Converts implicitly to/from cv::Point when OpenCV is available.
    // Python: mapped to Tuple[int, int].
    struct Point
    {
        int x = 0, y = 0;
        Point() = default;
        Point(int x_, int y_) : x(x_), y(y_) {}
        bool operator==(const Point& o) const { return x == o.x && y == o.y; }
        bool operator!=(const Point& o) const { return !(*this == o); }
        #ifdef IMMVISION_HAS_OPENCV
        Point(const cv::Point& p) : x(p.x), y(p.y) {}
        operator cv::Point() const { return {x, y}; }
        #endif
    };


    // 2D double-precision point. Converts implicitly to/from cv::Point2d when OpenCV is available.
    // Python: mapped to Tuple[float, float].
    struct Point2d
    {
        double x = 0., y = 0.;
        Point2d() = default;
        Point2d(double x_, double y_) : x(x_), y(y_) {}
        // Conversion from integer Point
        Point2d(const Point& p) : x((double)p.x), y((double)p.y) {}
        bool operator==(const Point2d& o) const { return x == o.x && y == o.y; }
        bool operator!=(const Point2d& o) const { return !(*this == o); }
        Point2d operator+(const Point2d& o) const { return {x + o.x, y + o.y}; }
        Point2d operator-(const Point2d& o) const { return {x - o.x, y - o.y}; }
        #ifdef IMMVISION_HAS_OPENCV
        Point2d(const cv::Point2d& p) : x(p.x), y(p.y) {}
        operator cv::Point2d() const { return {x, y}; }
        #endif
    };


    // 2D integer size. Converts implicitly to/from cv::Size when OpenCV is available.
    // Python: mapped to Tuple[int, int].
    struct Size
    {
        int width = 0, height = 0;
        Size() = default;
        Size(int w, int h) : width(w), height(h) {}
        bool empty() const { return width == 0 && height == 0; }
        int area() const { return width * height; }
        bool operator==(const Size& o) const { return width == o.width && height == o.height; }
        bool operator!=(const Size& o) const { return !(*this == o); }
        #ifdef IMMVISION_HAS_OPENCV
        Size(const cv::Size& s) : width(s.width), height(s.height) {}
        operator cv::Size() const { return {width, height}; }
        #endif
    };


    // 2D double-precision size. Used for drawing operations (ellipse, rectangle_size).
    struct Size2d
    {
        double width = 0., height = 0.;
        Size2d() = default;
        Size2d(double w, double h) : width(w), height(h) {}
        Size2d(const Size& s) : width((double)s.width), height((double)s.height) {}
    };


    // 3x3 double-precision matrix, used for zoom/pan affine transforms.
    // Converts implicitly to/from cv::Matx33d when OpenCV is available.
    // Python: mapped to List[List[float]] (3x3), also accepts numpy 3x3 arrays.
    struct Matrix33d
    {
        double m[3][3];

        // Default constructor: identity matrix
        IMMVISION_API Matrix33d();
        IMMVISION_API static Matrix33d eye();
        IMMVISION_API Matrix33d inv() const;
        IMMVISION_API Matrix33d operator*(const Matrix33d& rhs) const;

        double& operator()(int r, int c) { return m[r][c]; }
        const double& operator()(int r, int c) const { return m[r][c]; }

        bool operator==(const Matrix33d& o) const;
        bool operator!=(const Matrix33d& o) const { return !(*this == o); }

        // Apply this 3x3 matrix to a 2D point (homogeneous multiplication)
        Point2d apply(const Point2d& p) const
        {
            return Point2d(
                m[0][0] * p.x + m[0][1] * p.y + m[0][2],
                m[1][0] * p.x + m[1][1] * p.y + m[1][2]);
        }

        #ifdef IMMVISION_HAS_OPENCV
        IMMVISION_API Matrix33d(const cv::Matx33d& mat);
        IMMVISION_API operator cv::Matx33d() const;
        #endif
    };


    // 4-channel double color value (e.g. RGBA or BGRA).
    struct Color4d
    {
        double v[4] = {0, 0, 0, 255};
        double& operator[](int i) { return v[i]; }
        const double& operator[](int i) const { return v[i]; }
        Color4d() = default;
        Color4d(double v0, double v1, double v2, double v3) : v{v0, v1, v2, v3} {}
        #ifdef IMMVISION_HAS_OPENCV
        Color4d(const cv::Scalar& s) : v{s[0], s[1], s[2], s[3]} {}
        operator cv::Scalar() const { return {v[0], v[1], v[2], v[3]}; }
        #endif
    };


    // Integer rectangle (x, y, width, height).
    struct Rect
    {
        int x = 0, y = 0, width = 0, height = 0;
        Rect() = default;
        Rect(int x_, int y_, int w, int h) : x(x_), y(y_), width(w), height(h) {}
        Rect(Point pt, Size sz) : x(pt.x), y(pt.y), width(sz.width), height(sz.height) {}
        // Construct from two corner points (top-left and bottom-right)
        Rect(Point pt1, Point pt2)
            : x(std::min(pt1.x, pt2.x)), y(std::min(pt1.y, pt2.y)),
              width(std::max(pt1.x, pt2.x) - x), height(std::max(pt1.y, pt2.y) - y) {}
        bool empty() const { return width <= 0 || height <= 0; }
        int area() const { return width * height; }
        bool contains(Point pt) const { return pt.x >= x && pt.x < x + width && pt.y >= y && pt.y < y + height; }
        Size size() const { return {width, height}; }
        #ifdef IMMVISION_HAS_OPENCV
        Rect(const cv::Rect& r) : x(r.x), y(r.y), width(r.width), height(r.height) {}
        operator cv::Rect() const { return {x, y, width, height}; }
        #endif
    };


    // Lightweight image container. Holds a pointer to pixel data with metadata (width, height,
    // channels, depth, stride). Does not depend on OpenCV.
    //
    // C++ users:
    //     If OpenCV is available, you can pass cv::Mat directly to any ImmVision function
    //     that accepts an ImageBuffer — the implicit constructor ImageBuffer(const cv::Mat&)
    //     wraps the data with zero copy. Use to_cv_mat() to get a zero-copy cv::Mat view back,
    //     or to_cv_mat_clone() for a deep copy that outlives the ImageBuffer.
    //
    // Python users:
    //     ImageBuffer is mapped transparently to numpy.ndarray. You simply pass numpy arrays
    //     to ImmVision functions and receive numpy arrays back. No manual conversion is needed.
    struct ImageBuffer
    {
        void* data = nullptr;
        int width = 0, height = 0, channels = 0;
        ImageDepth depth = ImageDepth::uint8;
        size_t step = 0;  // bytes per row (stride)

        // Type-erased ownership: keeps the underlying memory alive.
        // Can hold a cv::Mat (refcount), a Python object (ndarray), or owned memory.
        std::shared_ptr<void> _ref_keeper;

        ImageBuffer() = default;

        // Basic queries
        bool empty() const { return data == nullptr || width == 0 || height == 0; }
        // Bytes per single-channel element
        IMMVISION_API size_t elemSize() const;
        // Bytes per pixel (all channels)
        size_t elemSizeTotal() const { return elemSize() * channels; }

        // Convenience accessors (aliases for width/height)
        int rows() const { return height; }
        int cols() const { return width; }
        Size size() const { return {width, height}; }

        // Typed row pointer
        template<typename T> T* ptr(int y)
        {
            return reinterpret_cast<T*>(static_cast<uint8_t*>(data) + y * step);
        }
        template<typename T> const T* ptr(int y) const
        {
            return reinterpret_cast<const T*>(static_cast<const uint8_t*>(data) + y * step);
        }

        // Pointer to the first channel of pixel (x, y)
        template<typename T> T* pixel_ptr(int y, int x)
        {
            return ptr<T>(y) + x * channels;
        }
        template<typename T> const T* pixel_ptr(int y, int x) const
        {
            return ptr<T>(y) + x * channels;
        }

        // Sub-image view (non-owning, shares ref_keeper)
        ImageBuffer subImage(const Rect& roi) const
        {
            ImageBuffer sub;
            sub.width = roi.width;
            sub.height = roi.height;
            sub.channels = channels;
            sub.depth = depth;
            sub.step = step;
            sub.data = static_cast<uint8_t*>(const_cast<void*>(data)) + roi.y * step + roi.x * elemSizeTotal();
            sub._ref_keeper = _ref_keeper;
            return sub;
        }

        // Copy pixel data from this image to dst (must be same size and type)
        void copyTo(ImageBuffer& dst) const
        {
            size_t row_bytes = (size_t)width * channels * ImageDepthSize(depth);
            for (int y = 0; y < height; y++)
            {
                const uint8_t* src_row = static_cast<const uint8_t*>(data) + y * step;
                uint8_t* dst_row = static_cast<uint8_t*>(dst.data) + y * dst.step;
                std::memcpy(dst_row, src_row, row_bytes);
            }
        }

        // Fill all pixels with a uniform color (for uint8 images)
        IMMVISION_API void fill(const Color4d& color);

        // Owning allocation
        IMMVISION_API static ImageBuffer Zeros(int w, int h, int ch, ImageDepth d);
        IMMVISION_API ImageBuffer clone() const;

        #ifdef IMMVISION_HAS_OPENCV
        // Zero-copy wrap: keeps cv::Mat refcount alive via _ref_keeper
        IMMVISION_API ImageBuffer(const cv::Mat& mat);
        // Zero-copy view: valid while this ImageBuffer lives
        IMMVISION_API cv::Mat to_cv_mat() const;
        // Deep copy: safe even after ImageBuffer is destroyed
        IMMVISION_API cv::Mat to_cv_mat_clone() const;
        #endif
    };

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision_types.cpp continued                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.cpp                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IMGUI_DEFINE_MATH_OPERATORS

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.h included by src/immvision/internal/cv/colormap.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "imgui.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.h included by src/immvision/internal/cv/colormap.h                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// IMMVISION_API is a marker for public API functions. IMMVISION_STRUCT_API is a marker for public API structs (in comment lines)
// Usage of ImmVision as a shared library is not recommended. No guaranty of ABI stability is provided
#ifndef IMMVISION_API
#define IMMVISION_API
#endif


namespace ImmVision
{
    // Set the color order for displayed images.
    // You **must** call once at the start of your program:
    //     ImmVision::UseRgbColorOrder() or ImmVision::UseBgrColorOrder() (C++)
    //     immvision.use_rgb_color_order() or immvision.use_bgr_color_order() (Python)
    // (Breaking change - October 2024)
    void UseRgbColorOrder();
    void UseBgrColorOrder();

    // Returns true if we are using RGB color order
    bool IsUsingRgbColorOrder();
    // Returns true if we are using BGR color order
    bool IsUsingBgrColorOrder();
    // Returns true if the color order is undefined (i.e. UseRgbColorOrder or UseBgrColorOrder was not called)
    bool IsColorOrderUndefined();

    // Temporary change of color order (useful for displaying a single image with a different color order)
    void PushColorOrderRgb();
    void PushColorOrderBgr();
    void PopColorOrder();

    // Are we using the stats on the full image, on the Visible ROI, or are we using Min/Max values
    enum class ColorMapStatsTypeId
    {
        FromFullImage,
        FromVisibleROI
    };

    // Scale the Colormap according to the Image  stats
    struct ColormapScaleFromStatsData                                                            // IMMVISION_API_STRUCT
    {
        // Are we using the stats on the full image, the visible ROI, or are we using Min/Max values
        ColorMapStatsTypeId ColorMapStatsType = ColorMapStatsTypeId::FromFullImage;

        // If stats active (either on ROI or on Image), how many sigmas around the mean should the Colormap be applied
        double NbSigmas = 1.5;

        // If ColorMapScaleType==ColorMapStatsType::FromMinMax, then ColormapScaleMin will be calculated from the matrix min value instead of a sigma based value
        bool UseStatsMin = false;
        // If ColorMapScaleType==ColorMapStatsType::FromMinMax, then ColormapScaleMax will be calculated from the matrix min value instead of a sigma based value
        bool UseStatsMax = false;
    };


    // Colormap Settings (useful for matrices with one channel, in order to see colors mapping float values)
    struct ColormapSettingsData                                                                 // IMMVISION_API_STRUCT
    {
        // Colormap, see available Colormaps with AvailableColormaps()
        // Work only with 1 channel matrices, i.e len(shape)==2
        std::string Colormap = "None";

        // ColormapScaleMin and ColormapScaleMax indicate how the Colormap is applied:
        //     - Values in [ColormapScaleMin, ColomapScaleMax] will use the full colormap.
        //     - Values outside this interval will be clamped before coloring
        // by default, the initial values are ignored, and they will be updated automatically
        // via the options in ColormapScaleFromStats
        double ColormapScaleMin = 0.;
        double ColormapScaleMax = 1.;

        // If ColormapScaleFromStats.ActiveOnFullImage or ColormapScaleFromStats.ActiveOnROI,
        // then ColormapScaleMin/Max are ignored, and the scaling is done according to the image stats.
        // ColormapScaleFromStats.ActiveOnFullImage is true by default
        ColormapScaleFromStatsData ColormapScaleFromStats = ColormapScaleFromStatsData();

        // Internal value: stores the name of the Colormap that is hovered by the mouse
        std::string internal_ColormapHovered = "";
    };


    // Contains information about the mouse inside an image
    struct MouseInformation                                                                     // IMMVISION_API_STRUCT
    {
        // Is the mouse hovering the image
        bool IsMouseHovering = false;

        // Mouse position in the original image/matrix
        // This position is given with float coordinates, and will be (-1., -1.) if the mouse is not hovering the image
        Point2d MousePosition = Point2d(-1., -1.);
        // Mouse position in the displayed portion of the image (the original image can be zoomed,
        // and only show a subset if it may be shown).
        // This position is given with integer coordinates, and will be (-1, -1) if the mouse is not hovering the image
        Point MousePosition_Displayed = Point(-1, -1);

        //
        // Note: you can query ImGui::IsMouseDown(mouse_button) (c++) or imgui.is_mouse_down(mouse_button) (Python)
        //
    };


    // Set of display parameters and options for an Image
    struct ImageParams                                                                           // IMMVISION_API_STRUCT
    {
        //
        // ImageParams store the parameters for a displayed image
        // (as well as user selected watched pixels, selected channel, etc.)
        // Its default constructor will give them reasonable choices, which you can adapt to your needs.
        // Its values will be updated when the user pans or zooms the image, adds watched pixels, etc.
        //

        //
        // Refresh Images Textures
        //

        // Refresh Image: images textures are cached. Set to true if your image matrix/buffer has changed
        // (for example, for live video images)
        bool RefreshImage = false;

        //
        // Display size and title
        //

        // Size of the displayed image (can be different from the matrix size)
        // If you specify only the width or height (e.g (300, 0), then the other dimension
        // will be calculated automatically, respecting the original image w/h ratio.
        Size ImageDisplaySize = Size();

        //
        // Zoom and Pan (represented by an affine transform matrix, of size 3x3)
        //

        // ZoomPanMatrix can be created using MakeZoomPanMatrix to create a view centered around a given point
        Matrix33d ZoomPanMatrix = Matrix33d::eye();
        // If displaying several images, those with the same ZoomKey will zoom and pan together
        std::string ZoomKey = "";

        //
        // Colormap Settings (useful for matrices with one channel, in order to see colors mapping float values)
        //
        // ColormapSettings stores all the parameter concerning the Colormap
        ColormapSettingsData ColormapSettings = ColormapSettingsData();
        // If displaying several images, those with the same ColormapKey will adjust together
        std::string ColormapKey = "";

        //
        // Zoom and pan with the mouse
        //
        bool PanWithMouse = true;
        bool ZoomWithMouseWheel = true;

        // Can the image widget be resized by the user
        bool CanResize = true;
        // Does the widget keep an aspect ratio equal to the image when resized
        bool ResizeKeepAspectRatio = true;

        //
        // Image display options
        //
        // if SelectedChannel >= 0 then only this channel is displayed
        int  SelectedChannel = -1;
        // Show a "school paper" background grid
        bool ShowSchoolPaperBackground = true;
        // show a checkerboard behind transparent portions of 4 channels RGBA images
        bool ShowAlphaChannelCheckerboard = true;
        // Grid displayed when the zoom is high
        bool ShowGrid = true;
        // Pixel values show when the zoom is high
        bool DrawValuesOnZoomedPixels = true;

        //
        // Image display options
        //
        // Show matrix type and size
        bool ShowImageInfo = true;
        // Show pixel values
        bool ShowPixelInfo = true;
        // Show buttons that enable to zoom in/out (the mouse wheel also zoom)
        bool ShowZoomButtons = true;
        // Open the options panel
        bool ShowOptionsPanel = false;
        // If set to true, then the option panel will be displayed in a transient tooltip window
        bool ShowOptionsInTooltip = false;
        // If set to false, then the Options button will not be displayed
        bool ShowOptionsButton = true;

        //
        // Watched Pixels
        //
        // List of Watched Pixel coordinates
        std::vector<Point> WatchedPixels = std::vector<Point>();
        // Shall we add a watched pixel on double click
        bool AddWatchedPixelOnDoubleClick = true;
        // Shall the watched pixels be drawn on the image
        bool HighlightWatchedPixels = true;

        // Mouse position information. These values are filled after displaying an image
        MouseInformation MouseInfo = MouseInformation();

        ~ImageParams();
    };

#ifdef IMMVISION_SERIALIZE_JSON
    IMMVISION_API std::string ImageParamsToJson(const ImageParams& params);
    IMMVISION_API void FillImageParamsFromJson(const std::string& json, ImageParams* params);
    IMMVISION_API ImageParams ImageParamsFromJson(const std::string& json);
#endif

    // Create ImageParams that display the image only, with no decoration, and no user interaction
    IMMVISION_API ImageParams FactorImageParamsDisplayOnly();


    // Create a zoom/pan matrix centered around a given point of interest
    IMMVISION_API Matrix33d MakeZoomPanMatrix(
                        const Point2d & zoomCenter,
                        double zoomRatio,
                        const Size displayedImageSize
    );

    IMMVISION_API Matrix33d MakeZoomPanMatrix_ScaleOne(
        Size imageSize,
        const Size displayedImageSize
    );

    IMMVISION_API Matrix33d MakeZoomPanMatrix_FullView(
        Size imageSize,
        const Size displayedImageSize
    );


    // Display an image, with full user control: zoom, pan, watch pixels, etc.
    //
    // :param label
    //     A legend that will be displayed.
    //     Important notes:
    //         - With ImGui and ImmVision, widgets *must* have a unique Ids.
    //           For this widget, the id is given by this label.
    //           Two widgets (for example) two images *cannot* have the same label or the same id!
    //           (you can use ImGui::PushID / ImGui::PopID to circumvent this, or add suffixes with ##)
    //
    //           If they do, they might not refresh correctly!
    //           To circumvent this, you can:
    //              - Call `ImGui::PushID("some_unique_string")` at the start of your function,
    //                and `ImGui::PopID()` at the end.
    //              - Or modify your label like this:
    //                  "MyLabel##some_unique_id"
    //                  (the part after "##" will not be displayed but will be part of the id)
    //        - To display an empty legend, use "##_some_unique_id"
    //
    // :param image
    //     The image to display. All dense image types are supported (uint8, int16, float32, etc.).
    //     C++: accepts ImageBuffer directly, or cv::Mat (implicit conversion, zero-copy).
    //     Python: pass a numpy.ndarray.
    //
    // :param params
    //     Complete options (as modifiable inputs), and outputs (mouse position, watched pixels, etc)
    //     @see ImageParams structure.
    //     The ImageParams may be modified by this function: you can extract from them
    //     the mouse position, watched pixels, etc.
    //     Important note:
    //         ImageParams is an input-output parameter, passed as a pointer.
    //         Its scope should be wide enough so that it is preserved from frame to frame.
    //         !! If you cannot zoom/pan in a displayed image, extend the scope of the ImageParams !!
    //
    // - This function requires that both imgui and OpenGL were initialized.
    //   (for example, use `imgui_runner.run`for Python,  or `HelloImGui::Run` for C++)
    IMMVISION_API void Image(const std::string& label, const ImageBuffer& image, ImageParams* params);


    // ImageDisplay: Only, display the image, with no user interaction (by default)
    //
    // Parameters:
    // :param label_id
    //     A legend that will be displayed.
    //     Important notes:
    //         - With ImGui and ImmVision, widgets must have a unique Ids. For this widget, the id is given by this label.
    //           Two widgets (for example) two images *cannot* have the same label or the same id!
    //           If they do, they might not refresh correctly!
    //           To circumvent this, you can modify your label like this:
    //              "MyLabel##some_unique_id"    (the part after "##" will not be displayed but will be part of the id)
    //        - To display an empty legend, use "##_some_unique_id"
    //        - if your legend is displayed (i.e. it does not start with "##"),
    //          then the total size of the widget will be larger than the imageDisplaySize.
    //
    // :param image:
    //     The image to display. All dense image types are supported.
    //     C++: accepts ImageBuffer directly, or cv::Mat (implicit conversion, zero-copy).
    //     Python: pass a numpy.ndarray.
    //
    // :param imageDisplaySize:
    //     Size of the displayed image (can be different from the image size)
    //     If you specify only the width or height (e.g (300, 0), then the other dimension
    //     will be calculated automatically, respecting the original image w/h ratio.
    //
    // :param refreshImage:
    //     images textures are cached. Set to true if your image has changed
    //     (for example, for live video images)
    //
    // :param showOptionsButton:
    //     If true, show an option button that opens the option panel.
    //     In that case, it also becomes possible to zoom & pan, add watched pixel by double-clicking, etc.
    //
    // :return:
    //      The mouse position in the original image coordinates, as double values.
    //      (i.e. it does not matter if imageDisplaySize is different from the image size)
    //      It will return (-1., -1.) if the mouse is not hovering the image.
    //
    //      Note: use ImGui::IsMouseDown(mouse_button) (C++) or imgui.is_mouse_down(mouse_button) (Python)
    //            to query more information about the mouse.
    //
    // Note: this function requires that both imgui and OpenGL were initialized.
    //       (for example, use `imgui_runner.run`for Python,  or `HelloImGui::Run` for C++)
    //
    IMMVISION_API Point2d ImageDisplay(
        const std::string& label_id,
        const ImageBuffer& image,
        const Size& imageDisplaySize = Size(),
        bool refreshImage = false,
        bool showOptionsButton = false
        );

    // ImageDisplayResizable: display the image, with no user interaction (by default)
    // The image can be resized by the user (and the new size will be stored in the size parameter, if provided)
    // The label will not be displayed (but it will be used as an id, and must be unique)
    //
    // :param image:
    //     The image to display.
    //     C++: accepts ImageBuffer directly, or cv::Mat (implicit conversion, zero-copy).
    //     Python: pass a numpy.ndarray.
    IMMVISION_API Point2d ImageDisplayResizable(
        const std::string& label_id,
        const ImageBuffer& image,
        ImVec2* size = nullptr,
        bool refreshImage = false,
        bool resizable = true,
        bool showOptionsButton = false
    );


    // Return the list of the available color maps
    // Taken from https://github.com/yuki-koyama/tinycolormap, thanks to Yuki Koyama
    IMMVISION_API std::vector<std::string> AvailableColormaps();


    // Clears the internal texture cache of immvision (this is done automatically at exit time)
    //
    // Note: this function requires that both imgui and OpenGL were initialized.
    //       (for example, use `imgui_runner.run`for Python,  or `HelloImGui::Run` for C++)
    IMMVISION_API void ClearTextureCache();

    // Returns the RGBA image currently displayed by ImmVision::Image or ImmVision::ImageDisplay
    // Note: this image must be currently displayed. This function will return the transformed image
    // (i.e with ColorMap, Zoom, etc.)
    IMMVISION_API ImageBuffer GetCachedRgbaImage(const std::string& label);

    // Return immvision version info
    IMMVISION_API std::string VersionInfo();

    // Load an image from file (PNG, JPG, BMP, TGA, HDR, etc.) using stb_image.
    // Returns an empty ImageBuffer if loading fails.
    // The returned image is always in RGB order (not BGR).
    // For uint8 images: channels are as stored in file (1, 3, or 4).
    // For HDR images: returns float32 data.
    IMMVISION_API ImageBuffer ImRead(const std::string& filename);

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.h continued                                         //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/insertion_order_map.h included by src/immvision/internal/cv/colormap.h//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <unordered_map>
#include <utility>
#include <algorithm>


namespace ImmVision
{
    template<typename Key, typename Value>
    class insertion_order_map
    {
    public:

        bool contains(const Key& k) const { return _map.find(k) != _map.end(); }

        bool empty() const { return _keys.empty(); }

        void clear() { _keys.clear(); _map.clear(); }

        void insert(const Key& k, const Value& v)
        {
            assert(!contains(k));
            _keys.push_back(k);
            _map[k] = v;
        }

        void insert(const Key& k, Value&& v)
        {
            assert(!contains(k));
            _keys.push_back(k);
            _map[k] = std::move(v);
        }


        Value& get(const Key& k)
        {
            assert(contains(k));
            return _map.at(k);
        }

        const Value& get(const Key& k) const
        {
            assert(contains(k));
            return _map.at(k);
        }

        void erase(const Key& k)
        {
            assert(contains(k));
            _map.erase(_map.find(k));
            _keys.erase(std::remove(_keys.begin(), _keys.end(), 5), _keys.end());
        }

        const std::vector<Key>& insertion_order_keys() const
        {
            return _keys;
        }

        const std::vector<std::pair<const Key&, const Value&>> items() const
        {
            std::vector<std::pair<const Key&, const Value&>> r;
            for (const auto& k : insertion_order_keys())
                r.push_back({k, get(k)});
            return r;
        }

        std::vector<std::pair<const Key&, Value&>> items()
        {
            std::vector<std::pair<const Key&, Value&>> r;
            for (const auto& k : insertion_order_keys())
                r.push_back({k, get(k)});
            return r;
        }

    private:
        std::unordered_map<Key, Value> _map;
        std::vector<Key> _keys;
    };



} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.h continued                                         //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <map>

// Uses https://github.com/jgreitemann/colormap
// Many thanks to Yuki Koyama

// Uses https://github.com/Neargye/magic_enum
// Many thanks to Daniil Goncharov


namespace ImmVision
{
    namespace Colormap
    {
        //
        // Base operations for ColormapScaleSettings
        //
        bool IsNone(const ColormapSettingsData& a);
        bool IsEqual(const ColormapSettingsData& v1, const ColormapSettingsData& v2);
        bool IsEqual(const ColormapScaleFromStatsData& v1, const ColormapScaleFromStatsData& v2);
        bool CanColormap(const ImageBuffer &image);
        ColormapSettingsData ComputeInitialColormapSettings(const ImageBuffer& m);


        //
        // Colormaps images and textures
        //
        std::vector<std::string> AvailableColormaps();

        const insertion_order_map<std::string, ImTextureID>& ColormapsTextures();
        void ClearColormapsTexturesCache();

        //
        // Apply Colormap
        //
        ImageBuffer ApplyColormap(const ImageBuffer& m, const ColormapSettingsData& settings);


        //
        // Interactive update during pan and zoom, full init on new Image
        //
        void UpdateRoiStatsInteractively(
            const ImageBuffer &image,
            const Rect& roi,
            ColormapSettingsData* inout_settings);
        void InitStatsOnNewImage(
            const ImageBuffer &image,
            const Rect& roi,
            ColormapSettingsData* inout_settings);

        //
        // GUI
        //
        void GuiShowColormapSettingsData(
            const ImageBuffer &image,
            const Rect& roi,
            float availableGuiWidth,
            ColormapSettingsData* inout_settings);

    } // namespace Colormap

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/tinycolormap.hpp included by src/immvision/internal/cv/colormap.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 MIT License

 Copyright (c) 2018-2020 Yuki Koyama

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 -------------------------------------------------------------------------------

 The lookup table for Turbo is derived by Shot511 in his PR,
 https://github.com/yuki-koyama/tinycolormap/pull/27 , from
 https://gist.github.com/mikhailov-work/6a308c20e494d9e0ccc29036b28faa7a , which
 is released by Anton Mikhailov, copyrighted by Google LLC, and licensed under
 the Apache 2.0 license. To the best of our knowledge, the Apache 2.0 license is
 compatible with the MIT license, and thus we release the merged entire code
 under the MIT license. The license notice for Anton's code is posted here:

 Copyright 2019 Google LLC.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
     http://www.apache.org/licenses/LICENSE-2.0
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 */

#ifndef TINYCOLORMAP_HPP_
#define TINYCOLORMAP_HPP_

#include <cmath>
#include <cstdint>

#if defined(TINYCOLORMAP_WITH_EIGEN)
#include <Eigen/Core>
#endif

#if defined(TINYCOLORMAP_WITH_QT5)
#include <QColor>
#endif

#if defined(TINYCOLORMAP_WITH_QT5) && defined(TINYCOLORMAP_WITH_EIGEN)
#include <QImage>
#include <QString>
#endif

#if defined(TINYCOLORMAP_WITH_GLM)
#include <glm/vec3.hpp>
#endif

namespace tinycolormap
{
    //////////////////////////////////////////////////////////////////////////////////
    // Interface
    //////////////////////////////////////////////////////////////////////////////////

    enum class ColormapType
    {
        None, Gray, Parula, Heat, Jet, Turbo, Hot, Magma, Inferno, Plasma, Viridis, Cividis, Github, Cubehelix
    };

    struct Color
    {
        explicit constexpr Color(double gray) noexcept : data{ gray, gray, gray } {}
        constexpr Color(double r, double g, double b) noexcept : data{ r, g, b } {}

        double data[3];

        double& r() noexcept { return data[0]; }
        double& g() noexcept { return data[1]; }
        double& b() noexcept { return data[2]; }
        constexpr double r() const noexcept { return data[0]; }
        constexpr double g() const noexcept { return data[1]; }
        constexpr double b() const noexcept { return data[2]; }

        constexpr uint8_t ri() const noexcept { return static_cast<uint8_t>(data[0] * 255.0); }
        constexpr uint8_t gi() const noexcept { return static_cast<uint8_t>(data[1] * 255.0); }
        constexpr uint8_t bi() const noexcept { return static_cast<uint8_t>(data[2] * 255.0); }

        double& operator[](std::size_t n) noexcept { return data[n]; }
        constexpr double operator[](std::size_t n) const noexcept { return data[n]; }
        double& operator()(std::size_t n) noexcept { return data[n]; }
        constexpr double operator()(std::size_t n) const noexcept { return data[n]; }

        friend constexpr Color operator+(const Color& c0, const Color& c1) noexcept
        {
            return { c0.r() + c1.r(), c0.g() + c1.g(), c0.b() + c1.b() };
        }

        friend constexpr Color operator*(double s, const Color& c) noexcept
        {
            return { s * c.r(), s * c.g(), s * c.b() };
        }

#if defined(TINYCOLORMAP_WITH_QT5)
        QColor ConvertToQColor() const { return QColor(data[0] * 255.0, data[1] * 255.0, data[2] * 255.0); }
#endif
#if defined(TINYCOLORMAP_WITH_EIGEN)
        Eigen::Vector3d ConvertToEigen() const { return Eigen::Vector3d(data[0], data[1], data[2]); }
#endif
#if defined(TINYCOLORMAP_WITH_GLM)
        glm::vec3 ConvertToGLM() const { return glm::vec3(data[0], data[1], data[2]); }
#endif
    };

    inline Color GetColor(double x, ColormapType type = ColormapType::Viridis);
    inline Color GetQuantizedColor(double x, unsigned int num_levels, ColormapType type = ColormapType::Viridis);
    inline Color constexpr GetNoneColor(double x) noexcept;
    inline Color GetParulaColor(double x);
    inline Color GetHeatColor(double x);
    inline Color GetJetColor(double x);
    inline Color GetTurboColor(double x);
    inline Color GetHotColor(double x);
    inline constexpr Color GetGrayColor(double x) noexcept;
    inline Color GetMagmaColor(double x);
    inline Color GetInfernoColor(double x);
    inline Color GetPlasmaColor(double x);
    inline Color GetViridisColor(double x);
    inline Color GetCividisColor(double x);
    inline Color GetGithubColor(double x);
    inline Color GetCubehelixColor(double x);

#if defined(TINYCOLORMAP_WITH_QT5) && defined(TINYCOLORMAP_WITH_EIGEN)
    inline QImage CreateMatrixVisualization(const Eigen::MatrixXd& matrix);
    inline void ExportMatrixVisualization(const Eigen::MatrixXd& matrix, const std::string& path);
#endif

    //////////////////////////////////////////////////////////////////////////////////
    // Private Implementation - public usage is not intended
    //////////////////////////////////////////////////////////////////////////////////

    namespace internal
    {
        inline constexpr double Clamp01(double x) noexcept
        {
            return (x < 0.0) ? 0.0 : (x > 1.0) ? 1.0 : x;
        }

        // A helper function to calculate linear interpolation
        template <std::size_t N>
        Color CalcLerp(double x, const Color (&data)[N])
        {
            const double a  = Clamp01(x) * (N - 1);
            const double i  = std::floor(a);
            const double t  = a - i;
            const Color& c0 = data[static_cast<std::size_t>(i)];
            const Color& c1 = data[static_cast<std::size_t>(std::ceil(a))];

            return (1.0 - t) * c0 + t * c1;
        }

        inline double QuantizeArgument(double x, unsigned int num_levels)
        {
            // Clamp num_classes to range [1, 255].
            num_levels = (std::max)(1u, (std::min)(num_levels, 255u));

            const double interval_length = 255.0 / num_levels;

            // Calculate index of the interval to which the given x belongs to.
            // Substracting eps prevents getting out of bounds index.
            constexpr double eps = 0.0005;
            const unsigned int index = static_cast<unsigned int>((x * 255.0 - eps) / interval_length);

            // Calculate upper and lower bounds of the given interval.
            const unsigned int upper_boundary = static_cast<unsigned int>(index * interval_length + interval_length);
            const unsigned int lower_boundary = static_cast<unsigned int>(upper_boundary - interval_length);

            // Get middle "coordinate" of the given interval and move it back to [0.0, 1.0] interval.
            const double xx = static_cast<double>(upper_boundary + lower_boundary) * 0.5 / 255.0;

            return xx;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Public Implementation
    //////////////////////////////////////////////////////////////////////////////////

    inline Color GetColor(double x, ColormapType type)
    {
        switch (type)
        {
            case ColormapType::None:
                return GetNoneColor(x);
            case ColormapType::Parula:
                return GetParulaColor(x);
            case ColormapType::Heat:
                return GetHeatColor(x);
            case ColormapType::Jet:
                return GetJetColor(x);
            case ColormapType::Turbo:
                return GetTurboColor(x);
            case ColormapType::Hot:
                return GetHotColor(x);
            case ColormapType::Gray:
                return GetGrayColor(x);
            case ColormapType::Magma:
                return GetMagmaColor(x);
            case ColormapType::Inferno:
                return GetInfernoColor(x);
            case ColormapType::Plasma:
                return GetPlasmaColor(x);
            case ColormapType::Viridis:
                return GetViridisColor(x);
            case ColormapType::Cividis:
                return GetCividisColor(x);
            case ColormapType::Github:
                return GetGithubColor(x);
            case ColormapType::Cubehelix:
                return GetCubehelixColor(x);
            default:
                break;
        }

        return GetViridisColor(x);
    }

    inline Color GetQuantizedColor(double x, unsigned int num_levels, ColormapType type)
    {
        return GetColor(internal::QuantizeArgument(x, num_levels), type);
    }

    inline Color constexpr GetNoneColor(double x) noexcept
    {
        return Color{ internal::Clamp01(x) };
    }

    inline Color GetParulaColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.2081, 0.1663, 0.5292 },
                { 0.2091, 0.1721, 0.5411 },
                { 0.2101, 0.1779, 0.553   },
                { 0.2109, 0.1837, 0.565   },
                { 0.2116, 0.1895, 0.5771 },
                { 0.2121, 0.1954, 0.5892 },
                { 0.2124, 0.2013, 0.6013 },
                { 0.2125, 0.2072, 0.6135 },
                { 0.2123, 0.2132, 0.6258 },
                { 0.2118, 0.2192, 0.6381 },
                { 0.2111, 0.2253, 0.6505 },
                { 0.2099, 0.2315, 0.6629 },
                { 0.2084, 0.2377, 0.6753 },
                { 0.2063, 0.244, 0.6878   },
                { 0.2038, 0.2503, 0.7003 },
                { 0.2006, 0.2568, 0.7129 },
                { 0.1968, 0.2632, 0.7255 },
                { 0.1921, 0.2698, 0.7381 },
                { 0.1867, 0.2764, 0.7507 },
                { 0.1802, 0.2832, 0.7634 },
                { 0.1728, 0.2902, 0.7762 },
                { 0.1641, 0.2975, 0.789   },
                { 0.1541, 0.3052, 0.8017 },
                { 0.1427, 0.3132, 0.8145 },
                { 0.1295, 0.3217, 0.8269 },
                { 0.1147, 0.3306, 0.8387 },
                { 0.0986, 0.3397, 0.8495 },
                { 0.0816, 0.3486, 0.8588 },
                { 0.0646, 0.3572, 0.8664 },
                { 0.0482, 0.3651, 0.8722 },
                { 0.0329, 0.3724, 0.8765 },
                { 0.0213, 0.3792, 0.8796 },
                { 0.0136, 0.3853, 0.8815 },
                { 0.0086, 0.3911, 0.8827 },
                { 0.006,  0.3965, 0.8833 },
                { 0.0051, 0.4017, 0.8834 },
                { 0.0054, 0.4066, 0.8831 },
                { 0.0067, 0.4113, 0.8825 },
                { 0.0089, 0.4159, 0.8816 },
                { 0.0116, 0.4203, 0.8805 },
                { 0.0148, 0.4246, 0.8793 },
                { 0.0184, 0.4288, 0.8779 },
                { 0.0223, 0.4329, 0.8763 },
                { 0.0264, 0.437, 0.8747   },
                { 0.0306, 0.441, 0.8729   },
                { 0.0349, 0.4449, 0.8711 },
                { 0.0394, 0.4488, 0.8692 },
                { 0.0437, 0.4526, 0.8672 },
                { 0.0477, 0.4564, 0.8652 },
                { 0.0514, 0.4602, 0.8632 },
                { 0.0549, 0.464, 0.8611   },
                { 0.0582, 0.4677, 0.8589 },
                { 0.0612, 0.4714, 0.8568 },
                { 0.064,  0.4751, 0.8546 },
                { 0.0666, 0.4788, 0.8525 },
                { 0.0689, 0.4825, 0.8503 },
                { 0.071,  0.4862, 0.8481 },
                { 0.0729, 0.4899, 0.846   },
                { 0.0746, 0.4937, 0.8439 },
                { 0.0761, 0.4974, 0.8418 },
                { 0.0773, 0.5012, 0.8398 },
                { 0.0782, 0.5051, 0.8378 },
                { 0.0789, 0.5089, 0.8359 },
                { 0.0794, 0.5129, 0.8341 },
                { 0.0795, 0.5169, 0.8324 },
                { 0.0793, 0.521, 0.8308   },
                { 0.0788, 0.5251, 0.8293 },
                { 0.0778, 0.5295, 0.828   },
                { 0.0764, 0.5339, 0.827   },
                { 0.0746, 0.5384, 0.8261 },
                { 0.0724, 0.5431, 0.8253 },
                { 0.0698, 0.5479, 0.8247 },
                { 0.0668, 0.5527, 0.8243 },
                { 0.0636, 0.5577, 0.8239 },
                { 0.06,   0.5627, 0.8237 },
                { 0.0562, 0.5677, 0.8234 },
                { 0.0523, 0.5727, 0.8231 },
                { 0.0484, 0.5777, 0.8228 },
                { 0.0445, 0.5826, 0.8223 },
                { 0.0408, 0.5874, 0.8217 },
                { 0.0372, 0.5922, 0.8209 },
                { 0.0342, 0.5968, 0.8198 },
                { 0.0317, 0.6012, 0.8186 },
                { 0.0296, 0.6055, 0.8171 },
                { 0.0279, 0.6097, 0.8154 },
                { 0.0265, 0.6137, 0.8135 },
                { 0.0255, 0.6176, 0.8114 },
                { 0.0248, 0.6214, 0.8091 },
                { 0.0243, 0.625, 0.8066   },
                { 0.0239, 0.6285, 0.8039 },
                { 0.0237, 0.6319, 0.801   },
                { 0.0235, 0.6352, 0.798   },
                { 0.0233, 0.6384, 0.7948 },
                { 0.0231, 0.6415, 0.7916 },
                { 0.023,  0.6445, 0.7881 },
                { 0.0229, 0.6474, 0.7846 },
                { 0.0227, 0.6503, 0.781, },
                { 0.0227, 0.6531, 0.7773 },
                { 0.0232, 0.6558, 0.7735 },
                { 0.0238, 0.6585, 0.7696 },
                { 0.0246, 0.6611, 0.7656 },
                { 0.0263, 0.6637, 0.7615 },
                { 0.0282, 0.6663, 0.7574 },
                { 0.0306, 0.6688, 0.7532 },
                { 0.0338, 0.6712, 0.749   },
                { 0.0373, 0.6737, 0.7446 },
                { 0.0418, 0.6761, 0.7402 },
                { 0.0467, 0.6784, 0.7358 },
                { 0.0516, 0.6808, 0.7313 },
                { 0.0574, 0.6831, 0.7267 },
                { 0.0629, 0.6854, 0.7221 },
                { 0.0692, 0.6877, 0.7173 },
                { 0.0755, 0.6899, 0.7126 },
                { 0.082,  0.6921, 0.7078 },
                { 0.0889, 0.6943, 0.7029 },
                { 0.0956, 0.6965, 0.6979 },
                { 0.1031, 0.6986, 0.6929 },
                { 0.1104, 0.7007, 0.6878 },
                { 0.118,  0.7028, 0.6827 },
                { 0.1258, 0.7049, 0.6775 },
                { 0.1335, 0.7069, 0.6723 },
                { 0.1418, 0.7089, 0.6669 },
                { 0.1499, 0.7109, 0.6616 },
                { 0.1585, 0.7129, 0.6561 },
                { 0.1671, 0.7148, 0.6507 },
                { 0.1758, 0.7168, 0.6451 },
                { 0.1849, 0.7186, 0.6395 },
                { 0.1938, 0.7205, 0.6338 },
                { 0.2033, 0.7223, 0.6281 },
                { 0.2128, 0.7241, 0.6223 },
                { 0.2224, 0.7259, 0.6165 },
                { 0.2324, 0.7275, 0.6107 },
                { 0.2423, 0.7292, 0.6048 },
                { 0.2527, 0.7308, 0.5988 },
                { 0.2631, 0.7324, 0.5929 },
                { 0.2735, 0.7339, 0.5869 },
                { 0.2845, 0.7354, 0.5809 },
                { 0.2953, 0.7368, 0.5749 },
                { 0.3064, 0.7381, 0.5689 },
                { 0.3177, 0.7394, 0.563   },
                { 0.3289, 0.7406, 0.557   },
                { 0.3405, 0.7417, 0.5512 },
                { 0.352,  0.7428, 0.5453 },
                { 0.3635, 0.7438, 0.5396 },
                { 0.3753, 0.7446, 0.5339 },
                { 0.3869, 0.7454, 0.5283 },
                { 0.3986, 0.7461, 0.5229 },
                { 0.4103, 0.7467, 0.5175 },
                { 0.4218, 0.7473, 0.5123 },
                { 0.4334, 0.7477, 0.5072 },
                { 0.4447, 0.7482, 0.5021 },
                { 0.4561, 0.7485, 0.4972 },
                { 0.4672, 0.7487, 0.4924 },
                { 0.4783, 0.7489, 0.4877 },
                { 0.4892, 0.7491, 0.4831 },
                { 0.5,    0.7491, 0.4786 },
                { 0.5106, 0.7492, 0.4741 },
                { 0.5212, 0.7492, 0.4698 },
                { 0.5315, 0.7491, 0.4655 },
                { 0.5418, 0.749, 0.4613   },
                { 0.5519, 0.7489, 0.4571 },
                { 0.5619, 0.7487, 0.4531 },
                { 0.5718, 0.7485, 0.449   },
                { 0.5816, 0.7482, 0.4451 },
                { 0.5913, 0.7479, 0.4412 },
                { 0.6009, 0.7476, 0.4374 },
                { 0.6103, 0.7473, 0.4335 },
                { 0.6197, 0.7469, 0.4298 },
                { 0.629,  0.7465, 0.4261 },
                { 0.6382, 0.746, 0.4224   },
                { 0.6473, 0.7456, 0.4188 },
                { 0.6564, 0.7451, 0.4152 },
                { 0.6653, 0.7446, 0.4116 },
                { 0.6742, 0.7441, 0.4081 },
                { 0.683,  0.7435, 0.4046 },
                { 0.6918, 0.743, 0.4011   },
                { 0.7004, 0.7424, 0.3976 },
                { 0.7091, 0.7418, 0.3942 },
                { 0.7176, 0.7412, 0.3908 },
                { 0.7261, 0.7405, 0.3874 },
                { 0.7346, 0.7399, 0.384   },
                { 0.743,  0.7392, 0.3806 },
                { 0.7513, 0.7385, 0.3773 },
                { 0.7596, 0.7378, 0.3739 },
                { 0.7679, 0.7372, 0.3706 },
                { 0.7761, 0.7364, 0.3673 },
                { 0.7843, 0.7357, 0.3639 },
                { 0.7924, 0.735, 0.3606   },
                { 0.8005, 0.7343, 0.3573 },
                { 0.8085, 0.7336, 0.3539 },
                { 0.8166, 0.7329, 0.3506 },
                { 0.8246, 0.7322, 0.3472 },
                { 0.8325, 0.7315, 0.3438 },
                { 0.8405, 0.7308, 0.3404 },
                { 0.8484, 0.7301, 0.337   },
                { 0.8563, 0.7294, 0.3336 },
                { 0.8642, 0.7288, 0.33    },
                { 0.872,  0.7282, 0.3265 },
                { 0.8798, 0.7276, 0.3229 },
                { 0.8877, 0.7271, 0.3193 },
                { 0.8954, 0.7266, 0.3156 },
                { 0.9032, 0.7262, 0.3117 },
                { 0.911,  0.7259, 0.3078 },
                { 0.9187, 0.7256, 0.3038 },
                { 0.9264, 0.7256, 0.2996 },
                { 0.9341, 0.7256, 0.2953 },
                { 0.9417, 0.7259, 0.2907 },
                { 0.9493, 0.7264, 0.2859 },
                { 0.9567, 0.7273, 0.2808 },
                { 0.9639, 0.7285, 0.2754 },
                { 0.9708, 0.7303, 0.2696 },
                { 0.9773, 0.7326, 0.2634 },
                { 0.9831, 0.7355, 0.257   },
                { 0.9882, 0.739, 0.2504   },
                { 0.9922, 0.7431, 0.2437 },
                { 0.9952, 0.7476, 0.2373 },
                { 0.9973, 0.7524, 0.231   },
                { 0.9986, 0.7573, 0.2251 },
                { 0.9991, 0.7624, 0.2195 },
                { 0.999,  0.7675, 0.2141 },
                { 0.9985, 0.7726, 0.209   },
                { 0.9976, 0.7778, 0.2042 },
                { 0.9964, 0.7829, 0.1995 },
                { 0.995,  0.788, 0.1949   },
                { 0.9933, 0.7931, 0.1905 },
                { 0.9914, 0.7981, 0.1863 },
                { 0.9894, 0.8032, 0.1821 },
                { 0.9873, 0.8083, 0.178   },
                { 0.9851, 0.8133, 0.174   },
                { 0.9828, 0.8184, 0.17    },
                { 0.9805, 0.8235, 0.1661 },
                { 0.9782, 0.8286, 0.1622 },
                { 0.9759, 0.8337, 0.1583 },
                { 0.9736, 0.8389, 0.1544 },
                { 0.9713, 0.8441, 0.1505 },
                { 0.9692, 0.8494, 0.1465 },
                { 0.9672, 0.8548, 0.1425 },
                { 0.9654, 0.8603, 0.1385 },
                { 0.9638, 0.8659, 0.1343 },
                { 0.9623, 0.8716, 0.1301 },
                { 0.9611, 0.8774, 0.1258 },
                { 0.96,   0.8834, 0.1215 },
                { 0.9593, 0.8895, 0.1171 },
                { 0.9588, 0.8958, 0.1126 },
                { 0.9586, 0.9022, 0.1082 },
                { 0.9587, 0.9088, 0.1036 },
                { 0.9591, 0.9155, 0.099   },
                { 0.9599, 0.9225, 0.0944 },
                { 0.961,  0.9296, 0.0897 },
                { 0.9624, 0.9368, 0.085   },
                { 0.9641, 0.9443, 0.0802 },
                { 0.9662, 0.9518, 0.0753 },
                { 0.9685, 0.9595, 0.0703 },
                { 0.971,  0.9673, 0.0651 },
                { 0.9736, 0.9752, 0.0597 },
                { 0.9763, 0.9831, 0.0538 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetHeatColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.0, 0.0, 1.0 },
                { 0.0, 1.0, 1.0 },
                { 0.0, 1.0, 0.0 },
                { 1.0, 1.0, 0.0 },
                { 1.0, 0.0, 0.0 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetJetColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.0, 0.0, 0.5 },
                { 0.0, 0.0, 1.0 },
                { 0.0, 0.5, 1.0 },
                { 0.0, 1.0, 1.0 },
                { 0.5, 1.0, 0.5 },
                { 1.0, 1.0, 0.0 },
                { 1.0, 0.5, 0.0 },
                { 1.0, 0.0, 0.0 },
                { 0.5, 0.0, 0.0 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetTurboColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.18995, 0.07176, 0.23217 },
                { 0.19483, 0.08339, 0.26149 },
                { 0.19956, 0.09498, 0.29024 },
                { 0.20415, 0.10652, 0.31844 },
                { 0.20860, 0.11802, 0.34607 },
                { 0.21291, 0.12947, 0.37314 },
                { 0.21708, 0.14087, 0.39964 },
                { 0.22111, 0.15223, 0.42558 },
                { 0.22500, 0.16354, 0.45096 },
                { 0.22875, 0.17481, 0.47578 },
                { 0.23236, 0.18603, 0.50004 },
                { 0.23582, 0.19720, 0.52373 },
                { 0.23915, 0.20833, 0.54686 },
                { 0.24234, 0.21941, 0.56942 },
                { 0.24539, 0.23044, 0.59142 },
                { 0.24830, 0.24143, 0.61286 },
                { 0.25107, 0.25237, 0.63374 },
                { 0.25369, 0.26327, 0.65406 },
                { 0.25618, 0.27412, 0.67381 },
                { 0.25853, 0.28492, 0.69300 },
                { 0.26074, 0.29568, 0.71162 },
                { 0.26280, 0.30639, 0.72968 },
                { 0.26473, 0.31706, 0.74718 },
                { 0.26652, 0.32768, 0.76412 },
                { 0.26816, 0.33825, 0.78050 },
                { 0.26967, 0.34878, 0.79631 },
                { 0.27103, 0.35926, 0.81156 },
                { 0.27226, 0.36970, 0.82624 },
                { 0.27334, 0.38008, 0.84037 },
                { 0.27429, 0.39043, 0.85393 },
                { 0.27509, 0.40072, 0.86692 },
                { 0.27576, 0.41097, 0.87936 },
                { 0.27628, 0.42118, 0.89123 },
                { 0.27667, 0.43134, 0.90254 },
                { 0.27691, 0.44145, 0.91328 },
                { 0.27701, 0.45152, 0.92347 },
                { 0.27698, 0.46153, 0.93309 },
                { 0.27680, 0.47151, 0.94214 },
                { 0.27648, 0.48144, 0.95064 },
                { 0.27603, 0.49132, 0.95857 },
                { 0.27543, 0.50115, 0.96594 },
                { 0.27469, 0.51094, 0.97275 },
                { 0.27381, 0.52069, 0.97899 },
                { 0.27273, 0.53040, 0.98461 },
                { 0.27106, 0.54015, 0.98930 },
                { 0.26878, 0.54995, 0.99303 },
                { 0.26592, 0.55979, 0.99583 },
                { 0.26252, 0.56967, 0.99773 },
                { 0.25862, 0.57958, 0.99876 },
                { 0.25425, 0.58950, 0.99896 },
                { 0.24946, 0.59943, 0.99835 },
                { 0.24427, 0.60937, 0.99697 },
                { 0.23874, 0.61931, 0.99485 },
                { 0.23288, 0.62923, 0.99202 },
                { 0.22676, 0.63913, 0.98851 },
                { 0.22039, 0.64901, 0.98436 },
                { 0.21382, 0.65886, 0.97959 },
                { 0.20708, 0.66866, 0.97423 },
                { 0.20021, 0.67842, 0.96833 },
                { 0.19326, 0.68812, 0.96190 },
                { 0.18625, 0.69775, 0.95498 },
                { 0.17923, 0.70732, 0.94761 },
                { 0.17223, 0.71680, 0.93981 },
                { 0.16529, 0.72620, 0.93161 },
                { 0.15844, 0.73551, 0.92305 },
                { 0.15173, 0.74472, 0.91416 },
                { 0.14519, 0.75381, 0.90496 },
                { 0.13886, 0.76279, 0.89550 },
                { 0.13278, 0.77165, 0.88580 },
                { 0.12698, 0.78037, 0.87590 },
                { 0.12151, 0.78896, 0.86581 },
                { 0.11639, 0.79740, 0.85559 },
                { 0.11167, 0.80569, 0.84525 },
                { 0.10738, 0.81381, 0.83484 },
                { 0.10357, 0.82177, 0.82437 },
                { 0.10026, 0.82955, 0.81389 },
                { 0.09750, 0.83714, 0.80342 },
                { 0.09532, 0.84455, 0.79299 },
                { 0.09377, 0.85175, 0.78264 },
                { 0.09287, 0.85875, 0.77240 },
                { 0.09267, 0.86554, 0.76230 },
                { 0.09320, 0.87211, 0.75237 },
                { 0.09451, 0.87844, 0.74265 },
                { 0.09662, 0.88454, 0.73316 },
                { 0.09958, 0.89040, 0.72393 },
                { 0.10342, 0.89600, 0.71500 },
                { 0.10815, 0.90142, 0.70599 },
                { 0.11374, 0.90673, 0.69651 },
                { 0.12014, 0.91193, 0.68660 },
                { 0.12733, 0.91701, 0.67627 },
                { 0.13526, 0.92197, 0.66556 },
                { 0.14391, 0.92680, 0.65448 },
                { 0.15323, 0.93151, 0.64308 },
                { 0.16319, 0.93609, 0.63137 },
                { 0.17377, 0.94053, 0.61938 },
                { 0.18491, 0.94484, 0.60713 },
                { 0.19659, 0.94901, 0.59466 },
                { 0.20877, 0.95304, 0.58199 },
                { 0.22142, 0.95692, 0.56914 },
                { 0.23449, 0.96065, 0.55614 },
                { 0.24797, 0.96423, 0.54303 },
                { 0.26180, 0.96765, 0.52981 },
                { 0.27597, 0.97092, 0.51653 },
                { 0.29042, 0.97403, 0.50321 },
                { 0.30513, 0.97697, 0.48987 },
                { 0.32006, 0.97974, 0.47654 },
                { 0.33517, 0.98234, 0.46325 },
                { 0.35043, 0.98477, 0.45002 },
                { 0.36581, 0.98702, 0.43688 },
                { 0.38127, 0.98909, 0.42386 },
                { 0.39678, 0.99098, 0.41098 },
                { 0.41229, 0.99268, 0.39826 },
                { 0.42778, 0.99419, 0.38575 },
                { 0.44321, 0.99551, 0.37345 },
                { 0.45854, 0.99663, 0.36140 },
                { 0.47375, 0.99755, 0.34963 },
                { 0.48879, 0.99828, 0.33816 },
                { 0.50362, 0.99879, 0.32701 },
                { 0.51822, 0.99910, 0.31622 },
                { 0.53255, 0.99919, 0.30581 },
                { 0.54658, 0.99907, 0.29581 },
                { 0.56026, 0.99873, 0.28623 },
                { 0.57357, 0.99817, 0.27712 },
                { 0.58646, 0.99739, 0.26849 },
                { 0.59891, 0.99638, 0.26038 },
                { 0.61088, 0.99514, 0.25280 },
                { 0.62233, 0.99366, 0.24579 },
                { 0.63323, 0.99195, 0.23937 },
                { 0.64362, 0.98999, 0.23356 },
                { 0.65394, 0.98775, 0.22835 },
                { 0.66428, 0.98524, 0.22370 },
                { 0.67462, 0.98246, 0.21960 },
                { 0.68494, 0.97941, 0.21602 },
                { 0.69525, 0.97610, 0.21294 },
                { 0.70553, 0.97255, 0.21032 },
                { 0.71577, 0.96875, 0.20815 },
                { 0.72596, 0.96470, 0.20640 },
                { 0.73610, 0.96043, 0.20504 },
                { 0.74617, 0.95593, 0.20406 },
                { 0.75617, 0.95121, 0.20343 },
                { 0.76608, 0.94627, 0.20311 },
                { 0.77591, 0.94113, 0.20310 },
                { 0.78563, 0.93579, 0.20336 },
                { 0.79524, 0.93025, 0.20386 },
                { 0.80473, 0.92452, 0.20459 },
                { 0.81410, 0.91861, 0.20552 },
                { 0.82333, 0.91253, 0.20663 },
                { 0.83241, 0.90627, 0.20788 },
                { 0.84133, 0.89986, 0.20926 },
                { 0.85010, 0.89328, 0.21074 },
                { 0.85868, 0.88655, 0.21230 },
                { 0.86709, 0.87968, 0.21391 },
                { 0.87530, 0.87267, 0.21555 },
                { 0.88331, 0.86553, 0.21719 },
                { 0.89112, 0.85826, 0.21880 },
                { 0.89870, 0.85087, 0.22038 },
                { 0.90605, 0.84337, 0.22188 },
                { 0.91317, 0.83576, 0.22328 },
                { 0.92004, 0.82806, 0.22456 },
                { 0.92666, 0.82025, 0.22570 },
                { 0.93301, 0.81236, 0.22667 },
                { 0.93909, 0.80439, 0.22744 },
                { 0.94489, 0.79634, 0.22800 },
                { 0.95039, 0.78823, 0.22831 },
                { 0.95560, 0.78005, 0.22836 },
                { 0.96049, 0.77181, 0.22811 },
                { 0.96507, 0.76352, 0.22754 },
                { 0.96931, 0.75519, 0.22663 },
                { 0.97323, 0.74682, 0.22536 },
                { 0.97679, 0.73842, 0.22369 },
                { 0.98000, 0.73000, 0.22161 },
                { 0.98289, 0.72140, 0.21918 },
                { 0.98549, 0.71250, 0.21650 },
                { 0.98781, 0.70330, 0.21358 },
                { 0.98986, 0.69382, 0.21043 },
                { 0.99163, 0.68408, 0.20706 },
                { 0.99314, 0.67408, 0.20348 },
                { 0.99438, 0.66386, 0.19971 },
                { 0.99535, 0.65341, 0.19577 },
                { 0.99607, 0.64277, 0.19165 },
                { 0.99654, 0.63193, 0.18738 },
                { 0.99675, 0.62093, 0.18297 },
                { 0.99672, 0.60977, 0.17842 },
                { 0.99644, 0.59846, 0.17376 },
                { 0.99593, 0.58703, 0.16899 },
                { 0.99517, 0.57549, 0.16412 },
                { 0.99419, 0.56386, 0.15918 },
                { 0.99297, 0.55214, 0.15417 },
                { 0.99153, 0.54036, 0.14910 },
                { 0.98987, 0.52854, 0.14398 },
                { 0.98799, 0.51667, 0.13883 },
                { 0.98590, 0.50479, 0.13367 },
                { 0.98360, 0.49291, 0.12849 },
                { 0.98108, 0.48104, 0.12332 },
                { 0.97837, 0.46920, 0.11817 },
                { 0.97545, 0.45740, 0.11305 },
                { 0.97234, 0.44565, 0.10797 },
                { 0.96904, 0.43399, 0.10294 },
                { 0.96555, 0.42241, 0.09798 },
                { 0.96187, 0.41093, 0.09310 },
                { 0.95801, 0.39958, 0.08831 },
                { 0.95398, 0.38836, 0.08362 },
                { 0.94977, 0.37729, 0.07905 },
                { 0.94538, 0.36638, 0.07461 },
                { 0.94084, 0.35566, 0.07031 },
                { 0.93612, 0.34513, 0.06616 },
                { 0.93125, 0.33482, 0.06218 },
                { 0.92623, 0.32473, 0.05837 },
                { 0.92105, 0.31489, 0.05475 },
                { 0.91572, 0.30530, 0.05134 },
                { 0.91024, 0.29599, 0.04814 },
                { 0.90463, 0.28696, 0.04516 },
                { 0.89888, 0.27824, 0.04243 },
                { 0.89298, 0.26981, 0.03993 },
                { 0.88691, 0.26152, 0.03753 },
                { 0.88066, 0.25334, 0.03521 },
                { 0.87422, 0.24526, 0.03297 },
                { 0.86760, 0.23730, 0.03082 },
                { 0.86079, 0.22945, 0.02875 },
                { 0.85380, 0.22170, 0.02677 },
                { 0.84662, 0.21407, 0.02487 },
                { 0.83926, 0.20654, 0.02305 },
                { 0.83172, 0.19912, 0.02131 },
                { 0.82399, 0.19182, 0.01966 },
                { 0.81608, 0.18462, 0.01809 },
                { 0.80799, 0.17753, 0.01660 },
                { 0.79971, 0.17055, 0.01520 },
                { 0.79125, 0.16368, 0.01387 },
                { 0.78260, 0.15693, 0.01264 },
                { 0.77377, 0.15028, 0.01148 },
                { 0.76476, 0.14374, 0.01041 },
                { 0.75556, 0.13731, 0.00942 },
                { 0.74617, 0.13098, 0.00851 },
                { 0.73661, 0.12477, 0.00769 },
                { 0.72686, 0.11867, 0.00695 },
                { 0.71692, 0.11268, 0.00629 },
                { 0.70680, 0.10680, 0.00571 },
                { 0.69650, 0.10102, 0.00522 },
                { 0.68602, 0.09536, 0.00481 },
                { 0.67535, 0.08980, 0.00449 },
                { 0.66449, 0.08436, 0.00424 },
                { 0.65345, 0.07902, 0.00408 },
                { 0.64223, 0.07380, 0.00401 },
                { 0.63082, 0.06868, 0.00401 },
                { 0.61923, 0.06367, 0.00410 },
                { 0.60746, 0.05878, 0.00427 },
                { 0.59550, 0.05399, 0.00453 },
                { 0.58336, 0.04931, 0.00486 },
                { 0.57103, 0.04474, 0.00529 },
                { 0.55852, 0.04028, 0.00579 },
                { 0.54583, 0.03593, 0.00638 },
                { 0.53295, 0.03169, 0.00705 },
                { 0.51989, 0.02756, 0.00780 },
                { 0.50664, 0.02354, 0.00863 },
                { 0.49321, 0.01963, 0.00955 },
                { 0.47960, 0.01583, 0.01055 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetHotColor(double x)
    {
        x = internal::Clamp01(x);

        constexpr Color r{ 1.0, 0.0, 0.0 };
        constexpr Color g{ 0.0, 1.0, 0.0 };
        constexpr Color b{ 0.0, 0.0, 1.0 };

        if (x < 0.4)
        {
            const double t = x / 0.4;
            return t * r;
        }
        else if (x < 0.8)
        {
            const double t = (x - 0.4) / (0.8 - 0.4);
            return r + t * g;
        }
        else
        {
            const double t = (x - 0.8) / (1.0 - 0.8);
            return r + g + t * b;
        }
    }

    inline constexpr Color GetGrayColor(double x) noexcept
    {
        return Color{ 1.0 - internal::Clamp01(x) };
    }

    inline Color GetMagmaColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.001462, 0.000466, 0.013866 },
                { 0.002258, 0.001295, 0.018331 },
                { 0.003279, 0.002305, 0.023708 },
                { 0.004512, 0.003490, 0.029965 },
                { 0.005950, 0.004843, 0.037130 },
                { 0.007588, 0.006356, 0.044973 },
                { 0.009426, 0.008022, 0.052844 },
                { 0.011465, 0.009828, 0.060750 },
                { 0.013708, 0.011771, 0.068667 },
                { 0.016156, 0.013840, 0.076603 },
                { 0.018815, 0.016026, 0.084584 },
                { 0.021692, 0.018320, 0.092610 },
                { 0.024792, 0.020715, 0.100676 },
                { 0.028123, 0.023201, 0.108787 },
                { 0.031696, 0.025765, 0.116965 },
                { 0.035520, 0.028397, 0.125209 },
                { 0.039608, 0.031090, 0.133515 },
                { 0.043830, 0.033830, 0.141886 },
                { 0.048062, 0.036607, 0.150327 },
                { 0.052320, 0.039407, 0.158841 },
                { 0.056615, 0.042160, 0.167446 },
                { 0.060949, 0.044794, 0.176129 },
                { 0.065330, 0.047318, 0.184892 },
                { 0.069764, 0.049726, 0.193735 },
                { 0.074257, 0.052017, 0.202660 },
                { 0.078815, 0.054184, 0.211667 },
                { 0.083446, 0.056225, 0.220755 },
                { 0.088155, 0.058133, 0.229922 },
                { 0.092949, 0.059904, 0.239164 },
                { 0.097833, 0.061531, 0.248477 },
                { 0.102815, 0.063010, 0.257854 },
                { 0.107899, 0.064335, 0.267289 },
                { 0.113094, 0.065492, 0.276784 },
                { 0.118405, 0.066479, 0.286321 },
                { 0.123833, 0.067295, 0.295879 },
                { 0.129380, 0.067935, 0.305443 },
                { 0.135053, 0.068391, 0.315000 },
                { 0.140858, 0.068654, 0.324538 },
                { 0.146785, 0.068738, 0.334011 },
                { 0.152839, 0.068637, 0.343404 },
                { 0.159018, 0.068354, 0.352688 },
                { 0.165308, 0.067911, 0.361816 },
                { 0.171713, 0.067305, 0.370771 },
                { 0.178212, 0.066576, 0.379497 },
                { 0.184801, 0.065732, 0.387973 },
                { 0.191460, 0.064818, 0.396152 },
                { 0.198177, 0.063862, 0.404009 },
                { 0.204935, 0.062907, 0.411514 },
                { 0.211718, 0.061992, 0.418647 },
                { 0.218512, 0.061158, 0.425392 },
                { 0.225302, 0.060445, 0.431742 },
                { 0.232077, 0.059889, 0.437695 },
                { 0.238826, 0.059517, 0.443256 },
                { 0.245543, 0.059352, 0.448436 },
                { 0.252220, 0.059415, 0.453248 },
                { 0.258857, 0.059706, 0.457710 },
                { 0.265447, 0.060237, 0.461840 },
                { 0.271994, 0.060994, 0.465660 },
                { 0.278493, 0.061978, 0.469190 },
                { 0.284951, 0.063168, 0.472451 },
                { 0.291366, 0.064553, 0.475462 },
                { 0.297740, 0.066117, 0.478243 },
                { 0.304081, 0.067835, 0.480812 },
                { 0.310382, 0.069702, 0.483186 },
                { 0.316654, 0.071690, 0.485380 },
                { 0.322899, 0.073782, 0.487408 },
                { 0.329114, 0.075972, 0.489287 },
                { 0.335308, 0.078236, 0.491024 },
                { 0.341482, 0.080564, 0.492631 },
                { 0.347636, 0.082946, 0.494121 },
                { 0.353773, 0.085373, 0.495501 },
                { 0.359898, 0.087831, 0.496778 },
                { 0.366012, 0.090314, 0.497960 },
                { 0.372116, 0.092816, 0.499053 },
                { 0.378211, 0.095332, 0.500067 },
                { 0.384299, 0.097855, 0.501002 },
                { 0.390384, 0.100379, 0.501864 },
                { 0.396467, 0.102902, 0.502658 },
                { 0.402548, 0.105420, 0.503386 },
                { 0.408629, 0.107930, 0.504052 },
                { 0.414709, 0.110431, 0.504662 },
                { 0.420791, 0.112920, 0.505215 },
                { 0.426877, 0.115395, 0.505714 },
                { 0.432967, 0.117855, 0.506160 },
                { 0.439062, 0.120298, 0.506555 },
                { 0.445163, 0.122724, 0.506901 },
                { 0.451271, 0.125132, 0.507198 },
                { 0.457386, 0.127522, 0.507448 },
                { 0.463508, 0.129893, 0.507652 },
                { 0.469640, 0.132245, 0.507809 },
                { 0.475780, 0.134577, 0.507921 },
                { 0.481929, 0.136891, 0.507989 },
                { 0.488088, 0.139186, 0.508011 },
                { 0.494258, 0.141462, 0.507988 },
                { 0.500438, 0.143719, 0.507920 },
                { 0.506629, 0.145958, 0.507806 },
                { 0.512831, 0.148179, 0.507648 },
                { 0.519045, 0.150383, 0.507443 },
                { 0.525270, 0.152569, 0.507192 },
                { 0.531507, 0.154739, 0.506895 },
                { 0.537755, 0.156894, 0.506551 },
                { 0.544015, 0.159033, 0.506159 },
                { 0.550287, 0.161158, 0.505719 },
                { 0.556571, 0.163269, 0.505230 },
                { 0.562866, 0.165368, 0.504692 },
                { 0.569172, 0.167454, 0.504105 },
                { 0.575490, 0.169530, 0.503466 },
                { 0.581819, 0.171596, 0.502777 },
                { 0.588158, 0.173652, 0.502035 },
                { 0.594508, 0.175701, 0.501241 },
                { 0.600868, 0.177743, 0.500394 },
                { 0.607238, 0.179779, 0.499492 },
                { 0.613617, 0.181811, 0.498536 },
                { 0.620005, 0.183840, 0.497524 },
                { 0.626401, 0.185867, 0.496456 },
                { 0.632805, 0.187893, 0.495332 },
                { 0.639216, 0.189921, 0.494150 },
                { 0.645633, 0.191952, 0.492910 },
                { 0.652056, 0.193986, 0.491611 },
                { 0.658483, 0.196027, 0.490253 },
                { 0.664915, 0.198075, 0.488836 },
                { 0.671349, 0.200133, 0.487358 },
                { 0.677786, 0.202203, 0.485819 },
                { 0.684224, 0.204286, 0.484219 },
                { 0.690661, 0.206384, 0.482558 },
                { 0.697098, 0.208501, 0.480835 },
                { 0.703532, 0.210638, 0.479049 },
                { 0.709962, 0.212797, 0.477201 },
                { 0.716387, 0.214982, 0.475290 },
                { 0.722805, 0.217194, 0.473316 },
                { 0.729216, 0.219437, 0.471279 },
                { 0.735616, 0.221713, 0.469180 },
                { 0.742004, 0.224025, 0.467018 },
                { 0.748378, 0.226377, 0.464794 },
                { 0.754737, 0.228772, 0.462509 },
                { 0.761077, 0.231214, 0.460162 },
                { 0.767398, 0.233705, 0.457755 },
                { 0.773695, 0.236249, 0.455289 },
                { 0.779968, 0.238851, 0.452765 },
                { 0.786212, 0.241514, 0.450184 },
                { 0.792427, 0.244242, 0.447543 },
                { 0.798608, 0.247040, 0.444848 },
                { 0.804752, 0.249911, 0.442102 },
                { 0.810855, 0.252861, 0.439305 },
                { 0.816914, 0.255895, 0.436461 },
                { 0.822926, 0.259016, 0.433573 },
                { 0.828886, 0.262229, 0.430644 },
                { 0.834791, 0.265540, 0.427671 },
                { 0.840636, 0.268953, 0.424666 },
                { 0.846416, 0.272473, 0.421631 },
                { 0.852126, 0.276106, 0.418573 },
                { 0.857763, 0.279857, 0.415496 },
                { 0.863320, 0.283729, 0.412403 },
                { 0.868793, 0.287728, 0.409303 },
                { 0.874176, 0.291859, 0.406205 },
                { 0.879464, 0.296125, 0.403118 },
                { 0.884651, 0.300530, 0.400047 },
                { 0.889731, 0.305079, 0.397002 },
                { 0.894700, 0.309773, 0.393995 },
                { 0.899552, 0.314616, 0.391037 },
                { 0.904281, 0.319610, 0.388137 },
                { 0.908884, 0.324755, 0.385308 },
                { 0.913354, 0.330052, 0.382563 },
                { 0.917689, 0.335500, 0.379915 },
                { 0.921884, 0.341098, 0.377376 },
                { 0.925937, 0.346844, 0.374959 },
                { 0.929845, 0.352734, 0.372677 },
                { 0.933606, 0.358764, 0.370541 },
                { 0.937221, 0.364929, 0.368567 },
                { 0.940687, 0.371224, 0.366762 },
                { 0.944006, 0.377643, 0.365136 },
                { 0.947180, 0.384178, 0.363701 },
                { 0.950210, 0.390820, 0.362468 },
                { 0.953099, 0.397563, 0.361438 },
                { 0.955849, 0.404400, 0.360619 },
                { 0.958464, 0.411324, 0.360014 },
                { 0.960949, 0.418323, 0.359630 },
                { 0.963310, 0.425390, 0.359469 },
                { 0.965549, 0.432519, 0.359529 },
                { 0.967671, 0.439703, 0.359810 },
                { 0.969680, 0.446936, 0.360311 },
                { 0.971582, 0.454210, 0.361030 },
                { 0.973381, 0.461520, 0.361965 },
                { 0.975082, 0.468861, 0.363111 },
                { 0.976690, 0.476226, 0.364466 },
                { 0.978210, 0.483612, 0.366025 },
                { 0.979645, 0.491014, 0.367783 },
                { 0.981000, 0.498428, 0.369734 },
                { 0.982279, 0.505851, 0.371874 },
                { 0.983485, 0.513280, 0.374198 },
                { 0.984622, 0.520713, 0.376698 },
                { 0.985693, 0.528148, 0.379371 },
                { 0.986700, 0.535582, 0.382210 },
                { 0.987646, 0.543015, 0.385210 },
                { 0.988533, 0.550446, 0.388365 },
                { 0.989363, 0.557873, 0.391671 },
                { 0.990138, 0.565296, 0.395122 },
                { 0.990871, 0.572706, 0.398714 },
                { 0.991558, 0.580107, 0.402441 },
                { 0.992196, 0.587502, 0.406299 },
                { 0.992785, 0.594891, 0.410283 },
                { 0.993326, 0.602275, 0.414390 },
                { 0.993834, 0.609644, 0.418613 },
                { 0.994309, 0.616999, 0.422950 },
                { 0.994738, 0.624350, 0.427397 },
                { 0.995122, 0.631696, 0.431951 },
                { 0.995480, 0.639027, 0.436607 },
                { 0.995810, 0.646344, 0.441361 },
                { 0.996096, 0.653659, 0.446213 },
                { 0.996341, 0.660969, 0.451160 },
                { 0.996580, 0.668256, 0.456192 },
                { 0.996775, 0.675541, 0.461314 },
                { 0.996925, 0.682828, 0.466526 },
                { 0.997077, 0.690088, 0.471811 },
                { 0.997186, 0.697349, 0.477182 },
                { 0.997254, 0.704611, 0.482635 },
                { 0.997325, 0.711848, 0.488154 },
                { 0.997351, 0.719089, 0.493755 },
                { 0.997351, 0.726324, 0.499428 },
                { 0.997341, 0.733545, 0.505167 },
                { 0.997285, 0.740772, 0.510983 },
                { 0.997228, 0.747981, 0.516859 },
                { 0.997138, 0.755190, 0.522806 },
                { 0.997019, 0.762398, 0.528821 },
                { 0.996898, 0.769591, 0.534892 },
                { 0.996727, 0.776795, 0.541039 },
                { 0.996571, 0.783977, 0.547233 },
                { 0.996369, 0.791167, 0.553499 },
                { 0.996162, 0.798348, 0.559820 },
                { 0.995932, 0.805527, 0.566202 },
                { 0.995680, 0.812706, 0.572645 },
                { 0.995424, 0.819875, 0.579140 },
                { 0.995131, 0.827052, 0.585701 },
                { 0.994851, 0.834213, 0.592307 },
                { 0.994524, 0.841387, 0.598983 },
                { 0.994222, 0.848540, 0.605696 },
                { 0.993866, 0.855711, 0.612482 },
                { 0.993545, 0.862859, 0.619299 },
                { 0.993170, 0.870024, 0.626189 },
                { 0.992831, 0.877168, 0.633109 },
                { 0.992440, 0.884330, 0.640099 },
                { 0.992089, 0.891470, 0.647116 },
                { 0.991688, 0.898627, 0.654202 },
                { 0.991332, 0.905763, 0.661309 },
                { 0.990930, 0.912915, 0.668481 },
                { 0.990570, 0.920049, 0.675675 },
                { 0.990175, 0.927196, 0.682926 },
                { 0.989815, 0.934329, 0.690198 },
                { 0.989434, 0.941470, 0.697519 },
                { 0.989077, 0.948604, 0.704863 },
                { 0.988717, 0.955742, 0.712242 },
                { 0.988367, 0.962878, 0.719649 },
                { 0.988033, 0.970012, 0.727077 },
                { 0.987691, 0.977154, 0.734536 },
                { 0.987387, 0.984288, 0.742002 },
                { 0.987053, 0.991438, 0.749504 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetInfernoColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.001462, 0.000466, 0.013866 },
                { 0.002267, 0.001270, 0.018570 },
                { 0.003299, 0.002249, 0.024239 },
                { 0.004547, 0.003392, 0.030909 },
                { 0.006006, 0.004692, 0.038558 },
                { 0.007676, 0.006136, 0.046836 },
                { 0.009561, 0.007713, 0.055143 },
                { 0.011663, 0.009417, 0.063460 },
                { 0.013995, 0.011225, 0.071862 },
                { 0.016561, 0.013136, 0.080282 },
                { 0.019373, 0.015133, 0.088767 },
                { 0.022447, 0.017199, 0.097327 },
                { 0.025793, 0.019331, 0.105930 },
                { 0.029432, 0.021503, 0.114621 },
                { 0.033385, 0.023702, 0.123397 },
                { 0.037668, 0.025921, 0.132232 },
                { 0.042253, 0.028139, 0.141141 },
                { 0.046915, 0.030324, 0.150164 },
                { 0.051644, 0.032474, 0.159254 },
                { 0.056449, 0.034569, 0.168414 },
                { 0.061340, 0.036590, 0.177642 },
                { 0.066331, 0.038504, 0.186962 },
                { 0.071429, 0.040294, 0.196354 },
                { 0.076637, 0.041905, 0.205799 },
                { 0.081962, 0.043328, 0.215289 },
                { 0.087411, 0.044556, 0.224813 },
                { 0.092990, 0.045583, 0.234358 },
                { 0.098702, 0.046402, 0.243904 },
                { 0.104551, 0.047008, 0.253430 },
                { 0.110536, 0.047399, 0.262912 },
                { 0.116656, 0.047574, 0.272321 },
                { 0.122908, 0.047536, 0.281624 },
                { 0.129285, 0.047293, 0.290788 },
                { 0.135778, 0.046856, 0.299776 },
                { 0.142378, 0.046242, 0.308553 },
                { 0.149073, 0.045468, 0.317085 },
                { 0.155850, 0.044559, 0.325338 },
                { 0.162689, 0.043554, 0.333277 },
                { 0.169575, 0.042489, 0.340874 },
                { 0.176493, 0.041402, 0.348111 },
                { 0.183429, 0.040329, 0.354971 },
                { 0.190367, 0.039309, 0.361447 },
                { 0.197297, 0.038400, 0.367535 },
                { 0.204209, 0.037632, 0.373238 },
                { 0.211095, 0.037030, 0.378563 },
                { 0.217949, 0.036615, 0.383522 },
                { 0.224763, 0.036405, 0.388129 },
                { 0.231538, 0.036405, 0.392400 },
                { 0.238273, 0.036621, 0.396353 },
                { 0.244967, 0.037055, 0.400007 },
                { 0.251620, 0.037705, 0.403378 },
                { 0.258234, 0.038571, 0.406485 },
                { 0.264810, 0.039647, 0.409345 },
                { 0.271347, 0.040922, 0.411976 },
                { 0.277850, 0.042353, 0.414392 },
                { 0.284321, 0.043933, 0.416608 },
                { 0.290763, 0.045644, 0.418637 },
                { 0.297178, 0.047470, 0.420491 },
                { 0.303568, 0.049396, 0.422182 },
                { 0.309935, 0.051407, 0.423721 },
                { 0.316282, 0.053490, 0.425116 },
                { 0.322610, 0.055634, 0.426377 },
                { 0.328921, 0.057827, 0.427511 },
                { 0.335217, 0.060060, 0.428524 },
                { 0.341500, 0.062325, 0.429425 },
                { 0.347771, 0.064616, 0.430217 },
                { 0.354032, 0.066925, 0.430906 },
                { 0.360284, 0.069247, 0.431497 },
                { 0.366529, 0.071579, 0.431994 },
                { 0.372768, 0.073915, 0.432400 },
                { 0.379001, 0.076253, 0.432719 },
                { 0.385228, 0.078591, 0.432955 },
                { 0.391453, 0.080927, 0.433109 },
                { 0.397674, 0.083257, 0.433183 },
                { 0.403894, 0.085580, 0.433179 },
                { 0.410113, 0.087896, 0.433098 },
                { 0.416331, 0.090203, 0.432943 },
                { 0.422549, 0.092501, 0.432714 },
                { 0.428768, 0.094790, 0.432412 },
                { 0.434987, 0.097069, 0.432039 },
                { 0.441207, 0.099338, 0.431594 },
                { 0.447428, 0.101597, 0.431080 },
                { 0.453651, 0.103848, 0.430498 },
                { 0.459875, 0.106089, 0.429846 },
                { 0.466100, 0.108322, 0.429125 },
                { 0.472328, 0.110547, 0.428334 },
                { 0.478558, 0.112764, 0.427475 },
                { 0.484789, 0.114974, 0.426548 },
                { 0.491022, 0.117179, 0.425552 },
                { 0.497257, 0.119379, 0.424488 },
                { 0.503493, 0.121575, 0.423356 },
                { 0.509730, 0.123769, 0.422156 },
                { 0.515967, 0.125960, 0.420887 },
                { 0.522206, 0.128150, 0.419549 },
                { 0.528444, 0.130341, 0.418142 },
                { 0.534683, 0.132534, 0.416667 },
                { 0.540920, 0.134729, 0.415123 },
                { 0.547157, 0.136929, 0.413511 },
                { 0.553392, 0.139134, 0.411829 },
                { 0.559624, 0.141346, 0.410078 },
                { 0.565854, 0.143567, 0.408258 },
                { 0.572081, 0.145797, 0.406369 },
                { 0.578304, 0.148039, 0.404411 },
                { 0.584521, 0.150294, 0.402385 },
                { 0.590734, 0.152563, 0.400290 },
                { 0.596940, 0.154848, 0.398125 },
                { 0.603139, 0.157151, 0.395891 },
                { 0.609330, 0.159474, 0.393589 },
                { 0.615513, 0.161817, 0.391219 },
                { 0.621685, 0.164184, 0.388781 },
                { 0.627847, 0.166575, 0.386276 },
                { 0.633998, 0.168992, 0.383704 },
                { 0.640135, 0.171438, 0.381065 },
                { 0.646260, 0.173914, 0.378359 },
                { 0.652369, 0.176421, 0.375586 },
                { 0.658463, 0.178962, 0.372748 },
                { 0.664540, 0.181539, 0.369846 },
                { 0.670599, 0.184153, 0.366879 },
                { 0.676638, 0.186807, 0.363849 },
                { 0.682656, 0.189501, 0.360757 },
                { 0.688653, 0.192239, 0.357603 },
                { 0.694627, 0.195021, 0.354388 },
                { 0.700576, 0.197851, 0.351113 },
                { 0.706500, 0.200728, 0.347777 },
                { 0.712396, 0.203656, 0.344383 },
                { 0.718264, 0.206636, 0.340931 },
                { 0.724103, 0.209670, 0.337424 },
                { 0.729909, 0.212759, 0.333861 },
                { 0.735683, 0.215906, 0.330245 },
                { 0.741423, 0.219112, 0.326576 },
                { 0.747127, 0.222378, 0.322856 },
                { 0.752794, 0.225706, 0.319085 },
                { 0.758422, 0.229097, 0.315266 },
                { 0.764010, 0.232554, 0.311399 },
                { 0.769556, 0.236077, 0.307485 },
                { 0.775059, 0.239667, 0.303526 },
                { 0.780517, 0.243327, 0.299523 },
                { 0.785929, 0.247056, 0.295477 },
                { 0.791293, 0.250856, 0.291390 },
                { 0.796607, 0.254728, 0.287264 },
                { 0.801871, 0.258674, 0.283099 },
                { 0.807082, 0.262692, 0.278898 },
                { 0.812239, 0.266786, 0.274661 },
                { 0.817341, 0.270954, 0.270390 },
                { 0.822386, 0.275197, 0.266085 },
                { 0.827372, 0.279517, 0.261750 },
                { 0.832299, 0.283913, 0.257383 },
                { 0.837165, 0.288385, 0.252988 },
                { 0.841969, 0.292933, 0.248564 },
                { 0.846709, 0.297559, 0.244113 },
                { 0.851384, 0.302260, 0.239636 },
                { 0.855992, 0.307038, 0.235133 },
                { 0.860533, 0.311892, 0.230606 },
                { 0.865006, 0.316822, 0.226055 },
                { 0.869409, 0.321827, 0.221482 },
                { 0.873741, 0.326906, 0.216886 },
                { 0.878001, 0.332060, 0.212268 },
                { 0.882188, 0.337287, 0.207628 },
                { 0.886302, 0.342586, 0.202968 },
                { 0.890341, 0.347957, 0.198286 },
                { 0.894305, 0.353399, 0.193584 },
                { 0.898192, 0.358911, 0.188860 },
                { 0.902003, 0.364492, 0.184116 },
                { 0.905735, 0.370140, 0.179350 },
                { 0.909390, 0.375856, 0.174563 },
                { 0.912966, 0.381636, 0.169755 },
                { 0.916462, 0.387481, 0.164924 },
                { 0.919879, 0.393389, 0.160070 },
                { 0.923215, 0.399359, 0.155193 },
                { 0.926470, 0.405389, 0.150292 },
                { 0.929644, 0.411479, 0.145367 },
                { 0.932737, 0.417627, 0.140417 },
                { 0.935747, 0.423831, 0.135440 },
                { 0.938675, 0.430091, 0.130438 },
                { 0.941521, 0.436405, 0.125409 },
                { 0.944285, 0.442772, 0.120354 },
                { 0.946965, 0.449191, 0.115272 },
                { 0.949562, 0.455660, 0.110164 },
                { 0.952075, 0.462178, 0.105031 },
                { 0.954506, 0.468744, 0.099874 },
                { 0.956852, 0.475356, 0.094695 },
                { 0.959114, 0.482014, 0.089499 },
                { 0.961293, 0.488716, 0.084289 },
                { 0.963387, 0.495462, 0.079073 },
                { 0.965397, 0.502249, 0.073859 },
                { 0.967322, 0.509078, 0.068659 },
                { 0.969163, 0.515946, 0.063488 },
                { 0.970919, 0.522853, 0.058367 },
                { 0.972590, 0.529798, 0.053324 },
                { 0.974176, 0.536780, 0.048392 },
                { 0.975677, 0.543798, 0.043618 },
                { 0.977092, 0.550850, 0.039050 },
                { 0.978422, 0.557937, 0.034931 },
                { 0.979666, 0.565057, 0.031409 },
                { 0.980824, 0.572209, 0.028508 },
                { 0.981895, 0.579392, 0.026250 },
                { 0.982881, 0.586606, 0.024661 },
                { 0.983779, 0.593849, 0.023770 },
                { 0.984591, 0.601122, 0.023606 },
                { 0.985315, 0.608422, 0.024202 },
                { 0.985952, 0.615750, 0.025592 },
                { 0.986502, 0.623105, 0.027814 },
                { 0.986964, 0.630485, 0.030908 },
                { 0.987337, 0.637890, 0.034916 },
                { 0.987622, 0.645320, 0.039886 },
                { 0.987819, 0.652773, 0.045581 },
                { 0.987926, 0.660250, 0.051750 },
                { 0.987945, 0.667748, 0.058329 },
                { 0.987874, 0.675267, 0.065257 },
                { 0.987714, 0.682807, 0.072489 },
                { 0.987464, 0.690366, 0.079990 },
                { 0.987124, 0.697944, 0.087731 },
                { 0.986694, 0.705540, 0.095694 },
                { 0.986175, 0.713153, 0.103863 },
                { 0.985566, 0.720782, 0.112229 },
                { 0.984865, 0.728427, 0.120785 },
                { 0.984075, 0.736087, 0.129527 },
                { 0.983196, 0.743758, 0.138453 },
                { 0.982228, 0.751442, 0.147565 },
                { 0.981173, 0.759135, 0.156863 },
                { 0.980032, 0.766837, 0.166353 },
                { 0.978806, 0.774545, 0.176037 },
                { 0.977497, 0.782258, 0.185923 },
                { 0.976108, 0.789974, 0.196018 },
                { 0.974638, 0.797692, 0.206332 },
                { 0.973088, 0.805409, 0.216877 },
                { 0.971468, 0.813122, 0.227658 },
                { 0.969783, 0.820825, 0.238686 },
                { 0.968041, 0.828515, 0.249972 },
                { 0.966243, 0.836191, 0.261534 },
                { 0.964394, 0.843848, 0.273391 },
                { 0.962517, 0.851476, 0.285546 },
                { 0.960626, 0.859069, 0.298010 },
                { 0.958720, 0.866624, 0.310820 },
                { 0.956834, 0.874129, 0.323974 },
                { 0.954997, 0.881569, 0.337475 },
                { 0.953215, 0.888942, 0.351369 },
                { 0.951546, 0.896226, 0.365627 },
                { 0.950018, 0.903409, 0.380271 },
                { 0.948683, 0.910473, 0.395289 },
                { 0.947594, 0.917399, 0.410665 },
                { 0.946809, 0.924168, 0.426373 },
                { 0.946392, 0.930761, 0.442367 },
                { 0.946403, 0.937159, 0.458592 },
                { 0.946903, 0.943348, 0.474970 },
                { 0.947937, 0.949318, 0.491426 },
                { 0.949545, 0.955063, 0.507860 },
                { 0.951740, 0.960587, 0.524203 },
                { 0.954529, 0.965896, 0.540361 },
                { 0.957896, 0.971003, 0.556275 },
                { 0.961812, 0.975924, 0.571925 },
                { 0.966249, 0.980678, 0.587206 },
                { 0.971162, 0.985282, 0.602154 },
                { 0.976511, 0.989753, 0.616760 },
                { 0.982257, 0.994109, 0.631017 },
                { 0.988362, 0.998364, 0.644924 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetPlasmaColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.050383, 0.029803, 0.527975 },
                { 0.063536, 0.028426, 0.533124 },
                { 0.075353, 0.027206, 0.538007 },
                { 0.086222, 0.026125, 0.542658 },
                { 0.096379, 0.025165, 0.547103 },
                { 0.105980, 0.024309, 0.551368 },
                { 0.115124, 0.023556, 0.555468 },
                { 0.123903, 0.022878, 0.559423 },
                { 0.132381, 0.022258, 0.563250 },
                { 0.140603, 0.021687, 0.566959 },
                { 0.148607, 0.021154, 0.570562 },
                { 0.156421, 0.020651, 0.574065 },
                { 0.164070, 0.020171, 0.577478 },
                { 0.171574, 0.019706, 0.580806 },
                { 0.178950, 0.019252, 0.584054 },
                { 0.186213, 0.018803, 0.587228 },
                { 0.193374, 0.018354, 0.590330 },
                { 0.200445, 0.017902, 0.593364 },
                { 0.207435, 0.017442, 0.596333 },
                { 0.214350, 0.016973, 0.599239 },
                { 0.221197, 0.016497, 0.602083 },
                { 0.227983, 0.016007, 0.604867 },
                { 0.234715, 0.015502, 0.607592 },
                { 0.241396, 0.014979, 0.610259 },
                { 0.248032, 0.014439, 0.612868 },
                { 0.254627, 0.013882, 0.615419 },
                { 0.261183, 0.013308, 0.617911 },
                { 0.267703, 0.012716, 0.620346 },
                { 0.274191, 0.012109, 0.622722 },
                { 0.280648, 0.011488, 0.625038 },
                { 0.287076, 0.010855, 0.627295 },
                { 0.293478, 0.010213, 0.629490 },
                { 0.299855, 0.009561, 0.631624 },
                { 0.306210, 0.008902, 0.633694 },
                { 0.312543, 0.008239, 0.635700 },
                { 0.318856, 0.007576, 0.637640 },
                { 0.325150, 0.006915, 0.639512 },
                { 0.331426, 0.006261, 0.641316 },
                { 0.337683, 0.005618, 0.643049 },
                { 0.343925, 0.004991, 0.644710 },
                { 0.350150, 0.004382, 0.646298 },
                { 0.356359, 0.003798, 0.647810 },
                { 0.362553, 0.003243, 0.649245 },
                { 0.368733, 0.002724, 0.650601 },
                { 0.374897, 0.002245, 0.651876 },
                { 0.381047, 0.001814, 0.653068 },
                { 0.387183, 0.001434, 0.654177 },
                { 0.393304, 0.001114, 0.655199 },
                { 0.399411, 0.000859, 0.656133 },
                { 0.405503, 0.000678, 0.656977 },
                { 0.411580, 0.000577, 0.657730 },
                { 0.417642, 0.000564, 0.658390 },
                { 0.423689, 0.000646, 0.658956 },
                { 0.429719, 0.000831, 0.659425 },
                { 0.435734, 0.001127, 0.659797 },
                { 0.441732, 0.001540, 0.660069 },
                { 0.447714, 0.002080, 0.660240 },
                { 0.453677, 0.002755, 0.660310 },
                { 0.459623, 0.003574, 0.660277 },
                { 0.465550, 0.004545, 0.660139 },
                { 0.471457, 0.005678, 0.659897 },
                { 0.477344, 0.006980, 0.659549 },
                { 0.483210, 0.008460, 0.659095 },
                { 0.489055, 0.010127, 0.658534 },
                { 0.494877, 0.011990, 0.657865 },
                { 0.500678, 0.014055, 0.657088 },
                { 0.506454, 0.016333, 0.656202 },
                { 0.512206, 0.018833, 0.655209 },
                { 0.517933, 0.021563, 0.654109 },
                { 0.523633, 0.024532, 0.652901 },
                { 0.529306, 0.027747, 0.651586 },
                { 0.534952, 0.031217, 0.650165 },
                { 0.540570, 0.034950, 0.648640 },
                { 0.546157, 0.038954, 0.647010 },
                { 0.551715, 0.043136, 0.645277 },
                { 0.557243, 0.047331, 0.643443 },
                { 0.562738, 0.051545, 0.641509 },
                { 0.568201, 0.055778, 0.639477 },
                { 0.573632, 0.060028, 0.637349 },
                { 0.579029, 0.064296, 0.635126 },
                { 0.584391, 0.068579, 0.632812 },
                { 0.589719, 0.072878, 0.630408 },
                { 0.595011, 0.077190, 0.627917 },
                { 0.600266, 0.081516, 0.625342 },
                { 0.605485, 0.085854, 0.622686 },
                { 0.610667, 0.090204, 0.619951 },
                { 0.615812, 0.094564, 0.617140 },
                { 0.620919, 0.098934, 0.614257 },
                { 0.625987, 0.103312, 0.611305 },
                { 0.631017, 0.107699, 0.608287 },
                { 0.636008, 0.112092, 0.605205 },
                { 0.640959, 0.116492, 0.602065 },
                { 0.645872, 0.120898, 0.598867 },
                { 0.650746, 0.125309, 0.595617 },
                { 0.655580, 0.129725, 0.592317 },
                { 0.660374, 0.134144, 0.588971 },
                { 0.665129, 0.138566, 0.585582 },
                { 0.669845, 0.142992, 0.582154 },
                { 0.674522, 0.147419, 0.578688 },
                { 0.679160, 0.151848, 0.575189 },
                { 0.683758, 0.156278, 0.571660 },
                { 0.688318, 0.160709, 0.568103 },
                { 0.692840, 0.165141, 0.564522 },
                { 0.697324, 0.169573, 0.560919 },
                { 0.701769, 0.174005, 0.557296 },
                { 0.706178, 0.178437, 0.553657 },
                { 0.710549, 0.182868, 0.550004 },
                { 0.714883, 0.187299, 0.546338 },
                { 0.719181, 0.191729, 0.542663 },
                { 0.723444, 0.196158, 0.538981 },
                { 0.727670, 0.200586, 0.535293 },
                { 0.731862, 0.205013, 0.531601 },
                { 0.736019, 0.209439, 0.527908 },
                { 0.740143, 0.213864, 0.524216 },
                { 0.744232, 0.218288, 0.520524 },
                { 0.748289, 0.222711, 0.516834 },
                { 0.752312, 0.227133, 0.513149 },
                { 0.756304, 0.231555, 0.509468 },
                { 0.760264, 0.235976, 0.505794 },
                { 0.764193, 0.240396, 0.502126 },
                { 0.768090, 0.244817, 0.498465 },
                { 0.771958, 0.249237, 0.494813 },
                { 0.775796, 0.253658, 0.491171 },
                { 0.779604, 0.258078, 0.487539 },
                { 0.783383, 0.262500, 0.483918 },
                { 0.787133, 0.266922, 0.480307 },
                { 0.790855, 0.271345, 0.476706 },
                { 0.794549, 0.275770, 0.473117 },
                { 0.798216, 0.280197, 0.469538 },
                { 0.801855, 0.284626, 0.465971 },
                { 0.805467, 0.289057, 0.462415 },
                { 0.809052, 0.293491, 0.458870 },
                { 0.812612, 0.297928, 0.455338 },
                { 0.816144, 0.302368, 0.451816 },
                { 0.819651, 0.306812, 0.448306 },
                { 0.823132, 0.311261, 0.444806 },
                { 0.826588, 0.315714, 0.441316 },
                { 0.830018, 0.320172, 0.437836 },
                { 0.833422, 0.324635, 0.434366 },
                { 0.836801, 0.329105, 0.430905 },
                { 0.840155, 0.333580, 0.427455 },
                { 0.843484, 0.338062, 0.424013 },
                { 0.846788, 0.342551, 0.420579 },
                { 0.850066, 0.347048, 0.417153 },
                { 0.853319, 0.351553, 0.413734 },
                { 0.856547, 0.356066, 0.410322 },
                { 0.859750, 0.360588, 0.406917 },
                { 0.862927, 0.365119, 0.403519 },
                { 0.866078, 0.369660, 0.400126 },
                { 0.869203, 0.374212, 0.396738 },
                { 0.872303, 0.378774, 0.393355 },
                { 0.875376, 0.383347, 0.389976 },
                { 0.878423, 0.387932, 0.386600 },
                { 0.881443, 0.392529, 0.383229 },
                { 0.884436, 0.397139, 0.379860 },
                { 0.887402, 0.401762, 0.376494 },
                { 0.890340, 0.406398, 0.373130 },
                { 0.893250, 0.411048, 0.369768 },
                { 0.896131, 0.415712, 0.366407 },
                { 0.898984, 0.420392, 0.363047 },
                { 0.901807, 0.425087, 0.359688 },
                { 0.904601, 0.429797, 0.356329 },
                { 0.907365, 0.434524, 0.352970 },
                { 0.910098, 0.439268, 0.349610 },
                { 0.912800, 0.444029, 0.346251 },
                { 0.915471, 0.448807, 0.342890 },
                { 0.918109, 0.453603, 0.339529 },
                { 0.920714, 0.458417, 0.336166 },
                { 0.923287, 0.463251, 0.332801 },
                { 0.925825, 0.468103, 0.329435 },
                { 0.928329, 0.472975, 0.326067 },
                { 0.930798, 0.477867, 0.322697 },
                { 0.933232, 0.482780, 0.319325 },
                { 0.935630, 0.487712, 0.315952 },
                { 0.937990, 0.492667, 0.312575 },
                { 0.940313, 0.497642, 0.309197 },
                { 0.942598, 0.502639, 0.305816 },
                { 0.944844, 0.507658, 0.302433 },
                { 0.947051, 0.512699, 0.299049 },
                { 0.949217, 0.517763, 0.295662 },
                { 0.951344, 0.522850, 0.292275 },
                { 0.953428, 0.527960, 0.288883 },
                { 0.955470, 0.533093, 0.285490 },
                { 0.957469, 0.538250, 0.282096 },
                { 0.959424, 0.543431, 0.278701 },
                { 0.961336, 0.548636, 0.275305 },
                { 0.963203, 0.553865, 0.271909 },
                { 0.965024, 0.559118, 0.268513 },
                { 0.966798, 0.564396, 0.265118 },
                { 0.968526, 0.569700, 0.261721 },
                { 0.970205, 0.575028, 0.258325 },
                { 0.971835, 0.580382, 0.254931 },
                { 0.973416, 0.585761, 0.251540 },
                { 0.974947, 0.591165, 0.248151 },
                { 0.976428, 0.596595, 0.244767 },
                { 0.977856, 0.602051, 0.241387 },
                { 0.979233, 0.607532, 0.238013 },
                { 0.980556, 0.613039, 0.234646 },
                { 0.981826, 0.618572, 0.231287 },
                { 0.983041, 0.624131, 0.227937 },
                { 0.984199, 0.629718, 0.224595 },
                { 0.985301, 0.635330, 0.221265 },
                { 0.986345, 0.640969, 0.217948 },
                { 0.987332, 0.646633, 0.214648 },
                { 0.988260, 0.652325, 0.211364 },
                { 0.989128, 0.658043, 0.208100 },
                { 0.989935, 0.663787, 0.204859 },
                { 0.990681, 0.669558, 0.201642 },
                { 0.991365, 0.675355, 0.198453 },
                { 0.991985, 0.681179, 0.195295 },
                { 0.992541, 0.687030, 0.192170 },
                { 0.993032, 0.692907, 0.189084 },
                { 0.993456, 0.698810, 0.186041 },
                { 0.993814, 0.704741, 0.183043 },
                { 0.994103, 0.710698, 0.180097 },
                { 0.994324, 0.716681, 0.177208 },
                { 0.994474, 0.722691, 0.174381 },
                { 0.994553, 0.728728, 0.171622 },
                { 0.994561, 0.734791, 0.168938 },
                { 0.994495, 0.740880, 0.166335 },
                { 0.994355, 0.746995, 0.163821 },
                { 0.994141, 0.753137, 0.161404 },
                { 0.993851, 0.759304, 0.159092 },
                { 0.993482, 0.765499, 0.156891 },
                { 0.993033, 0.771720, 0.154808 },
                { 0.992505, 0.777967, 0.152855 },
                { 0.991897, 0.784239, 0.151042 },
                { 0.991209, 0.790537, 0.149377 },
                { 0.990439, 0.796859, 0.147870 },
                { 0.989587, 0.803205, 0.146529 },
                { 0.988648, 0.809579, 0.145357 },
                { 0.987621, 0.815978, 0.144363 },
                { 0.986509, 0.822401, 0.143557 },
                { 0.985314, 0.828846, 0.142945 },
                { 0.984031, 0.835315, 0.142528 },
                { 0.982653, 0.841812, 0.142303 },
                { 0.981190, 0.848329, 0.142279 },
                { 0.979644, 0.854866, 0.142453 },
                { 0.977995, 0.861432, 0.142808 },
                { 0.976265, 0.868016, 0.143351 },
                { 0.974443, 0.874622, 0.144061 },
                { 0.972530, 0.881250, 0.144923 },
                { 0.970533, 0.887896, 0.145919 },
                { 0.968443, 0.894564, 0.147014 },
                { 0.966271, 0.901249, 0.148180 },
                { 0.964021, 0.907950, 0.149370 },
                { 0.961681, 0.914672, 0.150520 },
                { 0.959276, 0.921407, 0.151566 },
                { 0.956808, 0.928152, 0.152409 },
                { 0.954287, 0.934908, 0.152921 },
                { 0.951726, 0.941671, 0.152925 },
                { 0.949151, 0.948435, 0.152178 },
                { 0.946602, 0.955190, 0.150328 },
                { 0.944152, 0.961916, 0.146861 },
                { 0.941896, 0.968590, 0.140956 },
                { 0.940015, 0.975158, 0.131326 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetViridisColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.267004, 0.004874, 0.329415 },
                { 0.268510, 0.009605, 0.335427 },
                { 0.269944, 0.014625, 0.341379 },
                { 0.271305, 0.019942, 0.347269 },
                { 0.272594, 0.025563, 0.353093 },
                { 0.273809, 0.031497, 0.358853 },
                { 0.274952, 0.037752, 0.364543 },
                { 0.276022, 0.044167, 0.370164 },
                { 0.277018, 0.050344, 0.375715 },
                { 0.277941, 0.056324, 0.381191 },
                { 0.278791, 0.062145, 0.386592 },
                { 0.279566, 0.067836, 0.391917 },
                { 0.280267, 0.073417, 0.397163 },
                { 0.280894, 0.078907, 0.402329 },
                { 0.281446, 0.084320, 0.407414 },
                { 0.281924, 0.089666, 0.412415 },
                { 0.282327, 0.094955, 0.417331 },
                { 0.282656, 0.100196, 0.422160 },
                { 0.282910, 0.105393, 0.426902 },
                { 0.283091, 0.110553, 0.431554 },
                { 0.283197, 0.115680, 0.436115 },
                { 0.283229, 0.120777, 0.440584 },
                { 0.283187, 0.125848, 0.444960 },
                { 0.283072, 0.130895, 0.449241 },
                { 0.282884, 0.135920, 0.453427 },
                { 0.282623, 0.140926, 0.457517 },
                { 0.282290, 0.145912, 0.461510 },
                { 0.281887, 0.150881, 0.465405 },
                { 0.281412, 0.155834, 0.469201 },
                { 0.280868, 0.160771, 0.472899 },
                { 0.280255, 0.165693, 0.476498 },
                { 0.279574, 0.170599, 0.479997 },
                { 0.278826, 0.175490, 0.483397 },
                { 0.278012, 0.180367, 0.486697 },
                { 0.277134, 0.185228, 0.489898 },
                { 0.276194, 0.190074, 0.493001 },
                { 0.275191, 0.194905, 0.496005 },
                { 0.274128, 0.199721, 0.498911 },
                { 0.273006, 0.204520, 0.501721 },
                { 0.271828, 0.209303, 0.504434 },
                { 0.270595, 0.214069, 0.507052 },
                { 0.269308, 0.218818, 0.509577 },
                { 0.267968, 0.223549, 0.512008 },
                { 0.266580, 0.228262, 0.514349 },
                { 0.265145, 0.232956, 0.516599 },
                { 0.263663, 0.237631, 0.518762 },
                { 0.262138, 0.242286, 0.520837 },
                { 0.260571, 0.246922, 0.522828 },
                { 0.258965, 0.251537, 0.524736 },
                { 0.257322, 0.256130, 0.526563 },
                { 0.255645, 0.260703, 0.528312 },
                { 0.253935, 0.265254, 0.529983 },
                { 0.252194, 0.269783, 0.531579 },
                { 0.250425, 0.274290, 0.533103 },
                { 0.248629, 0.278775, 0.534556 },
                { 0.246811, 0.283237, 0.535941 },
                { 0.244972, 0.287675, 0.537260 },
                { 0.243113, 0.292092, 0.538516 },
                { 0.241237, 0.296485, 0.539709 },
                { 0.239346, 0.300855, 0.540844 },
                { 0.237441, 0.305202, 0.541921 },
                { 0.235526, 0.309527, 0.542944 },
                { 0.233603, 0.313828, 0.543914 },
                { 0.231674, 0.318106, 0.544834 },
                { 0.229739, 0.322361, 0.545706 },
                { 0.227802, 0.326594, 0.546532 },
                { 0.225863, 0.330805, 0.547314 },
                { 0.223925, 0.334994, 0.548053 },
                { 0.221989, 0.339161, 0.548752 },
                { 0.220057, 0.343307, 0.549413 },
                { 0.218130, 0.347432, 0.550038 },
                { 0.216210, 0.351535, 0.550627 },
                { 0.214298, 0.355619, 0.551184 },
                { 0.212395, 0.359683, 0.551710 },
                { 0.210503, 0.363727, 0.552206 },
                { 0.208623, 0.367752, 0.552675 },
                { 0.206756, 0.371758, 0.553117 },
                { 0.204903, 0.375746, 0.553533 },
                { 0.203063, 0.379716, 0.553925 },
                { 0.201239, 0.383670, 0.554294 },
                { 0.199430, 0.387607, 0.554642 },
                { 0.197636, 0.391528, 0.554969 },
                { 0.195860, 0.395433, 0.555276 },
                { 0.194100, 0.399323, 0.555565 },
                { 0.192357, 0.403199, 0.555836 },
                { 0.190631, 0.407061, 0.556089 },
                { 0.188923, 0.410910, 0.556326 },
                { 0.187231, 0.414746, 0.556547 },
                { 0.185556, 0.418570, 0.556753 },
                { 0.183898, 0.422383, 0.556944 },
                { 0.182256, 0.426184, 0.557120 },
                { 0.180629, 0.429975, 0.557282 },
                { 0.179019, 0.433756, 0.557430 },
                { 0.177423, 0.437527, 0.557565 },
                { 0.175841, 0.441290, 0.557685 },
                { 0.174274, 0.445044, 0.557792 },
                { 0.172719, 0.448791, 0.557885 },
                { 0.171176, 0.452530, 0.557965 },
                { 0.169646, 0.456262, 0.558030 },
                { 0.168126, 0.459988, 0.558082 },
                { 0.166617, 0.463708, 0.558119 },
                { 0.165117, 0.467423, 0.558141 },
                { 0.163625, 0.471133, 0.558148 },
                { 0.162142, 0.474838, 0.558140 },
                { 0.160665, 0.478540, 0.558115 },
                { 0.159194, 0.482237, 0.558073 },
                { 0.157729, 0.485932, 0.558013 },
                { 0.156270, 0.489624, 0.557936 },
                { 0.154815, 0.493313, 0.557840 },
                { 0.153364, 0.497000, 0.557724 },
                { 0.151918, 0.500685, 0.557587 },
                { 0.150476, 0.504369, 0.557430 },
                { 0.149039, 0.508051, 0.557250 },
                { 0.147607, 0.511733, 0.557049 },
                { 0.146180, 0.515413, 0.556823 },
                { 0.144759, 0.519093, 0.556572 },
                { 0.143343, 0.522773, 0.556295 },
                { 0.141935, 0.526453, 0.555991 },
                { 0.140536, 0.530132, 0.555659 },
                { 0.139147, 0.533812, 0.555298 },
                { 0.137770, 0.537492, 0.554906 },
                { 0.136408, 0.541173, 0.554483 },
                { 0.135066, 0.544853, 0.554029 },
                { 0.133743, 0.548535, 0.553541 },
                { 0.132444, 0.552216, 0.553018 },
                { 0.131172, 0.555899, 0.552459 },
                { 0.129933, 0.559582, 0.551864 },
                { 0.128729, 0.563265, 0.551229 },
                { 0.127568, 0.566949, 0.550556 },
                { 0.126453, 0.570633, 0.549841 },
                { 0.125394, 0.574318, 0.549086 },
                { 0.124395, 0.578002, 0.548287 },
                { 0.123463, 0.581687, 0.547445 },
                { 0.122606, 0.585371, 0.546557 },
                { 0.121831, 0.589055, 0.545623 },
                { 0.121148, 0.592739, 0.544641 },
                { 0.120565, 0.596422, 0.543611 },
                { 0.120092, 0.600104, 0.542530 },
                { 0.119738, 0.603785, 0.541400 },
                { 0.119512, 0.607464, 0.540218 },
                { 0.119423, 0.611141, 0.538982 },
                { 0.119483, 0.614817, 0.537692 },
                { 0.119699, 0.618490, 0.536347 },
                { 0.120081, 0.622161, 0.534946 },
                { 0.120638, 0.625828, 0.533488 },
                { 0.121380, 0.629492, 0.531973 },
                { 0.122312, 0.633153, 0.530398 },
                { 0.123444, 0.636809, 0.528763 },
                { 0.124780, 0.640461, 0.527068 },
                { 0.126326, 0.644107, 0.525311 },
                { 0.128087, 0.647749, 0.523491 },
                { 0.130067, 0.651384, 0.521608 },
                { 0.132268, 0.655014, 0.519661 },
                { 0.134692, 0.658636, 0.517649 },
                { 0.137339, 0.662252, 0.515571 },
                { 0.140210, 0.665859, 0.513427 },
                { 0.143303, 0.669459, 0.511215 },
                { 0.146616, 0.673050, 0.508936 },
                { 0.150148, 0.676631, 0.506589 },
                { 0.153894, 0.680203, 0.504172 },
                { 0.157851, 0.683765, 0.501686 },
                { 0.162016, 0.687316, 0.499129 },
                { 0.166383, 0.690856, 0.496502 },
                { 0.170948, 0.694384, 0.493803 },
                { 0.175707, 0.697900, 0.491033 },
                { 0.180653, 0.701402, 0.488189 },
                { 0.185783, 0.704891, 0.485273 },
                { 0.191090, 0.708366, 0.482284 },
                { 0.196571, 0.711827, 0.479221 },
                { 0.202219, 0.715272, 0.476084 },
                { 0.208030, 0.718701, 0.472873 },
                { 0.214000, 0.722114, 0.469588 },
                { 0.220124, 0.725509, 0.466226 },
                { 0.226397, 0.728888, 0.462789 },
                { 0.232815, 0.732247, 0.459277 },
                { 0.239374, 0.735588, 0.455688 },
                { 0.246070, 0.738910, 0.452024 },
                { 0.252899, 0.742211, 0.448284 },
                { 0.259857, 0.745492, 0.444467 },
                { 0.266941, 0.748751, 0.440573 },
                { 0.274149, 0.751988, 0.436601 },
                { 0.281477, 0.755203, 0.432552 },
                { 0.288921, 0.758394, 0.428426 },
                { 0.296479, 0.761561, 0.424223 },
                { 0.304148, 0.764704, 0.419943 },
                { 0.311925, 0.767822, 0.415586 },
                { 0.319809, 0.770914, 0.411152 },
                { 0.327796, 0.773980, 0.406640 },
                { 0.335885, 0.777018, 0.402049 },
                { 0.344074, 0.780029, 0.397381 },
                { 0.352360, 0.783011, 0.392636 },
                { 0.360741, 0.785964, 0.387814 },
                { 0.369214, 0.788888, 0.382914 },
                { 0.377779, 0.791781, 0.377939 },
                { 0.386433, 0.794644, 0.372886 },
                { 0.395174, 0.797475, 0.367757 },
                { 0.404001, 0.800275, 0.362552 },
                { 0.412913, 0.803041, 0.357269 },
                { 0.421908, 0.805774, 0.351910 },
                { 0.430983, 0.808473, 0.346476 },
                { 0.440137, 0.811138, 0.340967 },
                { 0.449368, 0.813768, 0.335384 },
                { 0.458674, 0.816363, 0.329727 },
                { 0.468053, 0.818921, 0.323998 },
                { 0.477504, 0.821444, 0.318195 },
                { 0.487026, 0.823929, 0.312321 },
                { 0.496615, 0.826376, 0.306377 },
                { 0.506271, 0.828786, 0.300362 },
                { 0.515992, 0.831158, 0.294279 },
                { 0.525776, 0.833491, 0.288127 },
                { 0.535621, 0.835785, 0.281908 },
                { 0.545524, 0.838039, 0.275626 },
                { 0.555484, 0.840254, 0.269281 },
                { 0.565498, 0.842430, 0.262877 },
                { 0.575563, 0.844566, 0.256415 },
                { 0.585678, 0.846661, 0.249897 },
                { 0.595839, 0.848717, 0.243329 },
                { 0.606045, 0.850733, 0.236712 },
                { 0.616293, 0.852709, 0.230052 },
                { 0.626579, 0.854645, 0.223353 },
                { 0.636902, 0.856542, 0.216620 },
                { 0.647257, 0.858400, 0.209861 },
                { 0.657642, 0.860219, 0.203082 },
                { 0.668054, 0.861999, 0.196293 },
                { 0.678489, 0.863742, 0.189503 },
                { 0.688944, 0.865448, 0.182725 },
                { 0.699415, 0.867117, 0.175971 },
                { 0.709898, 0.868751, 0.169257 },
                { 0.720391, 0.870350, 0.162603 },
                { 0.730889, 0.871916, 0.156029 },
                { 0.741388, 0.873449, 0.149561 },
                { 0.751884, 0.874951, 0.143228 },
                { 0.762373, 0.876424, 0.137064 },
                { 0.772852, 0.877868, 0.131109 },
                { 0.783315, 0.879285, 0.125405 },
                { 0.793760, 0.880678, 0.120005 },
                { 0.804182, 0.882046, 0.114965 },
                { 0.814576, 0.883393, 0.110347 },
                { 0.824940, 0.884720, 0.106217 },
                { 0.835270, 0.886029, 0.102646 },
                { 0.845561, 0.887322, 0.099702 },
                { 0.855810, 0.888601, 0.097452 },
                { 0.866013, 0.889868, 0.095953 },
                { 0.876168, 0.891125, 0.095250 },
                { 0.886271, 0.892374, 0.095374 },
                { 0.896320, 0.893616, 0.096335 },
                { 0.906311, 0.894855, 0.098125 },
                { 0.916242, 0.896091, 0.100717 },
                { 0.926106, 0.897330, 0.104071 },
                { 0.935904, 0.898570, 0.108131 },
                { 0.945636, 0.899815, 0.112838 },
                { 0.955300, 0.901065, 0.118128 },
                { 0.964894, 0.902323, 0.123941 },
                { 0.974417, 0.903590, 0.130215 },
                { 0.983868, 0.904867, 0.136897 },
                { 0.993248, 0.906157, 0.143936 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetCividisColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.0000, 0.1262, 0.3015 },
                { 0.0000, 0.1292, 0.3077 },
                { 0.0000, 0.1321, 0.3142 },
                { 0.0000, 0.1350, 0.3205 },
                { 0.0000, 0.1379, 0.3269 },
                { 0.0000, 0.1408, 0.3334 },
                { 0.0000, 0.1437, 0.3400 },
                { 0.0000, 0.1465, 0.3467 },
                { 0.0000, 0.1492, 0.3537 },
                { 0.0000, 0.1519, 0.3606 },
                { 0.0000, 0.1546, 0.3676 },
                { 0.0000, 0.1574, 0.3746 },
                { 0.0000, 0.1601, 0.3817 },
                { 0.0000, 0.1629, 0.3888 },
                { 0.0000, 0.1657, 0.3960 },
                { 0.0000, 0.1685, 0.4031 },
                { 0.0000, 0.1714, 0.4102 },
                { 0.0000, 0.1743, 0.4172 },
                { 0.0000, 0.1773, 0.4241 },
                { 0.0000, 0.1798, 0.4307 },
                { 0.0000, 0.1817, 0.4347 },
                { 0.0000, 0.1834, 0.4363 },
                { 0.0000, 0.1852, 0.4368 },
                { 0.0000, 0.1872, 0.4368 },
                { 0.0000, 0.1901, 0.4365 },
                { 0.0000, 0.1930, 0.4361 },
                { 0.0000, 0.1958, 0.4356 },
                { 0.0000, 0.1987, 0.4349 },
                { 0.0000, 0.2015, 0.4343 },
                { 0.0000, 0.2044, 0.4336 },
                { 0.0000, 0.2073, 0.4329 },
                { 0.0055, 0.2101, 0.4322 },
                { 0.0236, 0.2130, 0.4314 },
                { 0.0416, 0.2158, 0.4308 },
                { 0.0576, 0.2187, 0.4301 },
                { 0.0710, 0.2215, 0.4293 },
                { 0.0827, 0.2244, 0.4287 },
                { 0.0932, 0.2272, 0.4280 },
                { 0.1030, 0.2300, 0.4274 },
                { 0.1120, 0.2329, 0.4268 },
                { 0.1204, 0.2357, 0.4262 },
                { 0.1283, 0.2385, 0.4256 },
                { 0.1359, 0.2414, 0.4251 },
                { 0.1431, 0.2442, 0.4245 },
                { 0.1500, 0.2470, 0.4241 },
                { 0.1566, 0.2498, 0.4236 },
                { 0.1630, 0.2526, 0.4232 },
                { 0.1692, 0.2555, 0.4228 },
                { 0.1752, 0.2583, 0.4224 },
                { 0.1811, 0.2611, 0.4220 },
                { 0.1868, 0.2639, 0.4217 },
                { 0.1923, 0.2667, 0.4214 },
                { 0.1977, 0.2695, 0.4212 },
                { 0.2030, 0.2723, 0.4209 },
                { 0.2082, 0.2751, 0.4207 },
                { 0.2133, 0.2780, 0.4205 },
                { 0.2183, 0.2808, 0.4204 },
                { 0.2232, 0.2836, 0.4203 },
                { 0.2281, 0.2864, 0.4202 },
                { 0.2328, 0.2892, 0.4201 },
                { 0.2375, 0.2920, 0.4200 },
                { 0.2421, 0.2948, 0.4200 },
                { 0.2466, 0.2976, 0.4200 },
                { 0.2511, 0.3004, 0.4201 },
                { 0.2556, 0.3032, 0.4201 },
                { 0.2599, 0.3060, 0.4202 },
                { 0.2643, 0.3088, 0.4203 },
                { 0.2686, 0.3116, 0.4205 },
                { 0.2728, 0.3144, 0.4206 },
                { 0.2770, 0.3172, 0.4208 },
                { 0.2811, 0.3200, 0.4210 },
                { 0.2853, 0.3228, 0.4212 },
                { 0.2894, 0.3256, 0.4215 },
                { 0.2934, 0.3284, 0.4218 },
                { 0.2974, 0.3312, 0.4221 },
                { 0.3014, 0.3340, 0.4224 },
                { 0.3054, 0.3368, 0.4227 },
                { 0.3093, 0.3396, 0.4231 },
                { 0.3132, 0.3424, 0.4236 },
                { 0.3170, 0.3453, 0.4240 },
                { 0.3209, 0.3481, 0.4244 },
                { 0.3247, 0.3509, 0.4249 },
                { 0.3285, 0.3537, 0.4254 },
                { 0.3323, 0.3565, 0.4259 },
                { 0.3361, 0.3593, 0.4264 },
                { 0.3398, 0.3622, 0.4270 },
                { 0.3435, 0.3650, 0.4276 },
                { 0.3472, 0.3678, 0.4282 },
                { 0.3509, 0.3706, 0.4288 },
                { 0.3546, 0.3734, 0.4294 },
                { 0.3582, 0.3763, 0.4302 },
                { 0.3619, 0.3791, 0.4308 },
                { 0.3655, 0.3819, 0.4316 },
                { 0.3691, 0.3848, 0.4322 },
                { 0.3727, 0.3876, 0.4331 },
                { 0.3763, 0.3904, 0.4338 },
                { 0.3798, 0.3933, 0.4346 },
                { 0.3834, 0.3961, 0.4355 },
                { 0.3869, 0.3990, 0.4364 },
                { 0.3905, 0.4018, 0.4372 },
                { 0.3940, 0.4047, 0.4381 },
                { 0.3975, 0.4075, 0.4390 },
                { 0.4010, 0.4104, 0.4400 },
                { 0.4045, 0.4132, 0.4409 },
                { 0.4080, 0.4161, 0.4419 },
                { 0.4114, 0.4189, 0.4430 },
                { 0.4149, 0.4218, 0.4440 },
                { 0.4183, 0.4247, 0.4450 },
                { 0.4218, 0.4275, 0.4462 },
                { 0.4252, 0.4304, 0.4473 },
                { 0.4286, 0.4333, 0.4485 },
                { 0.4320, 0.4362, 0.4496 },
                { 0.4354, 0.4390, 0.4508 },
                { 0.4388, 0.4419, 0.4521 },
                { 0.4422, 0.4448, 0.4534 },
                { 0.4456, 0.4477, 0.4547 },
                { 0.4489, 0.4506, 0.4561 },
                { 0.4523, 0.4535, 0.4575 },
                { 0.4556, 0.4564, 0.4589 },
                { 0.4589, 0.4593, 0.4604 },
                { 0.4622, 0.4622, 0.4620 },
                { 0.4656, 0.4651, 0.4635 },
                { 0.4689, 0.4680, 0.4650 },
                { 0.4722, 0.4709, 0.4665 },
                { 0.4756, 0.4738, 0.4679 },
                { 0.4790, 0.4767, 0.4691 },
                { 0.4825, 0.4797, 0.4701 },
                { 0.4861, 0.4826, 0.4707 },
                { 0.4897, 0.4856, 0.4714 },
                { 0.4934, 0.4886, 0.4719 },
                { 0.4971, 0.4915, 0.4723 },
                { 0.5008, 0.4945, 0.4727 },
                { 0.5045, 0.4975, 0.4730 },
                { 0.5083, 0.5005, 0.4732 },
                { 0.5121, 0.5035, 0.4734 },
                { 0.5158, 0.5065, 0.4736 },
                { 0.5196, 0.5095, 0.4737 },
                { 0.5234, 0.5125, 0.4738 },
                { 0.5272, 0.5155, 0.4739 },
                { 0.5310, 0.5186, 0.4739 },
                { 0.5349, 0.5216, 0.4738 },
                { 0.5387, 0.5246, 0.4739 },
                { 0.5425, 0.5277, 0.4738 },
                { 0.5464, 0.5307, 0.4736 },
                { 0.5502, 0.5338, 0.4735 },
                { 0.5541, 0.5368, 0.4733 },
                { 0.5579, 0.5399, 0.4732 },
                { 0.5618, 0.5430, 0.4729 },
                { 0.5657, 0.5461, 0.4727 },
                { 0.5696, 0.5491, 0.4723 },
                { 0.5735, 0.5522, 0.4720 },
                { 0.5774, 0.5553, 0.4717 },
                { 0.5813, 0.5584, 0.4714 },
                { 0.5852, 0.5615, 0.4709 },
                { 0.5892, 0.5646, 0.4705 },
                { 0.5931, 0.5678, 0.4701 },
                { 0.5970, 0.5709, 0.4696 },
                { 0.6010, 0.5740, 0.4691 },
                { 0.6050, 0.5772, 0.4685 },
                { 0.6089, 0.5803, 0.4680 },
                { 0.6129, 0.5835, 0.4673 },
                { 0.6168, 0.5866, 0.4668 },
                { 0.6208, 0.5898, 0.4662 },
                { 0.6248, 0.5929, 0.4655 },
                { 0.6288, 0.5961, 0.4649 },
                { 0.6328, 0.5993, 0.4641 },
                { 0.6368, 0.6025, 0.4632 },
                { 0.6408, 0.6057, 0.4625 },
                { 0.6449, 0.6089, 0.4617 },
                { 0.6489, 0.6121, 0.4609 },
                { 0.6529, 0.6153, 0.4600 },
                { 0.6570, 0.6185, 0.4591 },
                { 0.6610, 0.6217, 0.4583 },
                { 0.6651, 0.6250, 0.4573 },
                { 0.6691, 0.6282, 0.4562 },
                { 0.6732, 0.6315, 0.4553 },
                { 0.6773, 0.6347, 0.4543 },
                { 0.6813, 0.6380, 0.4532 },
                { 0.6854, 0.6412, 0.4521 },
                { 0.6895, 0.6445, 0.4511 },
                { 0.6936, 0.6478, 0.4499 },
                { 0.6977, 0.6511, 0.4487 },
                { 0.7018, 0.6544, 0.4475 },
                { 0.7060, 0.6577, 0.4463 },
                { 0.7101, 0.6610, 0.4450 },
                { 0.7142, 0.6643, 0.4437 },
                { 0.7184, 0.6676, 0.4424 },
                { 0.7225, 0.6710, 0.4409 },
                { 0.7267, 0.6743, 0.4396 },
                { 0.7308, 0.6776, 0.4382 },
                { 0.7350, 0.6810, 0.4368 },
                { 0.7392, 0.6844, 0.4352 },
                { 0.7434, 0.6877, 0.4338 },
                { 0.7476, 0.6911, 0.4322 },
                { 0.7518, 0.6945, 0.4307 },
                { 0.7560, 0.6979, 0.4290 },
                { 0.7602, 0.7013, 0.4273 },
                { 0.7644, 0.7047, 0.4258 },
                { 0.7686, 0.7081, 0.4241 },
                { 0.7729, 0.7115, 0.4223 },
                { 0.7771, 0.7150, 0.4205 },
                { 0.7814, 0.7184, 0.4188 },
                { 0.7856, 0.7218, 0.4168 },
                { 0.7899, 0.7253, 0.4150 },
                { 0.7942, 0.7288, 0.4129 },
                { 0.7985, 0.7322, 0.4111 },
                { 0.8027, 0.7357, 0.4090 },
                { 0.8070, 0.7392, 0.4070 },
                { 0.8114, 0.7427, 0.4049 },
                { 0.8157, 0.7462, 0.4028 },
                { 0.8200, 0.7497, 0.4007 },
                { 0.8243, 0.7532, 0.3984 },
                { 0.8287, 0.7568, 0.3961 },
                { 0.8330, 0.7603, 0.3938 },
                { 0.8374, 0.7639, 0.3915 },
                { 0.8417, 0.7674, 0.3892 },
                { 0.8461, 0.7710, 0.3869 },
                { 0.8505, 0.7745, 0.3843 },
                { 0.8548, 0.7781, 0.3818 },
                { 0.8592, 0.7817, 0.3793 },
                { 0.8636, 0.7853, 0.3766 },
                { 0.8681, 0.7889, 0.3739 },
                { 0.8725, 0.7926, 0.3712 },
                { 0.8769, 0.7962, 0.3684 },
                { 0.8813, 0.7998, 0.3657 },
                { 0.8858, 0.8035, 0.3627 },
                { 0.8902, 0.8071, 0.3599 },
                { 0.8947, 0.8108, 0.3569 },
                { 0.8992, 0.8145, 0.3538 },
                { 0.9037, 0.8182, 0.3507 },
                { 0.9082, 0.8219, 0.3474 },
                { 0.9127, 0.8256, 0.3442 },
                { 0.9172, 0.8293, 0.3409 },
                { 0.9217, 0.8330, 0.3374 },
                { 0.9262, 0.8367, 0.3340 },
                { 0.9308, 0.8405, 0.3306 },
                { 0.9353, 0.8442, 0.3268 },
                { 0.9399, 0.8480, 0.3232 },
                { 0.9444, 0.8518, 0.3195 },
                { 0.9490, 0.8556, 0.3155 },
                { 0.9536, 0.8593, 0.3116 },
                { 0.9582, 0.8632, 0.3076 },
                { 0.9628, 0.8670, 0.3034 },
                { 0.9674, 0.8708, 0.2990 },
                { 0.9721, 0.8746, 0.2947 },
                { 0.9767, 0.8785, 0.2901 },
                { 0.9814, 0.8823, 0.2856 },
                { 0.9860, 0.8862, 0.2807 },
                { 0.9907, 0.8901, 0.2759 },
                { 0.9954, 0.8940, 0.2708 },
                { 1.0000, 0.8979, 0.2655 },
                { 1.0000, 0.9018, 0.2600 },
                { 1.0000, 0.9057, 0.2593 },
                { 1.0000, 0.9094, 0.2634 },
                { 1.0000, 0.9131, 0.2680 },
                { 1.0000, 0.9169, 0.2731 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetGithubColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.933333, 0.933333, 0.933333 },
                { 0.776470, 0.894117, 0.545098 },
                { 0.482352, 0.788235, 0.435294 },
                { 0.137254, 0.603921, 0.231372 },
                { 0.098039, 0.380392, 0.152941 }
            };

        return internal::CalcLerp(x, data);
    }

    inline Color GetCubehelixColor(double x)
    {
        constexpr Color data[] =
            {
                { 0.000000, 0.000000, 0.000000 },
                { 0.006716, 0.002119, 0.005970 },
                { 0.013252, 0.004287, 0.012162 },
                { 0.019599, 0.006514, 0.018563 },
                { 0.025748, 0.008803, 0.025162 },
                { 0.031691, 0.011164, 0.031946 },
                { 0.037421, 0.013600, 0.038902 },
                { 0.042932, 0.016118, 0.046016 },
                { 0.048217, 0.018724, 0.053275 },
                { 0.053271, 0.021423, 0.060666 },
                { 0.058090, 0.024220, 0.068173 },
                { 0.062670, 0.027119, 0.075781 },
                { 0.067008, 0.030126, 0.083478 },
                { 0.071101, 0.033243, 0.091246 },
                { 0.074947, 0.036475, 0.099072 },
                { 0.078546, 0.039824, 0.106939 },
                { 0.081898, 0.043295, 0.114834 },
                { 0.085002, 0.046889, 0.122740 },
                { 0.087860, 0.050609, 0.130643 },
                { 0.090474, 0.054457, 0.138527 },
                { 0.092845, 0.058434, 0.146378 },
                { 0.094978, 0.062542, 0.154180 },
                { 0.096875, 0.066781, 0.161918 },
                { 0.098542, 0.071152, 0.169579 },
                { 0.099984, 0.075655, 0.177147 },
                { 0.101205, 0.080290, 0.184609 },
                { 0.102212, 0.085055, 0.191951 },
                { 0.103013, 0.089951, 0.199159 },
                { 0.103615, 0.094975, 0.206221 },
                { 0.104025, 0.100126, 0.213124 },
                { 0.104252, 0.105403, 0.219855 },
                { 0.104305, 0.110801, 0.226402 },
                { 0.104194, 0.116320, 0.232755 },
                { 0.103929, 0.121956, 0.238903 },
                { 0.103519, 0.127705, 0.244834 },
                { 0.102976, 0.133564, 0.250541 },
                { 0.102310, 0.139529, 0.256012 },
                { 0.101534, 0.145596, 0.261240 },
                { 0.100659, 0.151759, 0.266217 },
                { 0.099697, 0.158016, 0.270935 },
                { 0.098661, 0.164359, 0.275388 },
                { 0.097563, 0.170785, 0.279569 },
                { 0.096415, 0.177287, 0.283474 },
                { 0.095232, 0.183860, 0.287097 },
                { 0.094026, 0.190498, 0.290434 },
                { 0.092810, 0.197194, 0.293483 },
                { 0.091597, 0.203943, 0.296240 },
                { 0.090402, 0.210739, 0.298703 },
                { 0.089237, 0.217573, 0.300873 },
                { 0.088115, 0.224441, 0.302747 },
                { 0.087051, 0.231334, 0.304327 },
                { 0.086056, 0.238247, 0.305612 },
                { 0.085146, 0.245171, 0.306606 },
                { 0.084331, 0.252101, 0.307310 },
                { 0.083626, 0.259028, 0.307728 },
                { 0.083043, 0.265946, 0.307863 },
                { 0.082594, 0.272848, 0.307720 },
                { 0.082291, 0.279726, 0.307304 },
                { 0.082148, 0.286573, 0.306621 },
                { 0.082174, 0.293383, 0.305677 },
                { 0.082381, 0.300147, 0.304480 },
                { 0.082780, 0.306860, 0.303037 },
                { 0.083383, 0.313514, 0.301356 },
                { 0.084198, 0.320102, 0.299448 },
                { 0.085235, 0.326618, 0.297320 },
                { 0.086504, 0.333055, 0.294984 },
                { 0.088014, 0.339406, 0.292449 },
                { 0.089772, 0.345666, 0.289728 },
                { 0.091787, 0.351829, 0.286831 },
                { 0.094066, 0.357887, 0.283771 },
                { 0.096615, 0.363836, 0.280560 },
                { 0.099441, 0.369671, 0.277211 },
                { 0.102549, 0.375385, 0.273736 },
                { 0.105944, 0.380974, 0.270151 },
                { 0.109630, 0.386433, 0.266468 },
                { 0.113611, 0.391757, 0.262703 },
                { 0.117891, 0.396943, 0.258868 },
                { 0.122472, 0.401985, 0.254979 },
                { 0.127356, 0.406881, 0.251051 },
                { 0.132543, 0.411627, 0.247099 },
                { 0.138035, 0.416220, 0.243137 },
                { 0.143832, 0.420656, 0.239182 },
                { 0.149933, 0.424934, 0.235247 },
                { 0.156336, 0.429052, 0.231350 },
                { 0.163040, 0.433007, 0.227504 },
                { 0.170042, 0.436798, 0.223726 },
                { 0.177339, 0.440423, 0.220029 },
                { 0.184927, 0.443882, 0.216431 },
                { 0.192802, 0.447175, 0.212944 },
                { 0.200958, 0.450301, 0.209585 },
                { 0.209391, 0.453260, 0.206367 },
                { 0.218092, 0.456053, 0.203306 },
                { 0.227057, 0.458680, 0.200415 },
                { 0.236277, 0.461144, 0.197707 },
                { 0.245744, 0.463444, 0.195197 },
                { 0.255451, 0.465584, 0.192898 },
                { 0.265388, 0.467565, 0.190822 },
                { 0.275545, 0.469391, 0.188982 },
                { 0.285913, 0.471062, 0.187389 },
                { 0.296481, 0.472584, 0.186055 },
                { 0.307239, 0.473959, 0.184992 },
                { 0.318175, 0.475191, 0.184208 },
                { 0.329277, 0.476285, 0.183716 },
                { 0.340534, 0.477243, 0.183523 },
                { 0.351934, 0.478072, 0.183638 },
                { 0.363463, 0.478776, 0.184071 },
                { 0.375109, 0.479360, 0.184829 },
                { 0.386858, 0.479829, 0.185918 },
                { 0.398697, 0.480190, 0.187345 },
                { 0.410613, 0.480448, 0.189115 },
                { 0.422591, 0.480609, 0.191235 },
                { 0.434618, 0.480679, 0.193708 },
                { 0.446680, 0.480665, 0.196538 },
                { 0.458762, 0.480574, 0.199728 },
                { 0.470850, 0.480412, 0.203280 },
                { 0.482930, 0.480186, 0.207197 },
                { 0.494987, 0.479903, 0.211478 },
                { 0.507008, 0.479572, 0.216124 },
                { 0.518978, 0.479198, 0.221136 },
                { 0.530883, 0.478789, 0.226510 },
                { 0.542708, 0.478353, 0.232247 },
                { 0.554441, 0.477898, 0.238342 },
                { 0.566067, 0.477430, 0.244794 },
                { 0.577573, 0.476958, 0.251597 },
                { 0.588945, 0.476490, 0.258747 },
                { 0.600171, 0.476032, 0.266239 },
                { 0.611237, 0.475592, 0.274067 },
                { 0.622132, 0.475178, 0.282223 },
                { 0.632842, 0.474798, 0.290702 },
                { 0.643357, 0.474459, 0.299495 },
                { 0.653665, 0.474168, 0.308593 },
                { 0.663755, 0.473933, 0.317987 },
                { 0.673616, 0.473761, 0.327668 },
                { 0.683239, 0.473658, 0.337626 },
                { 0.692613, 0.473632, 0.347849 },
                { 0.701729, 0.473690, 0.358327 },
                { 0.710579, 0.473838, 0.369047 },
                { 0.719155, 0.474083, 0.379998 },
                { 0.727448, 0.474430, 0.391167 },
                { 0.735453, 0.474886, 0.402541 },
                { 0.743162, 0.475457, 0.414106 },
                { 0.750569, 0.476148, 0.425849 },
                { 0.757669, 0.476964, 0.437755 },
                { 0.764458, 0.477911, 0.449811 },
                { 0.770932, 0.478994, 0.462001 },
                { 0.777086, 0.480216, 0.474310 },
                { 0.782918, 0.481583, 0.486725 },
                { 0.788426, 0.483098, 0.499228 },
                { 0.793609, 0.484765, 0.511805 },
                { 0.798465, 0.486587, 0.524441 },
                { 0.802993, 0.488567, 0.537119 },
                { 0.807196, 0.490708, 0.549824 },
                { 0.811072, 0.493013, 0.562540 },
                { 0.814625, 0.495483, 0.575253 },
                { 0.817855, 0.498121, 0.587945 },
                { 0.820767, 0.500927, 0.600602 },
                { 0.823364, 0.503903, 0.613208 },
                { 0.825649, 0.507050, 0.625748 },
                { 0.827628, 0.510368, 0.638207 },
                { 0.829305, 0.513857, 0.650570 },
                { 0.830688, 0.517516, 0.662822 },
                { 0.831781, 0.521346, 0.674949 },
                { 0.832593, 0.525345, 0.686938 },
                { 0.833130, 0.529511, 0.698773 },
                { 0.833402, 0.533844, 0.710443 },
                { 0.833416, 0.538342, 0.721933 },
                { 0.833181, 0.543001, 0.733232 },
                { 0.832708, 0.547820, 0.744327 },
                { 0.832006, 0.552795, 0.755206 },
                { 0.831086, 0.557924, 0.765859 },
                { 0.829958, 0.563202, 0.776274 },
                { 0.828633, 0.568627, 0.786443 },
                { 0.827124, 0.574193, 0.796354 },
                { 0.825442, 0.579897, 0.805999 },
                { 0.823599, 0.585733, 0.815370 },
                { 0.821608, 0.591698, 0.824459 },
                { 0.819482, 0.597785, 0.833258 },
                { 0.817233, 0.603990, 0.841761 },
                { 0.814875, 0.610307, 0.849963 },
                { 0.812421, 0.616730, 0.857858 },
                { 0.809884, 0.623252, 0.865441 },
                { 0.807278, 0.629869, 0.872709 },
                { 0.804617, 0.636573, 0.879658 },
                { 0.801914, 0.643359, 0.886286 },
                { 0.799183, 0.650218, 0.892592 },
                { 0.796438, 0.657146, 0.898574 },
                { 0.793692, 0.664134, 0.904231 },
                { 0.790959, 0.671176, 0.909565 },
                { 0.788253, 0.678264, 0.914576 },
                { 0.785586, 0.685392, 0.919267 },
                { 0.782973, 0.692553, 0.923639 },
                { 0.780425, 0.699738, 0.927695 },
                { 0.777957, 0.706942, 0.931441 },
                { 0.775579, 0.714157, 0.934879 },
                { 0.773305, 0.721375, 0.938016 },
                { 0.771147, 0.728589, 0.940857 },
                { 0.769116, 0.735793, 0.943409 },
                { 0.767224, 0.742979, 0.945678 },
                { 0.765481, 0.750140, 0.947673 },
                { 0.763898, 0.757269, 0.949402 },
                { 0.762485, 0.764360, 0.950874 },
                { 0.761251, 0.771405, 0.952098 },
                { 0.760207, 0.778399, 0.953084 },
                { 0.759360, 0.785335, 0.953843 },
                { 0.758718, 0.792207, 0.954386 },
                { 0.758290, 0.799008, 0.954724 },
                { 0.758082, 0.805734, 0.954869 },
                { 0.758101, 0.812378, 0.954833 },
                { 0.758353, 0.818934, 0.954629 },
                { 0.758842, 0.825399, 0.954270 },
                { 0.759575, 0.831767, 0.953769 },
                { 0.760554, 0.838033, 0.953140 },
                { 0.761784, 0.844192, 0.952397 },
                { 0.763267, 0.850242, 0.951554 },
                { 0.765006, 0.856178, 0.950625 },
                { 0.767001, 0.861997, 0.949624 },
                { 0.769255, 0.867695, 0.948567 },
                { 0.771766, 0.873270, 0.947467 },
                { 0.774535, 0.878718, 0.946340 },
                { 0.777561, 0.884039, 0.945201 },
                { 0.780841, 0.889230, 0.944063 },
                { 0.784374, 0.894289, 0.942942 },
                { 0.788156, 0.899216, 0.941853 },
                { 0.792184, 0.904010, 0.940809 },
                { 0.796453, 0.908669, 0.939825 },
                { 0.800958, 0.913194, 0.938916 },
                { 0.805694, 0.917586, 0.938095 },
                { 0.810654, 0.921845, 0.937376 },
                { 0.815832, 0.925971, 0.936772 },
                { 0.821221, 0.929967, 0.936297 },
                { 0.826811, 0.933833, 0.935962 },
                { 0.832595, 0.937572, 0.935781 },
                { 0.838565, 0.941187, 0.935766 },
                { 0.844709, 0.944679, 0.935927 },
                { 0.851018, 0.948053, 0.936275 },
                { 0.857482, 0.951311, 0.936822 },
                { 0.864090, 0.954457, 0.937578 },
                { 0.870830, 0.957495, 0.938550 },
                { 0.877690, 0.960430, 0.939749 },
                { 0.884659, 0.963266, 0.941183 },
                { 0.891723, 0.966009, 0.942858 },
                { 0.898871, 0.968662, 0.944783 },
                { 0.906088, 0.971233, 0.946962 },
                { 0.913362, 0.973726, 0.949402 },
                { 0.920679, 0.976147, 0.952108 },
                { 0.928026, 0.978504, 0.955083 },
                { 0.935387, 0.980802, 0.958331 },
                { 0.942750, 0.983048, 0.961854 },
                { 0.950101, 0.985249, 0.965654 },
                { 0.957424, 0.987412, 0.969733 },
                { 0.964706, 0.989543, 0.974090 },
                { 0.971932, 0.991652, 0.978724 },
                { 0.979088, 0.993744, 0.983635 },
                { 0.986161, 0.995828, 0.988820 },
                { 0.993136, 0.997910, 0.994276 },
                { 1.000000, 1.000000, 1.000000 }
            };

        return internal::CalcLerp(x, data);
    }

#if defined(TINYCOLORMAP_WITH_QT5) && defined(TINYCOLORMAP_WITH_EIGEN)
    inline QImage CreateMatrixVisualization(const Eigen::MatrixXd& matrix)
    {
        const int w = matrix.cols();
        const int h = matrix.rows();
        const double max_coeff = matrix.maxCoeff();
        const double min_coeff = matrix.minCoeff();
        const Eigen::MatrixXd normalized = (1.0 / (max_coeff - min_coeff)) * (matrix - Eigen::MatrixXd::Constant(h, w, min_coeff));

        QImage image(w, h, QImage::Format_ARGB32);
        for (int x = 0; x < w; ++ x)
        {
            for (int y = 0; y < h; ++ y)
            {
                const QColor color = tinycolormap::GetColor(normalized(y, x)).ConvertToQColor();
                image.setPixel(x, y, color.rgb());
            }
        }

        return image;
    }

    inline void ExportMatrixVisualization(const Eigen::MatrixXd& matrix, const std::string& path)
    {
        CreateMatrixVisualization(matrix).save(QString::fromStdString(path));
    }
#endif
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/magic_enum.hpp included by src/immvision/internal/cv/colormap.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.7.3
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef NEARGYE_MAGIC_ENUM_HPP
#define NEARGYE_MAGIC_ENUM_HPP

#define MAGIC_ENUM_VERSION_MAJOR 0
#define MAGIC_ENUM_VERSION_MINOR 7
#define MAGIC_ENUM_VERSION_PATCH 3

#include <array>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <type_traits>
#include <variant>

#if defined(MAGIC_ENUM_CONFIG_FILE)
#include MAGIC_ENUM_CONFIG_FILE
#endif

#if !defined(MAGIC_ENUM_USING_ALIAS_OPTIONAL)
#include <optional>
#endif
#if !defined(MAGIC_ENUM_USING_ALIAS_STRING)
#endif
#if !defined(MAGIC_ENUM_USING_ALIAS_STRING_VIEW)
#include <string_view>
#endif

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-const-variable"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // May be used uninitialized 'return {};'.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'static_string<N>::chars_' is uninitialized.
#  pragma warning(disable : 28020) // Arithmetic overflow: Using operator '-' on a 4 byte value and then casting the result to a 8 byte value.
#  pragma warning(disable : 26451) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call.
#endif

// Checks magic_enum compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  MAGIC_ENUM_SUPPORTED
#  define MAGIC_ENUM_SUPPORTED 1
#endif

// Checks magic_enum compiler aliases compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  undef  MAGIC_ENUM_SUPPORTED_ALIASES
#  define MAGIC_ENUM_SUPPORTED_ALIASES 1
#endif

// Enum value must be greater or equals than MAGIC_ENUM_RANGE_MIN. By default MAGIC_ENUM_RANGE_MIN = -128.
// If need another min range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN.
#if !defined(MAGIC_ENUM_RANGE_MIN)
#  define MAGIC_ENUM_RANGE_MIN -128
#endif

// Enum value must be less or equals than MAGIC_ENUM_RANGE_MAX. By default MAGIC_ENUM_RANGE_MAX = 128.
// If need another max range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MAX.
#if !defined(MAGIC_ENUM_RANGE_MAX)
#  define MAGIC_ENUM_RANGE_MAX 128
#endif

namespace magic_enum {

// If need another optional type, define the macro MAGIC_ENUM_USING_ALIAS_OPTIONAL.
#if defined(MAGIC_ENUM_USING_ALIAS_OPTIONAL)
    MAGIC_ENUM_USING_ALIAS_OPTIONAL
#else
    using std::optional;
#endif

// If need another string_view type, define the macro MAGIC_ENUM_USING_ALIAS_STRING_VIEW.
#if defined(MAGIC_ENUM_USING_ALIAS_STRING_VIEW)
    MAGIC_ENUM_USING_ALIAS_STRING_VIEW
#else
    using std::string_view;
#endif

// If need another string type, define the macro MAGIC_ENUM_USING_ALIAS_STRING.
#if defined(MAGIC_ENUM_USING_ALIAS_STRING)
    MAGIC_ENUM_USING_ALIAS_STRING
#else
    using std::string;
#endif

    namespace customize {

// Enum value must be in range [MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]. By default MAGIC_ENUM_RANGE_MIN = -128, MAGIC_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN and MAGIC_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
        template <typename E>
        struct enum_range {
            static_assert(std::is_enum_v<E>, "magic_enum::customize::enum_range requires enum type.");
            static constexpr int min = MAGIC_ENUM_RANGE_MIN;
            static constexpr int max = MAGIC_ENUM_RANGE_MAX;
            static_assert(max > min, "magic_enum::customize::enum_range requires max > min.");
        };

        static_assert(MAGIC_ENUM_RANGE_MAX > MAGIC_ENUM_RANGE_MIN, "MAGIC_ENUM_RANGE_MAX must be greater than MAGIC_ENUM_RANGE_MIN.");
        static_assert((MAGIC_ENUM_RANGE_MAX - MAGIC_ENUM_RANGE_MIN) < (std::numeric_limits<std::uint16_t>::max)(), "MAGIC_ENUM_RANGE must be less than UINT16_MAX.");

        namespace detail {
            enum class default_customize_tag {};
            enum class invalid_customize_tag {};
        } // namespace magic_enum::customize::detail

        using customize_t = std::variant<string_view, detail::default_customize_tag, detail::invalid_customize_tag>;

// Default customize.
        inline constexpr auto default_tag = detail::default_customize_tag{};
// Invalid customize.
        inline constexpr auto invalid_tag = detail::invalid_customize_tag{};

// If need custom names for enum, add specialization enum_name for necessary enum type.
        template <typename E>
        constexpr customize_t enum_name(E) noexcept {
            return default_tag;
        }

// If need custom type name for enum, add specialization enum_type_name for necessary enum type.
        template <typename E>
        constexpr customize_t enum_type_name() noexcept {
            return default_tag;
        }

    } // namespace magic_enum::customize

    namespace detail {

        template <auto V, typename = std::enable_if_t<std::is_enum_v<std::decay_t<decltype(V)>>>>
        using enum_constant = std::integral_constant<std::decay_t<decltype(V)>, V>;

        template <typename... T>
        inline constexpr bool always_false_v = false;

        template <typename T>
        struct supported
#if defined(MAGIC_ENUM_SUPPORTED) && MAGIC_ENUM_SUPPORTED || defined(MAGIC_ENUM_NO_CHECK_SUPPORT)
            : std::true_type {};
#else
        : std::false_type {};
#endif

        template <typename T, typename = void>
        struct has_is_flags : std::false_type {};

        template <typename T>
        struct has_is_flags<T, std::void_t<decltype(customize::enum_range<T>::is_flags)>> : std::bool_constant<std::is_same_v<bool, std::decay_t<decltype(customize::enum_range<T>::is_flags)>>> {};

        template <typename T, typename = void>
        struct range_min : std::integral_constant<int, MAGIC_ENUM_RANGE_MIN> {};

        template <typename T>
        struct range_min<T, std::void_t<decltype(customize::enum_range<T>::min)>> : std::integral_constant<decltype(customize::enum_range<T>::min), customize::enum_range<T>::min> {};

        template <typename T, typename = void>
        struct range_max : std::integral_constant<int, MAGIC_ENUM_RANGE_MAX> {};

        template <typename T>
        struct range_max<T, std::void_t<decltype(customize::enum_range<T>::max)>> : std::integral_constant<decltype(customize::enum_range<T>::max), customize::enum_range<T>::max> {};

        template <std::size_t N>
        class static_string {
        public:
            constexpr explicit static_string(string_view str) noexcept : static_string{str, std::make_index_sequence<N>{}} {
                assert(str.size() == N);
            }

            constexpr const char* data() const noexcept { return chars_; }

            constexpr std::size_t size() const noexcept { return N; }

            constexpr operator string_view() const noexcept { return {data(), size()}; }

        private:
            template <std::size_t... I>
            constexpr static_string(string_view str, std::index_sequence<I...>) noexcept : chars_{str[I]..., '\0'} {}

            char chars_[N + 1];
        };

        template <>
        class static_string<0> {
        public:
            constexpr explicit static_string() = default;

            constexpr explicit static_string(string_view) noexcept {}

            constexpr const char* data() const noexcept { return nullptr; }

            constexpr std::size_t size() const noexcept { return 0; }

            constexpr operator string_view() const noexcept { return {}; }
        };

        constexpr string_view pretty_name(string_view name) noexcept {
            for (std::size_t i = name.size(); i > 0; --i) {
                if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
                      (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
                      (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
                      #if defined(MAGIC_ENUM_ENABLE_NONASCII)
                      (name[i - 1] & 0x80) ||
                      #endif
                      (name[i - 1] == '_'))) {
                    name.remove_prefix(i);
                    break;
                }
            }

            if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                                    (name.front() >= 'A' && name.front() <= 'Z') ||
                                    #if defined(MAGIC_ENUM_ENABLE_NONASCII)
                                    (name.front() & 0x80) ||
                                    #endif
                                    (name.front() == '_'))) {
                return name;
            }

            return {}; // Invalid name.
        }

        class case_insensitive {
            static constexpr char to_lower(char c) noexcept {
                return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
            }

        public:
            template <typename L, typename R>
            constexpr auto operator()([[maybe_unused]] L lhs, [[maybe_unused]] R rhs) const noexcept -> std::enable_if_t<std::is_same_v<std::decay_t<L>, char> && std::is_same_v<std::decay_t<R>, char>, bool> {
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
                static_assert(always_false_v<L, R>, "magic_enum::case_insensitive not supported Non-ASCII feature.");
    return false;
#else
                return to_lower(lhs) == to_lower(rhs);
#endif
            }
        };

        constexpr std::size_t find(string_view str, char c) noexcept {
#if defined(__clang__) && __clang_major__ < 9 && defined(__GLIBCXX__) || defined(_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
            // https://stackoverflow.com/questions/56484834/constexpr-stdstring-viewfind-last-of-doesnt-work-on-clang-8-with-libstdc
// https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
  constexpr bool workaround = true;
#else
            constexpr bool workaround = false;
#endif

            if constexpr (workaround) {
                for (std::size_t i = 0; i < str.size(); ++i) {
                    if (str[i] == c) {
                        return i;
                    }
                }

                return string_view::npos;
            } else {
                return str.find_first_of(c);
            }
        }

        template <typename T, std::size_t N, std::size_t... I>
        constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N], std::index_sequence<I...>) noexcept {
            return {{a[I]...}};
        }

        template <typename BinaryPredicate>
        constexpr bool is_default_predicate() noexcept {
            return std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<string_view::value_type>> ||
                   std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<>>;
        }

        template <typename BinaryPredicate>
        constexpr bool is_nothrow_invocable() {
            return is_default_predicate<BinaryPredicate>() ||
                   std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>;
        }

        template <typename BinaryPredicate>
        constexpr bool cmp_equal(string_view lhs, string_view rhs, [[maybe_unused]] BinaryPredicate&& p) noexcept(is_nothrow_invocable<BinaryPredicate>()) {
#if defined(_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
            // https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
  // https://developercommunity.visualstudio.com/content/problem/232218/c-constexpr-string-view.html
  constexpr bool workaround = true;
#else
            constexpr bool workaround = false;
#endif

            if constexpr (!is_default_predicate<BinaryPredicate>() || workaround) {
                if (lhs.size() != rhs.size()) {
                    return false;
                }

                const auto size = lhs.size();
                for (std::size_t i = 0; i < size; ++i) {
                    if (!p(lhs[i], rhs[i])) {
                        return false;
                    }
                }

                return true;
            } else {
                return lhs == rhs;
            }
        }

        template <typename L, typename R>
        constexpr bool cmp_less(L lhs, R rhs) noexcept {
            static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "magic_enum::detail::cmp_less requires integral type.");

            if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
                // If same signedness (both signed or both unsigned).
                return lhs < rhs;
            } else if constexpr (std::is_same_v<L, bool>) { // bool special case
                return static_cast<R>(lhs) < rhs;
            } else if constexpr (std::is_same_v<R, bool>) { // bool special case
                return lhs < static_cast<L>(rhs);
            } else if constexpr (std::is_signed_v<R>) {
                // If 'right' is negative, then result is 'false', otherwise cast & compare.
                return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
            } else {
                // If 'left' is negative, then result is 'true', otherwise cast & compare.
                return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
            }
        }

        template <typename I>
        constexpr I log2(I value) noexcept {
            static_assert(std::is_integral_v<I>, "magic_enum::detail::log2 requires integral type.");

            if constexpr (std::is_same_v<I, bool>) { // bool special case
                return assert(false), value;
            } else {
                auto ret = I{0};
                for (; value > I{1}; value >>= I{1}, ++ret) {}

                return ret;
            }
        }

        template <typename T>
        inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

        template <typename E>
        constexpr auto n() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");

            [[maybe_unused]] constexpr auto custom = customize::enum_type_name<E>();
            static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");
            if constexpr (custom.index() == 0) {
                constexpr auto name = std::get<string_view>(custom);
                static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
                return static_string<name.size()>{name};
            } else if constexpr (custom.index() == 1 && supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
                constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
                constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
    constexpr auto name = string_view{};
#endif
                return static_string<name.size()>{name};
            } else {
                return static_string<0>{}; // Unsupported compiler or Invalid customize.
            }
        }

        template <typename E>
        inline constexpr auto type_name_v = n<E>();

        template <typename E, E V>
        constexpr auto n() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");

            [[maybe_unused]] constexpr auto custom = customize::enum_name<E>(V);
            static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");
            if constexpr (custom.index() == 0) {
                constexpr auto name = std::get<string_view>(custom);
                static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
                return static_string<name.size()>{name};
            } else if constexpr (custom.index() == 1 && supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
                constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
                constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
    constexpr auto name = string_view{};
#endif
                return static_string<name.size()>{name};
            } else {
                return static_string<0>{}; // Unsupported compiler or Invalid customize.
            }
        }

        template <typename E, E V>
        inline constexpr auto enum_name_v = n<E, V>();

        template <typename E, auto V>
        constexpr bool is_valid() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::is_valid requires enum type.");

            return n<E, static_cast<E>(V)>().size() != 0;
        }

        template <typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
        constexpr E value(std::size_t i) noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::value requires enum type.");

            if constexpr (std::is_same_v<U, bool>) { // bool special case
                static_assert(O == 0, "magic_enum::detail::value requires valid offset.");

                return static_cast<E>(i);
            } else if constexpr (IsFlags) {
                return static_cast<E>(U{1} << static_cast<U>(static_cast<int>(i) + O));
            } else {
                return static_cast<E>(static_cast<int>(i) + O);
            }
        }

        template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
        constexpr int reflected_min() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::reflected_min requires enum type.");

            if constexpr (IsFlags) {
                return 0;
            } else {
                constexpr auto lhs = range_min<E>::value;
                constexpr auto rhs = (std::numeric_limits<U>::min)();

                if constexpr (cmp_less(rhs, lhs)) {
                    return lhs;
                } else {
                    return rhs;
                }
            }
        }

        template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
        constexpr int reflected_max() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::reflected_max requires enum type.");

            if constexpr (IsFlags) {
                return std::numeric_limits<U>::digits - 1;
            } else {
                constexpr auto lhs = range_max<E>::value;
                constexpr auto rhs = (std::numeric_limits<U>::max)();

                if constexpr (cmp_less(lhs, rhs)) {
                    return lhs;
                } else {
                    return rhs;
                }
            }
        }

        template <typename E, bool IsFlags>
        inline constexpr auto reflected_min_v = reflected_min<E, IsFlags>();

        template <typename E, bool IsFlags>
        inline constexpr auto reflected_max_v = reflected_max<E, IsFlags>();

        template <std::size_t N>
        constexpr std::size_t values_count(const bool (&valid)[N]) noexcept {
            auto count = std::size_t{0};
            for (std::size_t i = 0; i < N; ++i) {
                if (valid[i]) {
                    ++count;
                }
            }

            return count;
        }

        template <typename E, bool IsFlags, int Min, std::size_t... I>
        constexpr auto values(std::index_sequence<I...>) noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
            constexpr bool valid[sizeof...(I)] = {is_valid<E, value<E, Min, IsFlags>(I)>()...};
            constexpr std::size_t count = values_count(valid);

            if constexpr (count > 0) {
                E values[count] = {};
                for (std::size_t i = 0, v = 0; v < count; ++i) {
                    if (valid[i]) {
                        values[v++] = value<E, Min, IsFlags>(i);
                    }
                }

                return to_array(values, std::make_index_sequence<count>{});
            } else {
                return std::array<E, 0>{};
            }
        }

        template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
        constexpr auto values() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
            constexpr auto min = reflected_min_v<E, IsFlags>;
            constexpr auto max = reflected_max_v<E, IsFlags>;
            constexpr auto range_size = max - min + 1;
            static_assert(range_size > 0, "magic_enum::enum_range requires valid size.");
            static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(), "magic_enum::enum_range requires valid size.");

            return values<E, IsFlags, reflected_min_v<E, IsFlags>>(std::make_index_sequence<range_size>{});
        }

        template <typename E, typename U = std::underlying_type_t<E>>
        constexpr bool is_flags_enum() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::is_flags_enum requires enum type.");

            if constexpr (has_is_flags<E>::value) {
                return customize::enum_range<E>::is_flags;
            } else if constexpr (std::is_same_v<U, bool>) { // bool special case
                return false;
            } else {
#if defined(MAGIC_ENUM_NO_CHECK_FLAGS)
                return false;
#else
                constexpr auto flags_values = values<E, true>();
                constexpr auto default_values = values<E, false>();
                if (flags_values.size() == 0 || default_values.size() > flags_values.size()) {
                    return false;
                }
                for (std::size_t i = 0; i < default_values.size(); ++i) {
                    const auto v = static_cast<U>(default_values[i]);
                    if (v != 0 && (v & (v - 1)) != 0) {
                        return false;
                    }
                }
                return flags_values.size() > 0;
#endif
            }
        }

        template <typename E>
        inline constexpr bool is_flags_v = is_flags_enum<E>();

        template <typename E>
        inline constexpr std::array values_v = values<E, is_flags_v<E>>();

        template <typename E, typename D = std::decay_t<E>>
        using values_t = decltype((values_v<D>));

        template <typename E>
        inline constexpr auto count_v = values_v<E>.size();

        template <typename E, typename U = std::underlying_type_t<E>>
        inline constexpr auto min_v = (count_v<E> > 0) ? static_cast<U>(values_v<E>.front()) : U{0};

        template <typename E, typename U = std::underlying_type_t<E>>
        inline constexpr auto max_v = (count_v<E> > 0) ? static_cast<U>(values_v<E>.back()) : U{0};

        template <typename E, std::size_t... I>
        constexpr auto names(std::index_sequence<I...>) noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::names requires enum type.");

            return std::array<string_view, sizeof...(I)>{{enum_name_v<E, values_v<E>[I]>...}};
        }

        template <typename E>
        inline constexpr std::array names_v = names<E>(std::make_index_sequence<count_v<E>>{});

        template <typename E, typename D = std::decay_t<E>>
        using names_t = decltype((names_v<D>));

        template <typename E, std::size_t... I>
        constexpr auto entries(std::index_sequence<I...>) noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::entries requires enum type.");

            return std::array<std::pair<E, string_view>, sizeof...(I)>{{{values_v<E>[I], enum_name_v<E, values_v<E>[I]>}...}};
        }

        template <typename E>
        inline constexpr std::array entries_v = entries<E>(std::make_index_sequence<count_v<E>>{});

        template <typename E, typename D = std::decay_t<E>>
        using entries_t = decltype((entries_v<D>));

        template <typename E, typename U = std::underlying_type_t<E>>
        constexpr bool is_sparse() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::is_sparse requires enum type.");

            if constexpr (count_v<E> == 0) {
                return false;
            } else if constexpr (std::is_same_v<U, bool>) { // bool special case
                return false;
            } else {
                constexpr auto max = is_flags_v<E> ? log2(max_v<E>) : max_v<E>;
                constexpr auto min = is_flags_v<E> ? log2(min_v<E>) : min_v<E>;
                constexpr auto range_size = max - min + 1;

                return range_size != count_v<E>;
            }
        }

        template <typename E>
        inline constexpr bool is_sparse_v = is_sparse<E>();

        template <typename E, typename U = std::underlying_type_t<E>>
        constexpr U values_ors() noexcept {
            static_assert(is_enum_v<E>, "magic_enum::detail::values_ors requires enum type.");

            auto ors = U{0};
            for (std::size_t i = 0; i < count_v<E>; ++i) {
                ors |= static_cast<U>(values_v<E>[i]);
            }

            return ors;
        }

        template <bool, typename R>
        struct enable_if_enum {};

        template <typename R>
        struct enable_if_enum<true, R> {
            using type = R;
            static_assert(supported<R>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
        };

        template <typename T, typename R, typename BinaryPredicate = std::equal_to<>>
        using enable_if_t = typename enable_if_enum<std::is_enum_v<std::decay_t<T>> && std::is_invocable_r_v<bool, BinaryPredicate, char, char>, R>::type;

        template <typename T, typename Enable = std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
        using enum_concept = T;

        template <typename T, bool = std::is_enum_v<T>>
        struct is_scoped_enum : std::false_type {};

        template <typename T>
        struct is_scoped_enum<T, true> : std::bool_constant<!std::is_convertible_v<T, std::underlying_type_t<T>>> {};

        template <typename T, bool = std::is_enum_v<T>>
        struct is_unscoped_enum : std::false_type {};

        template <typename T>
        struct is_unscoped_enum<T, true> : std::bool_constant<std::is_convertible_v<T, std::underlying_type_t<T>>> {};

        template <typename T, bool = std::is_enum_v<std::decay_t<T>>>
        struct underlying_type {};

        template <typename T>
        struct underlying_type<T, true> : std::underlying_type<std::decay_t<T>> {};

        template <typename Value, typename = void>
        struct constexpr_hash_t;

        template <typename Value>
        struct constexpr_hash_t<Value, std::enable_if_t<is_enum_v<Value>>> {
            constexpr auto operator()(Value value) const noexcept {
                using U = typename underlying_type<Value>::type;
                if constexpr (std::is_same_v<U, bool>) { // bool special case
                    return static_cast<std::size_t>(value);
                } else {
                    return static_cast<U>(value);
                }
            }
            using secondary_hash = constexpr_hash_t;
        };

        template <typename Value>
        struct constexpr_hash_t<Value, std::enable_if_t<std::is_same_v<Value, string_view>>> {
            static constexpr std::uint32_t crc_table[256] {
                0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
                0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
                0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
                0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
                0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
                0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
                0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
                0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
                0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
                0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
                0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
                0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
                0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
                0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
                0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
                0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
                0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
                0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
                0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
                0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
                0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
                0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
                0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
                0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
                0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
                0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
                0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
                0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
                0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
                0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
                0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
                0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
            };
            constexpr std::uint32_t operator()(string_view value) const noexcept {
                auto crc = static_cast<std::uint32_t>(0xffffffffL);
                for (const auto c : value) {
                    crc = (crc >> 8) ^ crc_table[(crc ^ static_cast<std::uint32_t>(c)) & 0xff];
                }
                return crc ^ 0xffffffffL;
            }

            struct secondary_hash {
                constexpr std::uint32_t operator()(string_view value) const noexcept {
                    auto acc = static_cast<std::uint64_t>(2166136261ULL);
                    for (const auto c : value) {
                        acc = ((acc ^ static_cast<std::uint64_t>(c)) * static_cast<std::uint64_t>(16777619ULL)) & std::numeric_limits<std::uint32_t>::max();
                    }
                    return static_cast<std::uint32_t>(acc);
                }
            };
        };

        template <typename Hash>
        constexpr static Hash hash_v{};

        template <auto* GlobValues, typename Hash>
        constexpr auto calculate_cases(std::size_t Page) noexcept {
            constexpr std::array values = *GlobValues;
            constexpr std::size_t size = values.size();

            using switch_t = std::invoke_result_t<Hash, typename decltype(values)::value_type>;
            static_assert(std::is_integral_v<switch_t> && !std::is_same_v<switch_t, bool>);
            const std::size_t values_to = (std::min)(static_cast<std::size_t>(256), size - Page);

            std::array<switch_t, 256> result{};
            auto fill = result.begin();
            for (auto first = values.begin() + Page, last = values.begin() + Page + values_to; first != last; ) {
                *fill++ = hash_v<Hash>(*first++);
            }

            // dead cases, try to avoid case collisions
            for (switch_t last_value = result[values_to - 1]; fill != result.end() && last_value != (std::numeric_limits<switch_t>::max)(); *fill++ = ++last_value) {
            }

            auto it = result.begin();
            for (auto last_value = (std::numeric_limits<switch_t>::min)(); fill != result.end(); *fill++ = last_value++) {
                while (last_value == *it) {
                    ++last_value, ++it;
                }
            }

            return result;
        }

        template <typename R, typename F, typename... Args>
        constexpr R invoke_r(F&& f, Args&&... args) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>) {
            if constexpr (std::is_void_v<R>) {
                std::forward<F>(f)(std::forward<Args>(args)...);
            } else {
                return static_cast<R>(std::forward<F>(f)(std::forward<Args>(args)...));
            }
        }

        enum class case_call_t {
            index, value
        };

        template <typename T = void>
        inline constexpr auto default_result_type_lambda = []() noexcept(std::is_nothrow_default_constructible_v<T>) { return T{}; };

        template <>
        inline constexpr auto default_result_type_lambda<void> = []() noexcept {};

        template <auto* Arr, typename Hash>
        constexpr bool no_duplicate() noexcept {
            using value_t = std::decay_t<decltype((*Arr)[0])>;
            using hash_value_t = std::invoke_result_t<Hash, value_t>;
            std::array<hash_value_t, Arr->size()> hashes{};
            std::size_t size = 0;
            for (auto elem : *Arr) {
                hashes[size] = hash_v<Hash>(elem);
                for (auto i = size++; i > 0; --i) {
                    if (hashes[i] < hashes[i - 1]) {
                        auto tmp = hashes[i];
                        hashes[i] = hashes[i - 1];
                        hashes[i - 1] = tmp;
                    } else if (hashes[i] == hashes[i - 1]) {
                        return false;
                    } else {
                        break;
                    }
                }
            }
            return true;
        }

#define MAGIC_ENUM_FOR_EACH_256(T) T(0)T(1)T(2)T(3)T(4)T(5)T(6)T(7)T(8)T(9)T(10)T(11)T(12)T(13)T(14)T(15)T(16)T(17)T(18)T(19)T(20)T(21)T(22)T(23)T(24)T(25)T(26)T(27)T(28)T(29)T(30)T(31)          \
  T(32)T(33)T(34)T(35)T(36)T(37)T(38)T(39)T(40)T(41)T(42)T(43)T(44)T(45)T(46)T(47)T(48)T(49)T(50)T(51)T(52)T(53)T(54)T(55)T(56)T(57)T(58)T(59)T(60)T(61)T(62)T(63)                                 \
  T(64)T(65)T(66)T(67)T(68)T(69)T(70)T(71)T(72)T(73)T(74)T(75)T(76)T(77)T(78)T(79)T(80)T(81)T(82)T(83)T(84)T(85)T(86)T(87)T(88)T(89)T(90)T(91)T(92)T(93)T(94)T(95)                                 \
  T(96)T(97)T(98)T(99)T(100)T(101)T(102)T(103)T(104)T(105)T(106)T(107)T(108)T(109)T(110)T(111)T(112)T(113)T(114)T(115)T(116)T(117)T(118)T(119)T(120)T(121)T(122)T(123)T(124)T(125)T(126)T(127)     \
  T(128)T(129)T(130)T(131)T(132)T(133)T(134)T(135)T(136)T(137)T(138)T(139)T(140)T(141)T(142)T(143)T(144)T(145)T(146)T(147)T(148)T(149)T(150)T(151)T(152)T(153)T(154)T(155)T(156)T(157)T(158)T(159) \
  T(160)T(161)T(162)T(163)T(164)T(165)T(166)T(167)T(168)T(169)T(170)T(171)T(172)T(173)T(174)T(175)T(176)T(177)T(178)T(179)T(180)T(181)T(182)T(183)T(184)T(185)T(186)T(187)T(188)T(189)T(190)T(191) \
  T(192)T(193)T(194)T(195)T(196)T(197)T(198)T(199)T(200)T(201)T(202)T(203)T(204)T(205)T(206)T(207)T(208)T(209)T(210)T(211)T(212)T(213)T(214)T(215)T(216)T(217)T(218)T(219)T(220)T(221)T(222)T(223) \
  T(224)T(225)T(226)T(227)T(228)T(229)T(230)T(231)T(232)T(233)T(234)T(235)T(236)T(237)T(238)T(239)T(240)T(241)T(242)T(243)T(244)T(245)T(246)T(247)T(248)T(249)T(250)T(251)T(252)T(253)T(254)T(255)

#define MAGIC_ENUM_CASE(val)                                                                                                      \
  case cases[val]:                                                                                                                \
    if constexpr ((val) + Page < size) {                                                                                          \
      if (!pred(values[val + Page], searched)) {                                                                                  \
        break;                                                                                                                    \
      }                                                                                                                           \
      if constexpr (CallValue == case_call_t::index) {                                                                            \
        if constexpr (std::is_invocable_r_v<result_t, Lambda, std::integral_constant<std::size_t, val + Page>>) {                 \
          return detail::invoke_r<result_t>(std::forward<Lambda>(lambda), std::integral_constant<std::size_t, val + Page>{});     \
        } else if constexpr (std::is_invocable_v<Lambda, std::integral_constant<std::size_t, val + Page>>) {                      \
          assert(false && "magic_enum::detail::constexpr_switch wrong result type.");                                             \
        }                                                                                                                         \
      } else if constexpr (CallValue == case_call_t::value) {                                                                     \
        if constexpr (std::is_invocable_r_v<result_t, Lambda, enum_constant<values[val + Page]>>) {                               \
          return detail::invoke_r<result_t>(std::forward<Lambda>(lambda), enum_constant<values[val + Page]>{});                   \
        } else if constexpr (std::is_invocable_r_v<result_t, Lambda, enum_constant<values[val + Page]>>) {                        \
          assert(false && "magic_enum::detail::constexpr_switch wrong result type.");                                             \
        }                                                                                                                         \
      }                                                                                                                           \
      break;                                                                                                                      \
    } else [[fallthrough]];

        template <auto* GlobValues,
            case_call_t CallValue,
            std::size_t Page = 0,
            typename Hash = constexpr_hash_t<typename std::decay_t<decltype(*GlobValues)>::value_type>,
            typename Lambda, typename ResultGetterType = decltype(default_result_type_lambda<>),
            typename BinaryPredicate = std::equal_to<>>
        constexpr std::invoke_result_t<ResultGetterType> constexpr_switch(
            Lambda&& lambda,
            typename std::decay_t<decltype(*GlobValues)>::value_type searched,
            ResultGetterType&& def = default_result_type_lambda<>,
            BinaryPredicate&& pred = {}) {
            using result_t = std::invoke_result_t<ResultGetterType>;
            using hash_t = std::conditional_t<no_duplicate<GlobValues, Hash>(), Hash, typename Hash::secondary_hash>;
            constexpr std::array values = *GlobValues;
            constexpr std::size_t size = values.size();
            constexpr std::array cases = calculate_cases<GlobValues, hash_t>(Page);

            switch (hash_v<hash_t>(searched)) {
                MAGIC_ENUM_FOR_EACH_256(MAGIC_ENUM_CASE)
                default:
                    if constexpr (size > 256 + Page) {
                        return constexpr_switch<GlobValues, CallValue, Page + 256, Hash>(std::forward<Lambda>(lambda), searched, std::forward<ResultGetterType>(def));
                    }
                    break;
            }
            return def();
        }

#undef MAGIC_ENUM_FOR_EACH_256
#undef MAGIC_ENUM_CASE

        template <typename E, typename Lambda, std::size_t... I>
        constexpr auto for_each(Lambda&& lambda, std::index_sequence<I...>) {
            static_assert(is_enum_v<E>, "magic_enum::detail::for_each requires enum type.");
            constexpr bool has_void_return = (std::is_void_v<std::invoke_result_t<Lambda, enum_constant<values_v<E>[I]>>> || ...);
            constexpr bool all_same_return = (std::is_same_v<std::invoke_result_t<Lambda, enum_constant<values_v<E>[0]>>, std::invoke_result_t<Lambda, enum_constant<values_v<E>[I]>>> && ...);

            if constexpr (has_void_return) {
                (lambda(enum_constant<values_v<E>[I]>{}), ...);
            } else if constexpr (all_same_return) {
                return std::array{lambda(enum_constant<values_v<E>[I]>{})...};
            } else {
                return std::tuple{lambda(enum_constant<values_v<E>[I]>{})...};
            }
        }

    } // namespace magic_enum::detail

// Checks is magic_enum supported compiler.
    inline constexpr bool is_magic_enum_supported = detail::supported<void>::value;

    template <typename T>
    using Enum = detail::enum_concept<T>;

// Checks whether T is an Unscoped enumeration type.
// Provides the member constant value which is equal to true, if T is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration) type. Otherwise, value is equal to false.
    template <typename T>
    struct is_unscoped_enum : detail::is_unscoped_enum<T> {};

    template <typename T>
    inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;

// Checks whether T is an Scoped enumeration type.
// Provides the member constant value which is equal to true, if T is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations) type. Otherwise, value is equal to false.
    template <typename T>
    struct is_scoped_enum : detail::is_scoped_enum<T> {};

    template <typename T>
    inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

// If T is a complete enumeration type, provides a member typedef type that names the underlying type of T.
// Otherwise, if T is not an enumeration type, there is no member type. Otherwise (T is an incomplete enumeration type), the program is ill-formed.
    template <typename T>
    struct underlying_type : detail::underlying_type<T> {};

    template <typename T>
    using underlying_type_t = typename underlying_type<T>::type;

    template <auto V>
    using enum_constant = detail::enum_constant<V>;

// Returns type name of enum.
    template <typename E>
    [[nodiscard]] constexpr auto enum_type_name() noexcept -> detail::enable_if_t<E, string_view> {
        constexpr string_view name = detail::type_name_v<std::decay_t<E>>;
        static_assert(!name.empty(), "magic_enum::enum_type_name enum type does not have a name.");

        return name;
    }

// Returns number of enum values.
    template <typename E>
    [[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_t<E, std::size_t> {
        return detail::count_v<std::decay_t<E>>;
    }

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
    template <typename E>
    [[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
        using D = std::decay_t<E>;

        if constexpr (detail::is_sparse_v<D>) {
            return assert((index < detail::count_v<D>)), detail::values_v<D>[index];
        } else {
            constexpr bool is_flag = detail::is_flags_v<D>;
            constexpr auto min = is_flag ? detail::log2(detail::min_v<D>) : detail::min_v<D>;

            return assert((index < detail::count_v<D>)), detail::value<D, min, is_flag>(index);
        }
    }

// Returns enum value at specified index.
    template <typename E, std::size_t I>
    [[nodiscard]] constexpr auto enum_value() noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
        using D = std::decay_t<E>;
        static_assert(I < detail::count_v<D>, "magic_enum::enum_value out of range.");

        return enum_value<D>(I);
    }

// Returns std::array with enum values, sorted by enum value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_values() noexcept -> detail::enable_if_t<E, detail::values_t<E>> {
        return detail::values_v<std::decay_t<E>>;
    }

// Returns integer value from enum value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_integer(E value) noexcept -> detail::enable_if_t<E, underlying_type_t<E>> {
        return static_cast<underlying_type_t<E>>(value);
    }


// Returns underlying value from enum value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_underlying(E value) noexcept -> detail::enable_if_t<E, underlying_type_t<E>> {
        return static_cast<underlying_type_t<E>>(value);
    }

// Obtains index in enum values from enum value.
// Returns optional with index.
    template <typename E>
    [[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_t<E, optional<std::size_t>> {
        using D = std::decay_t<E>;
        using U = underlying_type_t<D>;

        if constexpr (detail::count_v<D> == 0) {
            return {}; // Empty enum.
        } else if constexpr (detail::is_sparse_v<D> || detail::is_flags_v<D>) {
            return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::index>(
                [](std::size_t i) { return optional<std::size_t>{i}; },
                value,
                detail::default_result_type_lambda<optional<std::size_t>>);
        } else {
            const auto v = static_cast<U>(value);
            if (v >= detail::min_v<D> && v <= detail::max_v<D>) {
                return static_cast<std::size_t>(v - detail::min_v<D>);
            }
            return {}; // Invalid value or out of range.
        }
    }

// Returns name from static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
    template <auto V>
    [[nodiscard]] constexpr auto enum_name() noexcept -> detail::enable_if_t<decltype(V), string_view> {
        constexpr string_view name = detail::enum_name_v<std::decay_t<decltype(V)>, V>;
        static_assert(!name.empty(), "magic_enum::enum_name enum value does not have a name.");

        return name;
    }

// Returns name from enum value.
// If enum value does not have name or value out of range, returns empty string.
    template <typename E>
    [[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_t<E, string_view> {
        using D = std::decay_t<E>;

        if (const auto i = enum_index<D>(value)) {
            return detail::names_v<D>[*i];
        }
        return {};
    }

// Returns name from enum-flags value.
// If enum-flags value does not have name or value out of range, returns empty string.
    template <typename E>
    [[nodiscard]] auto enum_flags_name(E value) -> detail::enable_if_t<E, string> {
        using D = std::decay_t<E>;
        using U = underlying_type_t<D>;

        if constexpr (detail::is_flags_v<D>) {
            string name;
            auto check_value = U{0};
            for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
                if (const auto v = static_cast<U>(enum_value<D>(i)); (static_cast<U>(value) & v) != 0) {
                    check_value |= v;
                    const auto n = detail::names_v<D>[i];
                    if (!name.empty()) {
                        name.append(1, '|');
                    }
                    name.append(n.data(), n.size());
                }
            }

            if (check_value != 0 && check_value == static_cast<U>(value)) {
                return name;
            }

            return {}; // Invalid value or out of range.
        } else {
            return string{enum_name(value)};
        }
    }

// Returns std::array with names, sorted by enum value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_names() noexcept -> detail::enable_if_t<E, detail::names_t<E>> {
        return detail::names_v<std::decay_t<E>>;
    }

// Returns std::array with pairs (value, name), sorted by enum value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_entries() noexcept -> detail::enable_if_t<E, detail::entries_t<E>> {
        return detail::entries_v<std::decay_t<E>>;
    }

// Obtains enum value from integer value.
// Returns optional with enum value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_cast(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
        using D = std::decay_t<E>;
        using U = underlying_type_t<D>;

        if constexpr (detail::count_v<D> == 0) {
            return {}; // Empty enum.
        } else if constexpr (detail::is_sparse_v<D>) {
            if constexpr (detail::is_flags_v<D>) {
                constexpr auto count = detail::count_v<D>;
                auto check_value = U{0};
                for (std::size_t i = 0; i < count; ++i) {
                    if (const auto v = static_cast<U>(enum_value<D>(i)); (value & v) != 0) {
                        check_value |= v;
                    }
                }

                if (check_value != 0 && check_value == value) {
                    return static_cast<D>(value);
                }
                return {}; // Invalid value or out of range.
            } else {
                return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
                    [](D v) { return optional<D>{v}; },
                    static_cast<D>(value),
                    detail::default_result_type_lambda<optional<D>>);
            }
        } else {
            constexpr auto min = detail::min_v<D>;
            constexpr auto max = detail::is_flags_v<D> ? detail::values_ors<D>() : detail::max_v<D>;

            if (value >= min && value <= max) {
                return static_cast<D>(value);
            }
            return {}; // Invalid value or out of range.
        }
    }

// Allows you to write magic_enum::enum_cast<foo>("bar", magic_enum::case_insensitive);
    inline constexpr auto case_insensitive = detail::case_insensitive{};

// Obtains enum value from name.
// Returns optional with enum value.
    template <typename E, typename BinaryPredicate = std::equal_to<>>
    [[nodiscard]] constexpr auto enum_cast(string_view value, [[maybe_unused]] BinaryPredicate&& p = {}) noexcept(detail::is_nothrow_invocable<BinaryPredicate>()) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
        static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_cast requires bool(char, char) invocable predicate.");
        using D = std::decay_t<E>;
        using U = underlying_type_t<D>;

        if constexpr (detail::count_v<D> == 0) {
            return {}; // Empty enum.
        } else if constexpr (detail::is_flags_v<D>) {
            auto result = U{0};
            while (!value.empty()) {
                const auto d = detail::find(value, '|');
                const auto s = (d == string_view::npos) ? value : value.substr(0, d);
                auto f = U{0};
                for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
                    if (detail::cmp_equal(s, detail::names_v<D>[i], p)) {
                        f = static_cast<U>(enum_value<D>(i));
                        result |= f;
                        break;
                    }
                }
                if (f == U{0}) {
                    return {}; // Invalid value or out of range.
                }
                value.remove_prefix((d == string_view::npos) ? value.size() : d + 1);
            }

            if (result != U{0}) {
                return static_cast<D>(result);
            }
            return {}; // Invalid value or out of range.
        } else if constexpr (detail::count_v<D> > 0) {
            if constexpr (detail::is_default_predicate<BinaryPredicate>()) {
                return detail::constexpr_switch<&detail::names_v<D>, detail::case_call_t::index>(
                    [](std::size_t i) { return optional<D>{detail::values_v<D>[i]}; },
                    value,
                    detail::default_result_type_lambda<optional<D>>,
                    [&p](string_view lhs, string_view rhs) { return detail::cmp_equal(lhs, rhs, p); });
            } else {
                for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
                    if (detail::cmp_equal(value, detail::names_v<D>[i], p)) {
                        return enum_value<D>(i);
                    }
                }
                return {}; // Invalid value or out of range.
            }
        }
    }

// Obtains index in enum values from static storage enum variable.
    template <auto V>
    [[nodiscard]] constexpr auto enum_index() noexcept -> detail::enable_if_t<decltype(V), std::size_t> {
        constexpr auto index = enum_index<std::decay_t<decltype(V)>>(V);
        static_assert(index, "magic_enum::enum_index enum value does not have a index.");

        return *index;
    }

// Checks whether enum contains enumerator with such enum value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
        using D = std::decay_t<E>;
        using U = underlying_type_t<D>;

        return static_cast<bool>(enum_cast<D>(static_cast<U>(value)));
    }

// Checks whether enum contains enumerator with such integer value.
    template <typename E>
    [[nodiscard]] constexpr auto enum_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, bool> {
        using D = std::decay_t<E>;

        return static_cast<bool>(enum_cast<D>(value));
    }

// Checks whether enum contains enumerator with such name.
    template <typename E, typename BinaryPredicate = std::equal_to<>>
    [[nodiscard]] constexpr auto enum_contains(string_view value, BinaryPredicate&& p = {}) noexcept(detail::is_nothrow_invocable<BinaryPredicate>()) -> detail::enable_if_t<E, bool, BinaryPredicate> {
        static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_contains requires bool(char, char) invocable predicate.");
        using D = std::decay_t<E>;

        return static_cast<bool>(enum_cast<D>(value, std::forward<BinaryPredicate>(p)));
    }

    template <typename Result = void, typename E, typename Lambda>
    constexpr auto enum_switch(Lambda&& lambda, E value) -> detail::enable_if_t<E, Result> {
        using D = std::decay_t<E>;

        return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
            std::forward<Lambda>(lambda),
            value,
            detail::default_result_type_lambda<Result>);
    }

    template <typename Result, typename E, typename Lambda>
    constexpr auto enum_switch(Lambda&& lambda, E value, Result&& result) -> detail::enable_if_t<E, Result> {
        using D = std::decay_t<E>;

        return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
            std::forward<Lambda>(lambda),
            value,
            [&result] { return std::forward<Result>(result); });
    }

    template <typename E, typename Result = void, typename BinaryPredicate = std::equal_to<>, typename Lambda>
    constexpr auto enum_switch(Lambda&& lambda, string_view name, BinaryPredicate&& p = {}) -> detail::enable_if_t<E, Result, BinaryPredicate> {
        static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_switch requires bool(char, char) invocable predicate.");
        using D = std::decay_t<E>;

        if (const auto v = enum_cast<D>(name, std::forward<BinaryPredicate>(p))) {
            return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v);
        }
        return detail::default_result_type_lambda<Result>();
    }

    template <typename E, typename Result, typename BinaryPredicate = std::equal_to<>, typename Lambda>
    constexpr auto enum_switch(Lambda&& lambda, string_view name, Result&& result, BinaryPredicate&& p = {}) -> detail::enable_if_t<E, Result, BinaryPredicate> {
        static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>, "magic_enum::enum_switch requires bool(char, char) invocable predicate.");
        using D = std::decay_t<E>;

        if (const auto v = enum_cast<D>(name, std::forward<BinaryPredicate>(p))) {
            return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v, std::forward<Result>(result));
        }
        return std::forward<Result>(result);
    }

    template <typename E, typename Result = void, typename Lambda>
    constexpr auto enum_switch(Lambda&& lambda, underlying_type_t<E> value) -> detail::enable_if_t<E, Result> {
        using D = std::decay_t<E>;

        if (const auto v = enum_cast<D>(value)) {
            return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v);
        }
        return detail::default_result_type_lambda<Result>();
    }

    template <typename E, typename Result, typename Lambda>
    constexpr auto enum_switch(Lambda&& lambda, underlying_type_t<E> value, Result&& result) -> detail::enable_if_t<E, Result> {
        using D = std::decay_t<E>;

        if (const auto v = enum_cast<D>(value)) {
            return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v, std::forward<Result>(result));
        }
        return std::forward<Result>(result);
    }

    template <typename E, typename Lambda>
    constexpr auto enum_for_each(Lambda&& lambda) {
        using D = std::decay_t<E>;
        static_assert(std::is_enum_v<D>, "magic_enum::enum_for_each requires enum type.");

        return detail::for_each<D>(std::forward<Lambda>(lambda), std::make_index_sequence<detail::count_v<D>>{});
    }

    namespace detail {

        template <typename E>
        constexpr optional<std::uintmax_t> fuse_one_enum(optional<std::uintmax_t> hash, E value) noexcept {
            if (hash) {
                if (const auto index = enum_index(value)) {
                    return (*hash << log2(enum_count<E>() + 1)) | *index;
                }
            }
            return {};
        }

        template <typename E>
        constexpr optional<std::uintmax_t> fuse_enum(E value) noexcept {
            return fuse_one_enum(0, value);
        }

        template <typename E, typename... Es>
        constexpr optional<std::uintmax_t> fuse_enum(E head, Es... tail) noexcept {
            return fuse_one_enum(fuse_enum(tail...), head);
        }

        template <typename... Es>
        constexpr auto typesafe_fuse_enum(Es... values) noexcept {
            enum class enum_fuse_t : std::uintmax_t;
            const auto fuse = fuse_enum(values...);
            if (fuse) {
                return optional<enum_fuse_t>{static_cast<enum_fuse_t>(*fuse)};
            }
            return optional<enum_fuse_t>{};
        }

    } // namespace magic_enum::detail

// Returns a bijective mix of several enum values. This can be used to emulate 2D switch/case statements.
    template <typename... Es>
    [[nodiscard]] constexpr auto enum_fuse(Es... values) noexcept {
        static_assert((std::is_enum_v<std::decay_t<Es>> && ...), "magic_enum::enum_fuse requires enum type.");
        static_assert(sizeof...(Es) >= 2, "magic_enum::enum_fuse requires at least 2 values.");
        static_assert((detail::log2(enum_count<Es>() + 1) + ...) <= (sizeof(std::uintmax_t) * 8), "magic_enum::enum_fuse does not work for large enums");
#if defined(MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE)
        const auto fuse = detail::fuse_enum<std::decay_t<Es>...>(values...);
#else
        const auto fuse = detail::typesafe_fuse_enum<std::decay_t<Es>...>(values...);
#endif
        return assert(fuse), fuse;
    }

    namespace ostream_operators {

        template <typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
        std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, E value) {
            using D = std::decay_t<E>;
            using U = underlying_type_t<D>;

            if constexpr (detail::supported<D>::value) {
                if (const auto name = enum_flags_name<D>(value); !name.empty()) {
                    for (const auto c : name) {
                        os.put(c);
                    }
                    return os;
                }
            }
            return (os << static_cast<U>(value));
        }

        template <typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
        std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, optional<E> value) {
            return value ? (os << *value) : os;
        }

    } // namespace magic_enum::ostream_operators

    namespace istream_operators {

        template <typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
        std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& is, E& value) {
            using D = std::decay_t<E>;

            std::basic_string<Char, Traits> s;
            is >> s;
            if (const auto v = enum_cast<D>(s)) {
                value = *v;
            } else {
                is.setstate(std::basic_ios<Char>::failbit);
            }
            return is;
        }

    } // namespace magic_enum::istream_operators

    namespace iostream_operators {

        using namespace ostream_operators;
        using namespace istream_operators;

    } // namespace magic_enum::iostream_operators

    namespace bitwise_operators {

        template <typename E, detail::enable_if_t<E, int> = 0>
        constexpr E operator~(E rhs) noexcept {
            return static_cast<E>(~static_cast<underlying_type_t<E>>(rhs));
        }

        template <typename E, detail::enable_if_t<E, int> = 0>
        constexpr E operator|(E lhs, E rhs) noexcept {
            return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) | static_cast<underlying_type_t<E>>(rhs));
        }

        template <typename E, detail::enable_if_t<E, int> = 0>
        constexpr E operator&(E lhs, E rhs) noexcept {
            return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) & static_cast<underlying_type_t<E>>(rhs));
        }

        template <typename E, detail::enable_if_t<E, int> = 0>
        constexpr E operator^(E lhs, E rhs) noexcept {
            return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) ^ static_cast<underlying_type_t<E>>(rhs));
        }

        template <typename E, detail::enable_if_t<E, int> = 0>
        constexpr E& operator|=(E& lhs, E rhs) noexcept {
            return lhs = (lhs | rhs);
        }

        template <typename E, detail::enable_if_t<E, int> = 0>
        constexpr E& operator&=(E& lhs, E rhs) noexcept {
            return lhs = (lhs & rhs);
        }

        template <typename E, detail::enable_if_t<E, int> = 0>
        constexpr E& operator^=(E& lhs, E rhs) noexcept {
            return lhs = (lhs ^ rhs);
        }

    } // namespace magic_enum::bitwise_operators

} // namespace magic_enum

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_HPP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/math_utils.h included by src/immvision/internal/cv/colormap.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace MathUtils
    {
        int RoundInt(double v);
        double MaximumValue(const std::vector<double> &v);
        double MinimumValue(const std::vector<double> &v);

        inline double Lerp(double a, double b, double x) noexcept
        {
            return (a + (b - a) * x);
        }
        inline double UnLerp(double a, double b, double x) noexcept
        {
            return (x - a) / (b - a);
        }

        inline std::vector<double> arange(double a, double b, double step)
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
    } // namespace MathUtils


} // namespace ImmVision
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/gl_texture.h included by src/immvision/internal/cv/colormap.cpp          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    // GlTexture contains an OpenGL texture which can be created or updated from an ImageBuffer (C++), or numpy array (Python)
    struct GlTexture
    {
        //
        // Constructors
        //

        // Create an empty texture
        GlTexture();
        // Create a texture from an image (ImageBuffer in C++, numpy array in Python)
        // isColorOrderBGR: if true, the image is assumed to be in BGR order (OpenCV default)
        GlTexture(const ImageBuffer& image, bool isColorOrderBGR = false);
        // The destructor will delete the texture from the GPU
        ~GlTexture();

        // GlTexture is non copiable (since it holds a reference to a texture stored on the GPU),
        // but it is movable.
        GlTexture(const GlTexture& ) = delete;
        GlTexture& operator=(const GlTexture& ) = delete;
        GlTexture(GlTexture&& other) noexcept = default;
        GlTexture& operator=(GlTexture&& other) noexcept = default;


        //
        // Methods
        //

        // Update the texture from a new image (ImageBuffer in C++, numpy array in Python).
        void UpdateFromImage(const ImageBuffer& image, bool isColorOrderBGR = false);
        // Returns the size as ImVec2
        ImVec2 SizeImVec2() const;


        //
        // Members
        //

        // OpenGL texture ID on the GPU
        ImTextureID TextureId;
        // Image size in pixels
        Size ImageSize;
    };
} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/imgui_imm.h included by src/immvision/internal/cv/colormap.cpp           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Some extensions to ImGui, specific to ImmVision
namespace ImGuiImm
{
    // A slider that works for float, double, and long double
    // Internally, it calls ImGui::SliderFloat (so that the accuracy may be a little reduced)
    // Be sure to cast all your params when calling, especially v_min and v_max
    template<typename AnyFloat>
    bool SliderAnyFloat(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min = AnyFloat(0.),
        AnyFloat v_max = AnyFloat(1.),
        float width = 200.f,
        bool logarithmic = false,
        int nb_decimals = 6);

    template<typename AnyFloat>
    bool SliderAnyFloatLogarithmic(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min = AnyFloat(0.),
        AnyFloat v_max = AnyFloat(1.),
        float width = 200.f,
        int nb_decimals = 6);


    ImVec2 ComputeDisplayImageSize(ImVec2 askedImageSize, ImVec2 realImageSize);
    ImmVision::Size ComputeDisplayImageSize(ImmVision::Size askedImageSize, ImmVision::Size realImageSize);

    void PushDisabled();
    void PopDisabled();
    void SameLineAlignRight(float rightMargin = 0.f, float alignRegionWidth = -1.f);

    // cf https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353
    void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel();

    void BeginGroupPanel_FlagBorder(const char* name, bool draw_border, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel_FlagBorder();
    ImVec2 GroupPanel_FlagBorder_LastKnownSize(const char* name);

    // Draw a fixed width Separator
    // useful when ImGui::Separator() overflows to the right
    void SeparatorFixedWidth(float width);

    void BeginGroupFixedWidth(float width);
    void EndGroupFixedWidth();

    bool ButtonWithTooltip(const std::string& label, const std::string& tooltip);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/colormap.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <cmath>    // std::sqrt
#include <limits>   // std::numeric_limits
#include "imgui_internal.h"

#include <math.h>
#include <stdio.h>

namespace ImmVision
{
    namespace Colormap
    {
        namespace  // utility functions for this file
        {
            void GlTexture_Draw(const GlTexture& texture, const ImVec2& size)
            {
                ImVec2 size_(size);
                if (size.x == 0.f)
                    size_ = texture.SizeImVec2();
                ImGui::Image(texture.TextureId, size_);
            }

            bool GlTexture_DrawButton(const GlTexture& texture, const ImVec2& size)
            {
                ImVec2 size_(size);
                if (size.x == 0.f)
                    size_ = texture.SizeImVec2();
                char id[64];
                snprintf(id, 64, "##%p", &texture);
                return ImGui::ImageButton(id, texture.TextureId, size_);
            }
        }

        //
        // Base operations for ColormapSettingsData
        //
        bool IsNone(const ColormapSettingsData& a)
        {
            ColormapSettingsData empty;
            return IsEqual(a, empty);
        }


        bool IsEqual(const ColormapSettingsData& v1, const ColormapSettingsData& v2)
        {
            if (v1.Colormap != v2.Colormap)
                return false;
            if (fabs(v1.ColormapScaleMax - v2.ColormapScaleMax) > 1E-6)
                return false;
            if (fabs(v1.ColormapScaleMin - v2.ColormapScaleMin) > 1E-6)
                return false;
            if (v1.internal_ColormapHovered != v2.internal_ColormapHovered)
                return false;
            if (!IsEqual(v1.ColormapScaleFromStats, v2.ColormapScaleFromStats))
                return false;
            return true;
        }


        bool IsEqual(const ColormapScaleFromStatsData& v1, const ColormapScaleFromStatsData& v2)
        {
            if (v1.ColorMapStatsType != v2.ColorMapStatsType)
                return false;
            if (fabs(v1.NbSigmas - v2.NbSigmas) > 1E-6)
                return false;
            if (v1.UseStatsMin != v2.UseStatsMin)
                return false;
            if (v1.UseStatsMax != v2.UseStatsMax)
                return false;
            return true;
        }


        bool CanColormap(const ImageBuffer &image)
        {
            return (ImageDepthIsFloat(image.depth) && image.channels == 1);
        }


        ColormapSettingsData ComputeInitialColormapSettings(const ImageBuffer& m)
        {
            (void)m;
            ColormapSettingsData r;
            return r;
        }


        //
        // Colormaps images and textures
        //


        using ColormapType = tinycolormap::ColormapType;


        std::vector<std::string> AvailableColormaps()
        {
            std::vector<std::string> r;
            magic_enum::enum_for_each<ColormapType>([&r] (auto val) {
                ColormapType type = val;
                const char* name = magic_enum::enum_name(type).data();
                r.push_back(name);
            });
            return r;
        }


        ImageBuffer MakeColormapImage(tinycolormap::ColormapType colorType)
        {
            int w = 256, h = 15;
            ImageBuffer m = ImageBuffer::Zeros(w, h, 3, ImageDepth::uint8);
            for (int x = 0; x < w; ++x)
            {
                double k = MathUtils::UnLerp(0., (double)w, (double)x);
                auto col = tinycolormap::GetColor(k, colorType);
                for (int y = 0; y < h; ++y)
                {
                    uint8_t* p = m.ptr<uint8_t>(y) + x * 3;
                    p[0] = col.bi();
                    p[1] = col.gi();
                    p[2] = col.ri();
                }
            }

            return m;
        }


        const insertion_order_map<std::string, ImageBuffer>& _ColormapsImages()
        {
            static insertion_order_map<std::string, ImageBuffer> cache;
            if (cache.empty())
            {
                magic_enum::enum_for_each<ColormapType>([] (auto val) {
                    ColormapType type = val;
                    const char* name = magic_enum::enum_name(type).data();
                    cache.insert(name, MakeColormapImage(type));
                });
            }
            return cache;
        }


        static insertion_order_map<std::string, std::unique_ptr<GlTexture>> sColormapsTexturesCache;


        void FillTextureCache()
        {
            if (sColormapsTexturesCache.empty())
            {
                auto images = _ColormapsImages();
                for (const auto& k: images.insertion_order_keys())
                {
                    ImageBuffer& m = images.get(k);
                    auto texture = std::make_unique<GlTexture>(m, true);
                    sColormapsTexturesCache.insert(k, std::move(texture));
                }
            }
        }


        const insertion_order_map<std::string, ImTextureID>& ColormapsTextures()
        {
            FillTextureCache();

            static insertion_order_map<std::string, ImTextureID> cache;
            if (cache.empty())
            {
                for (const auto& k: sColormapsTexturesCache.insertion_order_keys())
                    cache.insert(k, sColormapsTexturesCache.get(k)->TextureId);
            }
            return cache;
        }


        void ClearColormapsTexturesCache()
        {
            sColormapsTexturesCache.clear();
        }



        //
        // Apply Colormap
        //

        struct Color4u8
        {
            uint8_t r, g, b, a;
        };

        template<typename _Tp>
        ImageBuffer _ApplyColormap(const ImageBuffer &m, const ColormapSettingsData& settings)
        {
            assert(CanColormap(m));

            std::string colormapName = settings.internal_ColormapHovered.empty() ? settings.Colormap : settings.internal_ColormapHovered;

            auto _colormapType = magic_enum::enum_cast<ColormapType>(colormapName);
            if (!_colormapType.has_value())
            {
                fprintf(stderr, "_ApplyColormap: bad colormap name: %s\n", settings.Colormap.c_str());
                assert(false);
            }
            auto colormapType = _colormapType.value();

            std::array<Color4u8, 256> colorLut;
            for (size_t i = 0; i < 256; ++i)
            {
                double x = (double) i / 255.;
                 auto c = tinycolormap::GetColor(x, colormapType);
                colorLut[i] = { c.ri(), c.gi(), c.bi(), 255 };
            }

            double minValue = settings.ColormapScaleMin;
            double maxValue = settings.ColormapScaleMax;
            auto fnGetColor = [&](_Tp value) -> Color4u8
            {
                double k = (value - minValue) / (maxValue - minValue);
                k = std::clamp(k, 0., 1.);
                size_t idx = (size_t)(k * 255.);
                return colorLut[idx];
            };

            ImageBuffer rgba = ImageBuffer::Zeros(m.width, m.height, 4, ImageDepth::uint8);
            for (int y = 0; y < m.height; ++y)
            {
                uint8_t *dst = rgba.ptr<uint8_t>(y);
                const _Tp* src = m.ptr<_Tp>(y);
                for (int x = 0; x < m.width; ++x)
                {
                    Color4u8 c = fnGetColor(*src);
                    dst[0] = c.r;
                    dst[1] = c.g;
                    dst[2] = c.b;
                    dst[3] = c.a;
                    dst += 4;
                    ++src;
                }
            }
            return rgba;
        }


        ImageBuffer ApplyColormap(const ImageBuffer &m, const ColormapSettingsData& settings)
        {
            switch (m.depth)
            {
                case ImageDepth::uint8:   return _ApplyColormap<uint8_t>(m, settings);
                case ImageDepth::int8:    return _ApplyColormap<int8_t>(m, settings);
                case ImageDepth::uint16:  return _ApplyColormap<uint16_t>(m, settings);
                case ImageDepth::int16:   return _ApplyColormap<int16_t>(m, settings);
                case ImageDepth::int32:   return _ApplyColormap<int32_t>(m, settings);
                case ImageDepth::float32: return _ApplyColormap<float>(m, settings);
                case ImageDepth::float64: return _ApplyColormap<double>(m, settings);
                default:
                {
                    assert(false);
                    throw std::runtime_error("ApplyColormap: bad depth");
                }
            }
        }


        //
        // Interactive update during pan and zoom
        //
        struct ImageStats
        {
            double mean, stdev;
            double min, max;
        };

        template<typename T>
        ImageStats FillImageStatsTyped(const ImageBuffer& m)
        {
            double minVal = std::numeric_limits<double>::max();
            double maxVal = std::numeric_limits<double>::lowest();
            double sum = 0.0;
            double sumSq = 0.0;
            size_t count = (size_t)m.height * m.width;

            for (int y = 0; y < m.height; y++)
            {
                const T* row = m.ptr<T>(y);
                for (int x = 0; x < m.width; x++)
                {
                    double v = (double)row[x];
                    if (v < minVal) minVal = v;
                    if (v > maxVal) maxVal = v;
                    sum += v;
                    sumSq += v * v;
                }
            }

            ImageStats r;
            r.min = minVal;
            r.max = maxVal;
            r.mean = sum / (double)count;
            double variance = sumSq / (double)count - r.mean * r.mean;
            r.stdev = (variance > 0.0) ? std::sqrt(variance) : 0.0;
            return r;
        }

        ImageStats FillImageStats(const ImageBuffer& m)
        {
            assert(m.channels == 1);
            if (m.empty())
                return {0.0, 0.0, 0.0, 0.0};

            switch (m.depth)
            {
                case ImageDepth::uint8:   return FillImageStatsTyped<uint8_t>(m);
                case ImageDepth::int8:    return FillImageStatsTyped<int8_t>(m);
                case ImageDepth::uint16:  return FillImageStatsTyped<uint16_t>(m);
                case ImageDepth::int16:   return FillImageStatsTyped<int16_t>(m);
                case ImageDepth::int32:   return FillImageStatsTyped<int32_t>(m);
                case ImageDepth::float32: return FillImageStatsTyped<float>(m);
                case ImageDepth::float64: return FillImageStatsTyped<double>(m);
                default:                  return {0.0, 0.0, 0.0, 0.0};
            }
        }




        void ApplyColormapStatsToMinMax(const ImageBuffer& m, std::optional<Rect> roi, ColormapSettingsData* inout_settings)
        {
            bool isRoi = roi.has_value();

            ImageStats imageStats;
            if (isRoi)
                imageStats = FillImageStats(m.subImage(roi.value()));
            else
                imageStats = FillImageStats(m);

            if (inout_settings->ColormapScaleFromStats.UseStatsMin)
                inout_settings->ColormapScaleMin = imageStats.min;
            else
                inout_settings->ColormapScaleMin =
                    imageStats.mean - (double) inout_settings->ColormapScaleFromStats.NbSigmas * imageStats.stdev;

            if (inout_settings->ColormapScaleFromStats.UseStatsMax)
                inout_settings->ColormapScaleMax = imageStats.max;
            else
                inout_settings->ColormapScaleMax =
                    imageStats.mean + (double) inout_settings->ColormapScaleFromStats.NbSigmas * imageStats.stdev;
        }


        void UpdateRoiStatsInteractively(
            const ImageBuffer &image,
            const Rect& roi,
            ColormapSettingsData* inout_settings)
        {
            if (image.channels != 1)
                return;

            if(roi.empty())
                return;

            if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI)
                ApplyColormapStatsToMinMax(image, roi, inout_settings);
        }


        void InitStatsOnNewImage(
            const ImageBuffer &image,
            const Rect& roi,
            ColormapSettingsData* inout_settings)
        {
            if (image.channels != 1)
                return;

            if (roi.empty())
                return;

            if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI)
                ApplyColormapStatsToMinMax(image, roi, inout_settings);
            else if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromFullImage)
                ApplyColormapStatsToMinMax(image, std::nullopt, inout_settings);
        }


        //
        // GUI
        //
        void GuiChooseColormap(ColormapSettingsData* inout_params)
        {
            static std::optional<std::string> lastUnselectedColormap;
            FillTextureCache();

            inout_params->internal_ColormapHovered = "";
            for (const auto& kv: sColormapsTexturesCache.items())
            {
                std::string colormapName = kv.first;
                bool wasSelected = (colormapName == inout_params->Colormap);

                ImVec4 colorNormal(0.7f, 0.7f, 0.7f, 1.f);
                ImVec4 colorSelected(1.f, 1.f, 0.2f, 1.f);
                ImVec4 colorHovered = colorSelected;
                colorHovered.w = 0.65f;

                float kFont = ImGui::GetFontSize();
                float widthText = kFont * 5.5f;
                ImVec2 sizeTexture(kFont * 12.f, kFont * 0.8f);

                bool isHovered;
                {
                    auto posWidget = ImGui::GetCursorScreenPos();
                    auto posMouse = ImGui::GetMousePos();
                    ImRect bounding(posWidget, posWidget + ImVec2(sizeTexture.x + widthText, 15.f));
                    isHovered = bounding.Contains(posMouse);
                }

                ImVec4 color;
                if (wasSelected)
                    color = colorSelected;
                else if (isHovered)
                    color = colorHovered;
                else
                    color = colorNormal;

                auto pos = ImGui::GetCursorPos();
                ImGui::TextColored(color, "%s", colormapName.c_str());
                pos.x += widthText;
                ImGui::SetCursorPos(pos);
                if (wasSelected)
                    GlTexture_DrawButton(*kv.second, sizeTexture);
                else
                    GlTexture_DrawButton(*kv.second, sizeTexture);
                if (ImGui::IsItemHovered())
                {
                    if (!lastUnselectedColormap.has_value())
                        inout_params->internal_ColormapHovered = colormapName;
                    if (lastUnselectedColormap.has_value() && (*lastUnselectedColormap != colormapName))
                        inout_params->internal_ColormapHovered = colormapName;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                {
                    if (wasSelected)
                    {
                        inout_params->Colormap = "None";
                        lastUnselectedColormap = colormapName;
                    }
                    else
                    {
                        inout_params->Colormap = colormapName;
                        lastUnselectedColormap = std::nullopt;
                    }
                }
            }
        }


        void DrawColorTabsSubtitles(const std::string &title, float availableGuiWidth)
        {
            ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            ImVec4 backColor = ImGui::GetStyleColorVec4(ImGuiCol_TabSelected);
            backColor.w = 0.3f;

            // background rect
            {
                ImVec2 tl = ImGui::GetCursorScreenPos();
                ImVec2 br = tl;
                br.x += availableGuiWidth - 10.f;
                br.y += ImGui::GetFontSize() + 2.f;
                ImU32 col = ImGui::GetColorU32(backColor);
                float rounding = 4.f;
                ImGui::GetWindowDrawList()->AddRectFilled(tl, br, col, rounding);
            }
            std::string fullTitle = std::string("          Colormap Scale ") + title;

            ImGui::TextColored(textColor, "%s", fullTitle.c_str());
        }


        void GuiImageStats(const ImageBuffer& m, std::optional<Rect> roi, ColormapSettingsData* inout_settings, float availableGuiWidth)
        {
            float em = ImGui::GetFontSize();

            ImageStats imageStats;
            bool isRoi = roi.has_value();
            if (isRoi)
            {
                imageStats = FillImageStats(m.subImage(roi.value()));
                ImGui::PushID("ROI");
            }
            else
            {
                imageStats = FillImageStats(m);
                ImGui::PushID("Full");
            }

            ImGui::Text("Stats:");
            ImGui::Text("min=%.3lf max=%.3lf", imageStats.min, imageStats.max);
            ImGui::Text("mean=%3lf stdev=%3lf", imageStats.mean, imageStats.stdev);

            bool changed = false;

            {
                std::string help = "The number of sigmas will impact\n"
                                   "how the colormap scale min and max are calculated from\n"
                                   "the mean and standard deviation";
                changed |= ImGuiImm::SliderAnyFloat(
                    "Nb of sigmas",
                    &inout_settings->ColormapScaleFromStats.NbSigmas,
                    0., 8.,
                    em * 10.f);
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", help.c_str());
            }

            {
                std::string help = "If \"Use stats min\" is checked, then the scale\n"
                                   "will be calculated from the image minimum value \n"
                                   "instead of a sigma based value\n"
                                   "\n"
                                   "(same for max values)";
                changed |= ImGui::Checkbox("Use stats min", &inout_settings->ColormapScaleFromStats.UseStatsMin);
                ImGui::SameLine();
                changed |= ImGui::Checkbox("Use stats max", &inout_settings->ColormapScaleFromStats.UseStatsMax);
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", help.c_str());
            }

            if (changed)
                ApplyColormapStatsToMinMax(m, roi, inout_settings);

            {
                ImGui::Text("Colormap scale");
                ImGuiImm::SliderAnyFloatLogarithmic("Min", &inout_settings->ColormapScaleMin, -255., 255.,
                                                    em * 6.f);
                ImGui::SameLine();
                ImGuiImm::SliderAnyFloatLogarithmic("Max", &inout_settings->ColormapScaleMax, -255., 255.,
                                                    em * 6.f);
            }

            ImGui::PopID();
        }


        void GuiShowColormapSettingsData(
            const ImageBuffer &image,
            const Rect& roi,
            float availableGuiWidth,
            ColormapSettingsData* inout_settings
            )
        {
            GuiChooseColormap(inout_settings);

            {
                ImGui::NewLine();
                ImGui::Text("Colormap stats ROI:");
                if (ImGui::RadioButton(
                    "full image",
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromFullImage))
                {
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType = ColorMapStatsTypeId::FromFullImage;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton(
                    "visible ROI",
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI))
                {
                    inout_settings->ColormapScaleFromStats.ColorMapStatsType = ColorMapStatsTypeId::FromVisibleROI;
                }
                std::string helpRoi = "Warning, if using \"number of sigmas\" on a ROI\n"
                                      "the colormap scale will vary immediately\n"
                                      "whenever you zoom in/out or pan";
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", helpRoi.c_str());
            }

            std::optional<Rect> optionalRoi;
            if (inout_settings->ColormapScaleFromStats.ColorMapStatsType == ColorMapStatsTypeId::FromVisibleROI)
                optionalRoi = roi;
            GuiImageStats(image, optionalRoi, inout_settings, availableGuiWidth);

            // ImGuiImm::SeparatorFixedWidth(availableGuiWidth);

        }


    } // namespace Colormap
} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/drawing_utils.cpp                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/drawing_utils.h included by src/immvision/internal/cv/drawing_utils.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/drawing_utils.cpp continued                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/string_utils.h included by src/immvision/internal/cv/drawing_utils.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef IMMVISION_HAS_OPENCV
#endif

namespace ImmVision
{
    namespace StringUtils
    {
        std::string LowerString(const std::string& s);
        std::string JoinStrings(const std::vector<std::string>&v, const std::string& separator);
        std::vector<std::string> SplitString(const std::string& s, const char separator);
        std::string IndentLine(const std::string& s, int indentSize);
        std::string IndentLines(const std::string& s, int indentSize);


        std::string ToString(const std::string& s);
        std::string ToString(const double& v);
        std::string ToString(const float& v);
        std::string ToString(const int& v);
        std::string ToString(bool v);

#ifdef IMMVISION_HAS_OPENCV
        template<typename _Tp>
        std::string ToString(const cv::Point_<_Tp>& v)
        {
            return std::string("(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        }
        template<typename _Tp>
        std::string ToString(const cv::Size_<_Tp>& v)
        {
            return std::string("(") + std::to_string(v.width) + " x " + std::to_string(v.height) + ")";
        }
#endif

        // Overloads for ImmVision types (must be before the vector template so they're found)
        inline std::string ToString(const Point& v)
        {
            return std::string("(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        }
        inline std::string ToString(const Point2d& v)
        {
            return std::string("(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        }
        inline std::string ToString(const Size& v)
        {
            return std::string("(") + std::to_string(v.width) + " x " + std::to_string(v.height) + ")";
        }

        template<typename _Tp>
        std::string ToString(const std::vector<_Tp>& v)
        {
            std::vector<std::string> strs;
            for (const auto& x : v)
                strs.push_back(ToString(x));
            std::string r = "[" + JoinStrings(strs, ", ") + "]";
            return r;
        }

#ifdef IMMVISION_HAS_OPENCV
        template<typename _Tp, int _rows,int _cols>
        std::string ToString(const cv::Matx<_Tp, _rows, _cols>& m)
        {
            std::vector<std::string> lines;
            for (int i = 0; i < _rows; ++i)
            {
                std::vector<_Tp> lineValues;
                for (int j = 0; j < _cols; ++j)
                    lineValues.push_back(m(i, j));

                std::string lineString = ToString(lineValues);
                if (i != 0)
                    lineString = std::string("   ") + lineString;
                lines.push_back(lineString);
            }
            std::string r = "\n  [";
            r += JoinStrings(lines, ",\n");
            r += "]";
            return r;
        }
#endif

        inline std::string ToString(const Matrix33d& m)
        {
            std::vector<std::string> lines;
            for (int i = 0; i < 3; ++i)
            {
                std::vector<double> lineValues;
                for (int j = 0; j < 3; ++j)
                    lineValues.push_back(m.m[i][j]);
                std::string lineString = ToString(lineValues);
                if (i != 0)
                    lineString = std::string("   ") + lineString;
                lines.push_back(lineString);
            }
            std::string r = "\n  [";
            r += JoinStrings(lines, ",\n");
            r += "]";
            return r;
        }

    } // namespace StringUtils
} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/drawing_utils.cpp continued                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <type_traits>  // std::is_same_v
#include <limits>       // std::numeric_limits
#include <cmath>        // std::sqrt, std::clamp


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

            for (int y = 0; y < result.height; y++)
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
            }

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
            for (int y = 0; y < mat.height; y++)
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
            }
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/matrix_info_utils.cpp                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/matrix_info_utils.h included by src/immvision/internal/cv/matrix_info_utils.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const ImageBuffer& m);
        std::string _MatInfo(const ImageBuffer &m);
        std::vector<double> MatValuesAt(const ImageBuffer& m, int x, int y);
        std::string MatPixelColorInfo(const ImageBuffer & m, int x, int y, char separator = ',', bool add_paren = true);

    } // namespace MatrixInfoUtils

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/matrix_info_utils.cpp continued                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const ImageBuffer& m)
        {
            return ImageDepthName(m.depth);
        }

        std::string _MatInfo(const ImageBuffer &m)
        {
            return ImageDepthName(m.depth) + " " + std::to_string(m.width) + "x" + std::to_string(m.height);
        }

        std::string JoinStrings(const std::vector<std::string>&v, char separator)
        {
            std::string r;
            for (size_t i = 0; i < v.size(); ++ i)
            {
                r += v[i];
                if (i < v.size() - 1)
                    r += separator;
            }
            return r;
        }

        template<typename _Tp>
        std::vector<double> GrabValuesFromBuffer(const uint8_t * buffer, int nb)
        {
            std::vector<double> r;
            auto buffer_typed =  reinterpret_cast<const _Tp *>(buffer);
            for (int i = 0; i < nb; ++i)
            {
                r.push_back(static_cast<double>(*buffer_typed));
                ++buffer_typed;
            }
            return r;
        }

        std::vector<double> MatValuesAt(const ImageBuffer& m, int x, int y)
        {
            int nb_channels = m.channels;
            const uint8_t * ptr = m.ptr<uint8_t>(y) + x * m.elemSizeTotal();
            switch (m.depth)
            {
                case ImageDepth::uint8:
                    return GrabValuesFromBuffer<uint8_t>(ptr, nb_channels);
                case ImageDepth::int8:
                    return GrabValuesFromBuffer<int8_t>(ptr, nb_channels);
                case ImageDepth::uint16:
                    return GrabValuesFromBuffer<uint16_t>(ptr, nb_channels);
                case ImageDepth::int16:
                    return GrabValuesFromBuffer<int16_t>(ptr, nb_channels);
                case ImageDepth::int32:
                    return GrabValuesFromBuffer<int32_t>(ptr, nb_channels);
                case ImageDepth::float32:
                    return GrabValuesFromBuffer<float>(ptr, nb_channels);
                case ImageDepth::float64:
                    return GrabValuesFromBuffer<double>(ptr, nb_channels);
                default:
                    throw std::runtime_error("MatValuesAt: unhandled depth");
            }
        }

        std::string MatPixelColorInfo(const ImageBuffer & m, int x, int y, char separator, bool add_paren)
        {
            if (!Rect(Point(0, 0), m.size()).contains(Point(x, y)))
                return "";
            std::vector<double> values = MatValuesAt(m, x, y);

            auto formatValue = [](double v, ImageDepth depth) -> std::string
            {
                bool isFloat = ImageDepthIsFloat(depth);
                if (isFloat)
                {
                    char buffer_color[300];
                    snprintf(buffer_color, 300, "%.5G", (double) v);
                    return std::string(buffer_color);
                }
                else
                {
                    char buffer_color[300];
                    snprintf(buffer_color, 300, "%lld", (long long) v);
                    return std::string(buffer_color);
                }
            };

            std::vector<std::string> strs;
            for (double v: values)
                strs.push_back(formatValue(v, m.depth));

            std::string r = JoinStrings(strs, separator);
            if (add_paren)
                r = std::string("(") + r + ")";
            return r;
        }

    } // namespace MatrixInfoUtils

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/zoom_pan_transform.cpp                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/zoom_pan_transform.h included by src/immvision/internal/cv/zoom_pan_transform.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = Matrix33d;

        MatrixType Identity();

        MatrixType ComputeZoomMatrix(const Point2d & zoomCenter, double zoomRatio);
        MatrixType ComputePanMatrix(const Point2d& dragDelta, double currentZoom);
        MatrixType MakeScaleOne(Size imageSize, Size viewportSize);
        MatrixType MakeFullView(Size imageSize, Size viewportSize);
        Matrix33d MakeZoomMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize);

        bool IsEqual(const MatrixType & v1, const MatrixType & v2);

        Point2d Apply(const MatrixType& zoomMatrix, const Point2d &p);

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const Size& oldDisplaySize, const Size& newDisplaySize);

        Rect VisibleRoi(const MatrixType & zoomMatrix,
                            Size imageDisplaySize,
                            Size originalImageSize
                            );

        // Custom warp for scale+translate transforms (replaces cv::warpAffine)
        enum class WarpInterp { Nearest, Bilinear, Area };
        void WarpAffineScaleTranslate(const ImageBuffer& src, ImageBuffer& dst, const Matrix33d& m, WarpInterp interp);
        void _WarpAffineInterAreaForSmallSizes(const ImageBuffer& src, ImageBuffer& dst, const Matrix33d& m);

} // namespace ZoomPanTransform

    Matrix33d MakeZoomPanMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv/zoom_pan_transform.cpp continued                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = Matrix33d;

        MatrixType Identity()
        {
            return Matrix33d::eye();
        }

        MatrixType ComputeZoomMatrix(const Point2d & zoomCenter, double zoomRatio)
        {
            auto mat = Matrix33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            mat(0, 2) = zoomCenter.x * (1. - zoomRatio);
            mat(1, 2) = zoomCenter.y * (1. - zoomRatio);
            return mat;
        }

        MatrixType ComputePanMatrix(const Point2d& dragDelta, double currentZoom)
        {
            auto mat = Matrix33d::eye();
            mat(0, 2) = (double)dragDelta.x / currentZoom;
            mat(1, 2) = (double)dragDelta.y / currentZoom;
            return mat;
        }

        MatrixType MakeScaleOne(Size imageSize, Size viewportSize)
        {
            MatrixType r = Identity();
            r(0, 2) = (viewportSize.width / 2 - imageSize.width / 2);
            r(1, 2) = (viewportSize.height / 2 - imageSize.height / 2);
            return r;
        }

        MatrixType MakeFullView(Size imageSize, Size viewportSize)
        {
            assert(imageSize.area() > 0);
            assert(viewportSize.area() >0);

            MatrixType r = Identity();

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

        bool IsEqual(const MatrixType & v1, const MatrixType & v2)
        {
            for (int j = 0; j < 3; j++)
                for (int i = 0; i < 3; i++)
                    if (fabs(v2(j, i) - v1(j, i)) > 1E-6)
                        return false;
            return true;
        }

        Point2d Apply(const MatrixType& zoomMatrix, const Point2d &p)
        {
            return zoomMatrix.apply(p);
        }

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const Size& oldDisplaySize, const Size& newDisplaySize)
        {
            if (oldDisplaySize.area() == 0 || newDisplaySize.area() == 0)
                return oldZoomMatrix;

            MatrixType zoomMatrix;

            auto fnImageCenter = [](const Size s) {
                return Point2d((double)s.width / 2., (double)s.height / 2.);
            };

            double newZoomFactor;
            {
                double oldZoomFactor = oldZoomMatrix(0, 0);
                double kx = (double)newDisplaySize.width / (double)oldDisplaySize.width;
                double ky = (double)newDisplaySize.height / (double)oldDisplaySize.height;
                double k = (kx + ky) / 2.;
                newZoomFactor = oldZoomFactor * k;
            }

            zoomMatrix = MatrixType::eye();
            zoomMatrix(0, 0) = zoomMatrix(1, 1) = newZoomFactor;

            Point2d translation;
            {
                Point2d oldDisplayCenter_Zoomed = fnImageCenter(oldDisplaySize);
                Point2d oldDisplayCenter_Image = ZoomPanTransform::Apply(oldZoomMatrix.inv(), oldDisplayCenter_Zoomed);

                Point2d newDisplayCenter_Zoomed_Wanted = fnImageCenter(newDisplaySize);
                Point2d newDisplayCenter_Zoomed_Now = ZoomPanTransform::Apply(zoomMatrix, oldDisplayCenter_Image);
                translation = newDisplayCenter_Zoomed_Wanted - newDisplayCenter_Zoomed_Now;
            }

            zoomMatrix(0, 2) = translation.x;
            zoomMatrix(1, 2) = translation.y;

            return zoomMatrix;
        }

        Matrix33d MakeZoomMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize)
        {
            auto mat = Matrix33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            double dx = (double)displayedImageSize.width / 2. - zoomRatio * zoomCenter.x;
            double dy = (double)displayedImageSize.height / 2. - zoomRatio * zoomCenter.y;
            mat(0, 2) = dx;
            mat(1, 2) = dy;
            return mat;
        }

        Rect VisibleRoi(const MatrixType & zoomMatrix, Size imageDisplaySize, Size originalImageSize)
        {
            Rect roi;
            {
                Point2d tl = ZoomPanTransform::Apply(zoomMatrix.inv(), Point2d(0., 0.));
                Point tli(MathUtils::RoundInt(tl.x), MathUtils::RoundInt(tl.y));
                tli.x = std::clamp(tli.x, 0, originalImageSize.width - 1);
                tli.y = std::clamp(tli.y, 0, originalImageSize.height - 1);

                Point2d br = ZoomPanTransform::Apply(zoomMatrix.inv(), Point2d(
                    (double)imageDisplaySize.width, (double)imageDisplaySize.height));
                Point bri(MathUtils::RoundInt(br.x), MathUtils::RoundInt(br.y));
                bri.x = std::clamp(bri.x, 0, originalImageSize.width);
                bri.y = std::clamp(bri.y, 0, originalImageSize.height);

                //                bri.x += 1;
//                bri.y += 1;
                roi = Rect(tli, bri);
            }
            return roi;
        }

        // =====================================================================
        // Custom warp: scale + translate only (no rotation/shear)
        // Replaces cv::warpAffine and cv::resize for immvision's zoom/pan
        // =====================================================================

        // Sample a single pixel from src with bilinear interpolation
        inline void _SampleBilinear(const ImageBuffer& src, double sx, double sy, uint8_t* out, int ch)
        {
            int x0 = (int)std::floor(sx), y0 = (int)std::floor(sy);
            int x1 = x0 + 1, y1 = y0 + 1;
            double fx = sx - x0, fy = sy - y0;

            // Clamp to image bounds
            x0 = std::clamp(x0, 0, src.width - 1);
            x1 = std::clamp(x1, 0, src.width - 1);
            y0 = std::clamp(y0, 0, src.height - 1);
            y1 = std::clamp(y1, 0, src.height - 1);

            const uint8_t* p00 = src.ptr<uint8_t>(y0) + x0 * ch;
            const uint8_t* p10 = src.ptr<uint8_t>(y0) + x1 * ch;
            const uint8_t* p01 = src.ptr<uint8_t>(y1) + x0 * ch;
            const uint8_t* p11 = src.ptr<uint8_t>(y1) + x1 * ch;

            double w00 = (1 - fx) * (1 - fy), w10 = fx * (1 - fy);
            double w01 = (1 - fx) * fy, w11 = fx * fy;

            for (int c = 0; c < ch; c++)
                out[c] = (uint8_t)std::clamp(p00[c] * w00 + p10[c] * w10 + p01[c] * w01 + p11[c] * w11, 0.0, 255.0);
        }

        // Area downscale with proper fractional pixel weighting.
        // Each destination pixel covers a (scaleInv x scaleInv) region in the source.
        // Pixels at the edges contribute proportionally to their overlap.
        inline void _SampleArea(const ImageBuffer& src, double sx, double sy, double scaleInv, uint8_t* out, int ch)
        {
            double sx0 = sx, sy0 = sy;
            double sx1 = sx + scaleInv, sy1 = sy + scaleInv;

            // Clamp to source bounds
            sx0 = std::max(sx0, 0.0);
            sy0 = std::max(sy0, 0.0);
            sx1 = std::min(sx1, (double)src.width);
            sy1 = std::min(sy1, (double)src.height);

            int ix0 = (int)std::floor(sx0);
            int iy0 = (int)std::floor(sy0);
            int ix1 = std::min((int)std::floor(sx1), src.width - 1);
            int iy1 = std::min((int)std::floor(sy1), src.height - 1);

            double sums[4] = {0, 0, 0, 0};
            double totalWeight = 0;
            for (int y = iy0; y <= iy1; y++)
            {
                // Vertical weight: fractional overlap of this row with [sy0, sy1]
                double wy = std::min((double)(y + 1), sy1) - std::max((double)y, sy0);
                if (wy <= 0) continue;

                const uint8_t* row = src.ptr<uint8_t>(y);
                for (int x = ix0; x <= ix1; x++)
                {
                    // Horizontal weight: fractional overlap of this column with [sx0, sx1]
                    double wx = std::min((double)(x + 1), sx1) - std::max((double)x, sx0);
                    if (wx <= 0) continue;

                    double w = wx * wy;
                    for (int c = 0; c < ch; c++)
                        sums[c] += row[x * ch + c] * w;
                    totalWeight += w;
                }
            }
            if (totalWeight > 0)
                for (int c = 0; c < ch; c++)
                    out[c] = (uint8_t)std::clamp(sums[c] / totalWeight, 0.0, 255.0);
            else
                for (int c = 0; c < ch; c++)
                    out[c] = 0;
        }

        // Custom warp affine for scale+translate transforms on uint8 images.
        // m is a 3x3 homogeneous matrix (forward: src->dst).
        // dst must be pre-allocated. Pixels outside src bounds are left unchanged (transparent border).
        void WarpAffineScaleTranslate(const ImageBuffer& src, ImageBuffer& dst, const Matrix33d& m, WarpInterp interp)
        {
            assert(src.depth == ImageDepth::uint8);
            int ch = src.channels;
            Matrix33d mInv = m.inv();

            double scale = m(0, 0); // uniform scale factor
            double scaleInv = 1.0 / std::max(scale, 1e-10);

            for (int dy = 0; dy < dst.height; dy++)
            {
                uint8_t* dstRow = dst.ptr<uint8_t>(dy);
                for (int dx = 0; dx < dst.width; dx++)
                {
                    // Map destination pixel to source coordinates
                    double sx = mInv(0, 0) * dx + mInv(0, 1) * dy + mInv(0, 2);
                    double sy = mInv(1, 0) * dx + mInv(1, 1) * dy + mInv(1, 2);

                    // Bounds check
                    if (sx < -0.5 || sx >= src.width || sy < -0.5 || sy >= src.height)
                        continue; // leave dst pixel unchanged (transparent border)

                    uint8_t* out = dstRow + dx * ch;
                    if (interp == WarpInterp::Nearest)
                    {
                        int ix = std::clamp((int)std::round(sx), 0, src.width - 1);
                        int iy = std::clamp((int)std::round(sy), 0, src.height - 1);
                        const uint8_t* p = src.ptr<uint8_t>(iy) + ix * ch;
                        for (int c = 0; c < ch; c++)
                            out[c] = p[c];
                    }
                    else if (interp == WarpInterp::Bilinear)
                    {
                        _SampleBilinear(src, sx, sy, out, ch);
                    }
                    else // Area
                    {
                        _SampleArea(src, sx, sy, scaleInv, out, ch);
                    }
                }
            }
        }

        void _WarpAffineInterAreaForSmallSizes(const ImageBuffer& src, ImageBuffer& dst, const Matrix33d& m)
        {
            WarpAffineScaleTranslate(src, dst, m, WarpInterp::Area);
        }


    } // namespace ZoomPanTransform

    Matrix33d MakeZoomPanMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize)
    {
        return ZoomPanTransform::MakeZoomMatrix(zoomCenter, zoomRatio, displayedImageSize);
    }

    Matrix33d MakeZoomPanMatrix_ScaleOne(
        Size imageSize,
        const Size displayedImageSize)
    {
        return ZoomPanTransform::MakeScaleOne(imageSize, displayedImageSize);
    }

    Matrix33d MakeZoomPanMatrix_FullView(
        Size imageSize,
        const Size displayedImageSize)
    {
        return ZoomPanTransform::MakeFullView(imageSize, displayedImageSize);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/drawing/image_drawing.cpp                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/drawing/image_drawing.h included by src/immvision/internal/drawing/image_drawing.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace ImageDrawing
    {
        ImageBuffer DrawWatchedPixels(const ImageBuffer& image, const ImageParams& params);

        void DrawGrid(ImageBuffer& inOutImageRgba, const ImageParams& params);

        ImageBuffer DrawValuesOnZoomedPixels(const ImageBuffer& drawingImage, const ImageBuffer& valuesImage,
                                         const ImageParams& params, bool drawPixelCoords);

        ImageBuffer MakeSchoolPaperBackground(Size s);

        void BlitImageTexture(
            const ImageParams& params,
            const ImageBuffer& image,
            ImageBuffer& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTexture* outTexture
        );

        bool HasColormapParam(const ImageParams& params);

    } // namespace ImageDrawing

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/drawing/image_drawing.cpp continued                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstring>  // memcpy


namespace ImmVision
{
    namespace ImageDrawing
    {
        static float _ImmDrawingFontScaleRatio()
        {
            if (ImGui::GetFontSize() < 20.f)
                return 1.f;
            else
                return 1.5f;
        }

        ImageBuffer DrawWatchedPixels(const ImageBuffer& image, const ImageParams& params)
        {
            ImageBuffer r = image.clone();

            std::vector<std::pair<size_t, Point2d>> visiblePixels;
            {
                for (size_t i = 0; i < params.WatchedPixels.size(); ++i)
                {
                    Point w = params.WatchedPixels[i];
                    Point2d p = ZoomPanTransform::Apply(params.ZoomPanMatrix, Point2d(w));
                    if (Rect(Point(0, 0), params.ImageDisplaySize).contains(Point((int)p.x, (int)p.y)))
                        visiblePixels.push_back({i, p});
                }
            }

            for (const auto& kv : visiblePixels)
            {
                DrawingUtils::draw_named_feature(
                    r,         // img
                    kv.second, // position,
                    std::to_string(kv.first),       // name
                    Color4d(255, 255, 255, 255), // color
                    true, // add_cartouche
                    4.,   // size
                    2.5,  // size_hole
                    1,    // thickness
                    0.4 * (double) _ImmDrawingFontScaleRatio() // font size
                );
            }

            return r;
        }

        void DrawGrid(ImageBuffer& inOutImageRgba, const ImageParams& params)
        {
            double x_spacing = (double) params.ZoomPanMatrix(0, 0);
            double y_spacing = (double) params.ZoomPanMatrix(1, 1);

            double x_start, y_start;
            {
                Point2d origin_unzoomed = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(), Point2d(0., 0.));
                origin_unzoomed = Point2d(std::floor(origin_unzoomed.x) + 0.5, std::floor(origin_unzoomed.y) + 0.5);
                Point2d origin_zoomed = ZoomPanTransform::Apply(params.ZoomPanMatrix, origin_unzoomed);
                x_start = origin_zoomed.x;
                y_start = origin_zoomed.y;
            }
            double x_end = (double)inOutImageRgba.width - 1.;
            double y_end = (double)inOutImageRgba.height - 1.;

            auto lineColor = Color4d(255, 255, 0, 255);
            double alpha = 0.3;
            DrawingUtils::draw_grid(inOutImageRgba, lineColor, alpha, x_spacing, y_spacing, x_start, y_start, x_end, y_end);
        }

        ImageBuffer DrawValuesOnZoomedPixels(const ImageBuffer& drawingImage, const ImageBuffer& valuesImage,
                                         const ImageParams& params, bool drawPixelCoords)
        {
            assert(drawingImage.depth == ImageDepth::uint8 && drawingImage.channels == 4);

            ImageBuffer r = drawingImage;
            Point tl, br;
            {
                Point2d tld = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(), Point2d(0., 0.));
                Point2d brd = ZoomPanTransform::Apply(params.ZoomPanMatrix.inv(),
                                                          Point2d((double)params.ImageDisplaySize.width,
                                                                      (double)params.ImageDisplaySize.height));
                tl = { (int)std::floor(tld.x), (int)std::floor(tld.y) };
                br = { (int)std::ceil(brd.x), (int)std::ceil(brd.y) };
            }

            for (int x = tl.x; x <= br.x; x+= 1)
            {
                for (int y = tl.y; y <= br.y; y+= 1)
                {
                    std::string pixelInfo = MatrixInfoUtils::MatPixelColorInfo(valuesImage, x, y, '\n', false);
                    if (drawPixelCoords)
                        pixelInfo = std::string("x:") + std::to_string(x) + "\n" + "y:" + std::to_string(y) + "\n" + pixelInfo;

                    Point2d position = ZoomPanTransform::Apply(params.ZoomPanMatrix, Point2d((double)x, (double )y));

                    Color4d textColor;
                    {
                        Color4d white(255, 255, 255, 255);
                        Color4d black(0, 0, 0, 255);
                        uint8_t bgR = 0, bgG = 0, bgB = 0;
                        if (Rect(Point(0, 0), drawingImage.size()).contains(Point((int)position.x, (int)position.y)))
                        {
                            const uint8_t* bgPixel = drawingImage.ptr<uint8_t>((int)position.y) + (int)position.x * 4;
                            bgR = bgPixel[0]; bgG = bgPixel[1]; bgB = bgPixel[2];
                        }
                        double luminance = bgR * 0.2126 + bgG * 0.7152 + bgB * 0.0722;
                        if (luminance > 170.)
                            textColor = black;
                        else
                            textColor = white;
                    }
                    DrawingUtils::text(
                        r,
                        position,
                        pixelInfo,
                        textColor,
                        true, // center_around_point
                        false, // add_cartouche
                        0.42 * (double) _ImmDrawingFontScaleRatio() ,  //fontScale
                        1     //int thickness
                    );
                }
            }
            return r;
        };


        ImageBuffer MakeSchoolPaperBackground(Size s)
        {
            ImageBuffer mat = ImageBuffer::Zeros(s.width, s.height, 4, ImageDepth::uint8);

            Color4d paperColor(205, 215, 220, 255);
            Color4d lineColor(199, 196, 184, 255);
            mat.fill(paperColor);
            int quadSize = 17;
            for (int y = 0; y < s.height; y+= quadSize)
            {
                uint8_t* linePtr = mat.ptr<uint8_t>(y);
                for (int x = 0; x < s.width; ++x)
                {
                    uint8_t* p = linePtr + x * 4;
                    p[0] = (uint8_t)lineColor[0]; p[1] = (uint8_t)lineColor[1];
                    p[2] = (uint8_t)lineColor[2]; p[3] = (uint8_t)lineColor[3];
                }
            }
            for (int y = 0; y < s.height; y++)
            {
                uint8_t* linePtr = mat.ptr<uint8_t>(y);
                for (int x = 0; x < s.width; x+=quadSize)
                {
                    uint8_t* p = linePtr + x * 4;
                    p[0] = (uint8_t)lineColor[0]; p[1] = (uint8_t)lineColor[1];
                    p[2] = (uint8_t)lineColor[2]; p[3] = (uint8_t)lineColor[3];
                }
            }
            return mat;
        }

        void BlitImageTexture(
            const ImageParams& params,
            const ImageBuffer& image,
            ImageBuffer& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTexture* outTexture
        )
        {
            if (image.empty())
                return;

            ImageBuffer finalImage = image.clone();

            //
            // Adjustements needed before conversion to rgba
            //
            auto fnSelectChannel = [&finalImage, params]()
            {
                // Extract a single channel from a multi-channel image
                if (finalImage.channels > 1 && (params.SelectedChannel >= 0) && (params.SelectedChannel < finalImage.channels))
                {
                    int ch = params.SelectedChannel;
                    int nch = finalImage.channels;
                    ImageBuffer singleChannel = ImageBuffer::Zeros(finalImage.width, finalImage.height, 1, finalImage.depth);
                    for (int y = 0; y < finalImage.height; y++)
                    {
                        const uint8_t* src = finalImage.ptr<uint8_t>(y);
                        uint8_t* dst = singleChannel.ptr<uint8_t>(y);
                        int elemSize = (int)finalImage.elemSize();
                        for (int x = 0; x < finalImage.width; x++)
                            std::memcpy(dst + x * elemSize, src + (x * nch + ch) * elemSize, elemSize);
                    }
                    finalImage = singleChannel;
                }

            };
            auto fnAlphaCheckerboard = [&finalImage, params]()
            {
                if ((finalImage.depth == ImageDepth::uint8 && finalImage.channels == 4) && params.ShowAlphaChannelCheckerboard)
                {
                    ImageBuffer background = DrawingUtils::make_alpha_channel_checkerboard_image(finalImage.size());
                    finalImage = DrawingUtils::overlay_alpha_image_precise(background, finalImage, 1.);
                }
            };
            auto fnMakeBackground = [&params]() -> ImageBuffer
            {
                if (params.ShowSchoolPaperBackground)
                    return MakeSchoolPaperBackground(params.ImageDisplaySize);
                else
                {
                    ImageBuffer m = ImageBuffer::Zeros(params.ImageDisplaySize.width, params.ImageDisplaySize.height, 4, ImageDepth::uint8);
                    m.fill(Color4d(0, 0, 0, 255));
                    return m;
                }

            };

            //
            // Convert to rgba with adjustments if needed
            //
            if (shall_refresh_rgba)
            {
                if (HasColormapParam(params) && Colormap::CanColormap(image))
                    finalImage = Colormap::ApplyColormap(finalImage, params.ColormapSettings); // returns a rgba image
                else
                {
                    fnSelectChannel();
                    fnAlphaCheckerboard();
                    bool is_color_order_bgr = IsUsingBgrColorOrder();
                    finalImage = DrawingUtils::converted_to_rgba_image(finalImage, is_color_order_bgr);
                }
                in_out_rgba_image_cache = finalImage;
                assert(finalImage.depth == ImageDepth::uint8 && finalImage.channels == 4);
            }
            else
            {
                finalImage = in_out_rgba_image_cache;
                assert(finalImage.depth == ImageDepth::uint8 && finalImage.channels == 4);
                assert(!finalImage.empty());
            }

            double gridMinZoomFactor = 12.;
            double zoomFactor = (double)params.ZoomPanMatrix(0, 0);

            //
            // Zoom
            //
            {
                ZoomPanTransform::WarpInterp warpInterp;
                if (zoomFactor < 1.0)
                    warpInterp = ZoomPanTransform::WarpInterp::Area;
                else if (zoomFactor < gridMinZoomFactor)
                    warpInterp = ZoomPanTransform::WarpInterp::Bilinear;
                else
                    warpInterp = ZoomPanTransform::WarpInterp::Nearest;

                ImageBuffer backgroundWithImage = fnMakeBackground();

                ZoomPanTransform::WarpAffineScaleTranslate(
                    finalImage, backgroundWithImage,
                    params.ZoomPanMatrix,
                    warpInterp);
                finalImage = backgroundWithImage;
            }

            //
            // Drawings on final image
            //
            {
                // Draw grid
                if (params.ShowGrid && zoomFactor >= gridMinZoomFactor)
                    DrawGrid(finalImage, params);

                // Draw Pixel Values
                double drawPixelvaluesMinZoomFactor = (image.depth == ImageDepth::uint8) ? 36. : 48.;
                if (params.DrawValuesOnZoomedPixels && zoomFactor > drawPixelvaluesMinZoomFactor)
                {
                    double drawPixelCoordsMinZoomFactor = 80.;
                    bool drawPixelCoords = zoomFactor > drawPixelCoordsMinZoomFactor;
                    finalImage = DrawValuesOnZoomedPixels(finalImage, image, params, drawPixelCoords);
                }

                // Draw Watched Pixels
                if (params.HighlightWatchedPixels && (! params.WatchedPixels.empty()))
                    finalImage = DrawWatchedPixels(finalImage, params);

            }

            //
            // Blit
            //
            outTexture->UpdateFromImage(finalImage, false);
        }

        bool HasColormapParam(const ImageParams &params)
        {
            return (!params.ColormapSettings.Colormap.empty() || !params.ColormapSettings.internal_ColormapHovered.empty());
        }

    } // namespace ImageDrawing

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/drawing/internal_icons.cpp                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/drawing/internal_icons.h included by src/immvision/internal/drawing/internal_icons.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace Icons
    {
        enum class IconType
        {
            ZoomPlus,
            ZoomMinus,
            ZoomScaleOne,
            ZoomFullView,
            AdjustLevels,
        };
        ImTextureID GetIcon(IconType iconType);

        bool IconButton(IconType iconType, bool disabled = false);

        void ClearIconsTextureCache();

        void DevelPlaygroundGui();

    } // namespace Icons
} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/drawing/internal_icons.cpp continued                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace Icons
    {
        static Size iconsSizeDraw(200, 200);
        auto ScalePoint = [](Point2d p) {
            return Point2d(p.x * (double) iconsSizeDraw.width, p.y * (double) iconsSizeDraw.height);
        };
        auto ScaleDouble = [](double v) {
            return v * (double) iconsSizeDraw.width;
        };
        auto ScaleInt = [](double v) {
            return (int) (v * (double) iconsSizeDraw.width + 0.5);
        };

        auto PointFromOther = [](Point2d o, double angleDegree, double distance) {
            double m_pi = 3.14159265358979323846;
            double angleRadian = -angleDegree / 180. * m_pi;
            Point2d r(o.x + cos(angleRadian) * distance, o.y + sin(angleRadian) * distance);
            return r;
        };


        ImageBuffer MakeMagnifierImage(IconType iconType)
        {
            using namespace ImmVision;
            ImageBuffer m = ImageBuffer::Zeros(iconsSizeDraw.width, iconsSizeDraw.height, 4, ImageDepth::uint8);

            // Transparent background
            m.fill(Color4d(0, 0, 0, 0));

            Color4d color(255, 255, 255, 255);
            double radius = 0.3;
            Point2d center(1. - radius * 1.3, radius * 1.2);
            // Draw shadow
            {
                Point2d decal(radius * 0.1, radius * 0.1);
                Color4d color_shadow(127, 127, 127, 255);

                DrawingUtils::circle(
                    m, //image,
                    ScalePoint(center + decal),
                    ScaleDouble(radius), //radius
                    color_shadow,
                    ScaleInt(0.08)
                );
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 1.7) + decal),
                    ScalePoint(PointFromOther(center, 225., radius * 1.03) + decal),
                    color_shadow,
                    ScaleInt(0.08)
                );
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 2.3) + decal),
                    ScalePoint(PointFromOther(center, 225., radius * 1.5) + decal),
                    color_shadow,
                    ScaleInt(0.14)
                );
            }
            // Draw magnifier
            {
                DrawingUtils::circle(
                    m, //image,
                    ScalePoint(center),
                    ScaleDouble(radius), //radius
                    color,
                    ScaleInt(0.08)
                );
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 1.7)),
                    ScalePoint(PointFromOther(center, 225., radius * 1.03)),
                    color,
                    ScaleInt(0.08)
                );
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 2.3)),
                    ScalePoint(PointFromOther(center, 225., radius * 1.5)),
                    color,
                    ScaleInt(0.14)
                );
            }

            if (iconType == IconType::ZoomPlus)
            {
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 0., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 180., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 90., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 270., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
            }
            if (iconType == IconType::ZoomMinus)
            {
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 0., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 180., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
            }
            if (iconType == IconType::ZoomScaleOne)
            {
                Point2d a = PointFromOther(center, -90., radius * 0.45);
                Point2d b = PointFromOther(center, 90., radius * 0.45);
                a.x += radius * 0.05;
                b.x += radius * 0.05;
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(a),
                    ScalePoint(b),
                    color,
                    ScaleInt(0.06)
                );
                Point2d c(b.x - radius * 0.2, b.y + radius * 0.2);
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(b),
                    ScalePoint(c),
                    color,
                    ScaleInt(0.06)
                );
            }

            return m;
        }


        ImageBuffer MakeFullViewImage()
        {
            ImageBuffer m = ImageBuffer::Zeros(iconsSizeDraw.width, iconsSizeDraw.height, 4, ImageDepth::uint8);
            m.fill(Color4d(0, 0, 0, 0));

            Color4d color(255, 255, 255, 255);
            double decal = 0.1;
            double length_x = 0.3, length_y = 0.3;
            for (int y = 0; y <= 1; ++y)
            {
                for (int x = 0; x <= 1; ++x)
                {
                    Point2d corner;

                    corner.x = (x == 0) ? decal : 1. - decal;
                    corner.y = (y == 0) ? decal : 1. - decal;
                    double moveX = (x == 0) ? length_x : -length_x;
                    double moveY = (y == 0) ? length_y : -length_y;
                    Point2d pt_x(corner.x + moveX, corner.y);
                    Point2d pt_y(corner.x, corner.y + moveY);
                    int thickness = ScaleInt(0.09);
                    DrawingUtils::line(
                        m,
                        ScalePoint(corner),
                        ScalePoint(pt_x),
                        color,
                        thickness
                    );
                    DrawingUtils::line(
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

        ImageBuffer MakeAdjustLevelsImage()
        {
            ImageBuffer m = ImageBuffer::Zeros(iconsSizeDraw.width, iconsSizeDraw.height, 4, ImageDepth::uint8);
            m.fill(Color4d(0, 0, 0, 0));
            Color4d color(255, 255, 255, 255);

            double yMin = 0.15, yMax = 0.8;
            int nbBars = 3;
            for (int bar = 0; bar < nbBars; ++bar)
            {
                double xBar = (double)bar / ((double)(nbBars) + 0.17) + 0.2;
                Point2d a(xBar, yMin);
                Point2d b(xBar, yMax);
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(a),
                    ScalePoint(b),
                    color,
                    ScaleInt(0.08)
                );

                double barWidth = 0.1;
                double yBar = 0.7 - 0.2 * (double)bar;
                Point2d c(a.x - barWidth / 2., yBar);
                Point2d d(a.x + barWidth / 2., yBar);
                DrawingUtils::line(
                    m, //image,
                    ScalePoint(c),
                    ScalePoint(d),
                    color,
                    ScaleInt(0.16)
                );
            }

            return m;
        }


        static std::map<IconType, std::unique_ptr<GlTexture>> sIconsTextureCache;

        Size IconSize()
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
                ImageBuffer m;
                if (iconType == IconType::ZoomFullView)
                    m = MakeFullViewImage();
                else if (iconType == IconType::AdjustLevels)
                    m = MakeAdjustLevelsImage();
                else
                    m = MakeMagnifierImage(iconType);

                // Simple nearest-neighbor 2x resize for icons
                int dstW = IconSize().width * 2, dstH = IconSize().height * 2;
                ImageBuffer resized = ImageBuffer::Zeros(dstW, dstH, m.channels, m.depth);
                for (int y = 0; y < dstH; y++)
                    for (int x = 0; x < dstW; x++)
                    {
                        int sx = x * m.width / dstW, sy = y * m.height / dstH;
                        int ch = m.channels;
                        const uint8_t* src = m.ptr<uint8_t>(sy) + sx * ch;
                        uint8_t* dst = resized.ptr<uint8_t>(y) + x * ch;
                        for (int c = 0; c < ch; c++) dst[c] = src[c];
                    }
                auto texture = std::make_unique<GlTexture>(resized, true);
                sIconsTextureCache[iconType] = std::move(texture);
            }
            return sIconsTextureCache[iconType]->TextureId;
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
            static ImageBuffer mag = MakeMagnifierImage(IconType::ZoomScaleOne);
            static ImageBuffer img = MakeAdjustLevelsImage();

            static ImmVision::ImageParams imageParams1;
            imageParams1.ImageDisplaySize = {400, 400};
            ImmVision::Image("test", mag, &imageParams1);

            ImGui::SameLine();

            static ImmVision::ImageParams imageParams2;
            imageParams2.ImageDisplaySize = {400, 400};
            ImmVision::Image("test2", img, &imageParams2);

            ImVec2 iconSize(15.f, 15.f);
            ImGui::ImageButton("ZoomScaleOne", GetIcon(IconType::ZoomScaleOne), iconSize);
            ImGui::ImageButton("ZoomPlus", GetIcon(IconType::ZoomPlus), iconSize);
            ImGui::ImageButton("ZoomMinus", GetIcon(IconType::ZoomMinus), iconSize);
            ImGui::ImageButton("ZoomFullView", GetIcon(IconType::ZoomFullView), iconSize);
            ImGui::ImageButton("AdjustLevels", GetIcon(IconType::AdjustLevels), iconSize);
        }

        void ClearIconsTextureCache()
        {
            Icons::sIconsTextureCache.clear();
        }

} // namespace Icons


} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/gl_provider.cpp                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef IMMVISION_BUILDING_PYBIND


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/gl_provider.h included by src/immvision/internal/gl/gl_provider.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision_GlProvider
{
    // InitGlProvider must be called after the OpenGl Loader is initialized
    void InitGlProvider();
    // InitGlProvider must be called before the OpenGl Loader is reset
    void ResetGlProvider();

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, ImTextureID textureId);
    ImTextureID GenTexture();
    void DeleteTexture(ImTextureID texture_id);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/gl_provider.cpp continued                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision_gl_loader/immvision_gl_loader.h included by src/immvision/internal/gl/gl_provider.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(IMMVISION_CUSTOM_GL_INCLUDE)
    // See https://stackoverflow.com/questions/40062883/how-to-use-a-macro-in-an-include-directive
    #define STRINGIFY_MACRO(x) STR(x)
    #define STR(x) #x
    #include STRINGIFY_MACRO(IMMVISION_CUSTOM_GL_INCLUDE)
#elif defined(IMMVISION_USE_GLAD)
    #include <glad/glad.h>
#elif defined(IMMVISION_USE_GLES3)
    #if defined(IOS)
        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>
    #elif defined(__EMSCRIPTEN__)
        #include <GLES3/gl3.h>
        #include <GLES3/gl2ext.h>
    #else
        #include <GLES3/gl3.h>
        #include <GLES3/gl3ext.h>
    #endif
#elif defined(IMMVISION_USE_GLES2)
    #ifdef IOS
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #else
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
    #endif
#else
    #error "immvision_include_opengl: cannot determine GL include path"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/gl_provider.cpp continued                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>

#if defined(IMMVISION_USE_GLAD)
#include <glad/glad.h>
#endif

namespace ImmVision_GlProvider
{
    void _AssertOpenGlLoaderWorking()
    {
        // These OpenGL functions pointers should be filled once the OpenGL loader was inited
        size_t glGenTexturesAddress = (size_t)glGenTextures;
        size_t glDeleteTexturesAddress = (size_t)glDeleteTextures;

        // If they are empty, and if we are using glad, the user probably forgot to call gladLoadGL().
        // Let's help him (this is especially useful for python bindings where no bindings for glad are provided)
        if ((glGenTexturesAddress == 0) || (glDeleteTexturesAddress == 0))
        {
#if defined(IMMVISION_USE_GLAD)
            // fprintf(stderr, "OpenGL Loader was not initialized yet! Trying to initialize it with current OpenGL settings...\n"
            //                 "This may fail. If it does, please make sure to call OpenGL functions after the app is initialized.\n");
            gladLoadGL();
            glGenTexturesAddress = (size_t)glGenTextures;
            glDeleteTexturesAddress = (size_t)glDeleteTextures;
#endif
        }

        if ((glGenTexturesAddress == 0) || (glDeleteTexturesAddress == 0))
        {
            IM_ASSERT(false && "OpenGL Loader was not initialized yet!");
        }
    }

    void InitGlProvider()
    {
        // InitGlProvider must be called after the OpenGl Loader is initialized
        _AssertOpenGlLoaderWorking();
    }

    void ResetGlProvider()
    {
        // InitGlProvider must be called before the OpenGl Loader is reset
        _AssertOpenGlLoaderWorking();
        ImmVision::ClearTextureCache();
        ImmVision::Colormap::ClearColormapsTexturesCache();
        ImmVision::Icons::ClearIconsTextureCache();
    }

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, ImTextureID textureId)
    {
        //static int counter = 0;
        //++counter;
        //std::cout << "Blit_RGBA_Buffer counter=" << counter << "\n";
        GLuint textureIdAsUint = (GLuint)(size_t)textureId;
        glBindTexture(GL_TEXTURE_2D, textureIdAsUint);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(__EMSCRIPTEN__) || defined(IMMVISION_USE_GLES2) || defined(IMMVISION_USE_GLES3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        GLenum gl_color_flag_input = GL_RGBA;
        GLenum gl_color_flag_output = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, gl_color_flag_input,
                     image_width,
                     image_height, 0, gl_color_flag_output, GL_UNSIGNED_BYTE, image_data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ImTextureID GenTexture()
    {
        //std::cout << "GenTexture()\n";
        _AssertOpenGlLoaderWorking();
        GLuint textureId_Gl;
        glGenTextures(1, &textureId_Gl);
        return (ImTextureID)(size_t)textureId_Gl;
    }

    void DeleteTexture(ImTextureID texture_id)
    {
        //std::cout << "DeleteTexture()\n";
        _AssertOpenGlLoaderWorking();
        GLuint textureIdAsUint = (GLuint)(size_t)texture_id;
        glDeleteTextures(1, &textureIdAsUint);
    }
}

#endif // #ifndef IMMVISION_BUILDING_PYBIND


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/gl_texture.cpp                                               //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    GlTexture::GlTexture()
    {
        ImTextureID textureId_Gl = ImmVision_GlProvider::GenTexture();
        this->TextureId = textureId_Gl;
    }

    GlTexture::~GlTexture()
    {
        ImmVision_GlProvider::DeleteTexture(TextureId);
    }

    GlTexture::GlTexture(const ImageBuffer& image, bool isColorOrderBGR) : GlTexture()
    {
        UpdateFromImage(image, isColorOrderBGR);
    }

    void GlTexture::UpdateFromImage(const ImageBuffer& image, bool isColorOrderBGR)
    {
        if (image.empty())
            return;
        ImageBuffer mat_rgba = DrawingUtils::converted_to_rgba_image(image, isColorOrderBGR);

        ImmVision_GlProvider::Blit_RGBA_Buffer(static_cast<unsigned char*>(mat_rgba.data), mat_rgba.width, mat_rgba.height, TextureId);
        this->ImageSize = Size(mat_rgba.width, mat_rgba.height);
    }

    ImVec2 GlTexture::SizeImVec2() const
    {
        return {(float)ImageSize.width, (float)ImageSize.height};
    }

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/short_lived_cache.cpp                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/short_lived_cache.h included by src/immvision/internal/gl/short_lived_cache.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace ImmVision
{
    namespace internal
    {
        double TimerSeconds();

        template<typename Key, typename Value>
        class Cache
        {
        public:
            bool Contains(const Key& k) const
            {
                return mDict.find(k) != mDict.end();
            }

            Value& Get(const Key& k)
            {
                // If you encounter this assert during debugging, it is perhaps due to ShortLiveCache (below)
                // which periodically removes elements that were unused during a given period (5 seconds)
                // You can temporarily disable this feature by adding a return statement at the very beginning of
                // ShortLivedCache::ClearOldEntries()
                assert(mDict.find(k) != mDict.end());
                return mDict.at(k);
            }

            void AddKey(const Key& k)
            {
                assert(mDict.find(k) == mDict.end());
                mDict.insert({k, Value()});
            }

            void RemoveKey(const Key& k)
            {
                mDict.erase(k);
            }

            void Clear()
            {
                mDict.clear();
            }

            std::vector<Key> Keys() const
            {
                std::vector<Key> r;
                for (const auto& kv: mDict)
                    r.push_back(kv.first);
                return r;
            }
        private:
            std::map<Key, Value> mDict;
        };


        template<typename Value>
        class ShortLivedValue
        {
        public:
            ShortLivedValue()
            {
                mLastAccessTime = TimerSeconds();
            }
            Value& Get()
            {
                mLastAccessTime = TimerSeconds();
                return mValue;
            }
            const Value& Get() const
            {
                mLastAccessTime = TimerSeconds();
                return mValue;
            }
            double LastAccessTime() const
            {
                return mLastAccessTime;
            }
        private:
            mutable double mLastAccessTime = TimerSeconds();
            Value mValue;
        };


        template<typename Key, typename Value>
        class ShortLivedCache
        {
        public:
            ShortLivedCache(double timeToLive) : mTimeToLiveSeconds(timeToLive) {}
            bool Contains(const Key& k)
            {
                return mCache.Contains(k);
            }
            Value& Get(const Key& k)
            {
                auto& cached = mCache.Get(k);
                // double now = TimerSeconds();
                // printf("now=%.1f Get %p Last Access Time=%.1f Age=%.1f\n", now, k, cached.LastAccessTime(), now - cached.LastAccessTime());
                return cached.Get();
            }
            const Value& Get(const Key& k) const
            {
                return Get(k);
            }
            void AddKey(const Key& k)
            {
                mCache.AddKey(k);
            }
            void RemoveKey(const Key& k)
            {
                mCache.RemoveKey(k);
            }
            std::vector<Key> Keys() const
            {
                return mCache.Keys();
            }

            void ClearOldEntries()
            {
                double now = TimerSeconds();
                std::vector<Key> oldEntries;
                for (const auto& key: Keys())
                    if ((now - mCache.Get(key).LastAccessTime()) > mTimeToLiveSeconds)
                        oldEntries.push_back(key);

                for (auto& key: oldEntries)
                    mCache.RemoveKey(key);
            }

            void Clear()
            {
                mCache.Clear();
            }
        private:
            double mTimeToLiveSeconds = 1.;
            Cache< Key, ShortLivedValue<Value> > mCache;
        };
    } // namespace internal
} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl/short_lived_cache.cpp continued                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <chrono>

namespace ImmVision
{
    namespace internal
    {
        double TimerSeconds()
        {
            using chrono_second = std::chrono::duration<double, std::ratio<1>>;
            using chrono_clock = std::chrono::steady_clock;

            static std::chrono::time_point<chrono_clock> startTime = chrono_clock::now();
            double elapsed = std::chrono::duration_cast<chrono_second>(chrono_clock::now() - startTime).count();
            return elapsed;
        }

    } // namespace internal
} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/stb/stb_image_write.h included by src/immvision/internal/image.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* stb_image_write - v1.16 - public domain - http://nothings.org/stb
   writes out PNG/BMP/TGA/JPEG/HDR images to C stdio - Sean Barrett 2010-2015
                                     no warranty implied; use at your own risk

   Before #including,

       #define STB_IMAGE_WRITE_IMPLEMENTATION

   in the file that you want to have the implementation.

   Will probably not work correctly with strict-aliasing optimizations.

ABOUT:

   This header file is a library for writing images to C stdio or a callback.

   The PNG output is not optimal; it is 20-50% larger than the file
   written by a decent optimizing implementation; though providing a custom
   zlib compress function (see STBIW_ZLIB_COMPRESS) can mitigate that.
   This library is designed for source code compactness and simplicity,
   not optimal image file size or run-time performance.

BUILDING:

   You can #define STBIW_ASSERT(x) before the #include to avoid using assert.h.
   You can #define STBIW_MALLOC(), STBIW_REALLOC(), and STBIW_FREE() to replace
   malloc,realloc,free.
   You can #define STBIW_MEMMOVE() to replace memmove()
   You can #define STBIW_ZLIB_COMPRESS to use a custom zlib-style compress function
   for PNG compression (instead of the builtin one), it must have the following signature:
   unsigned char * my_compress(unsigned char *data, int data_len, int *out_len, int quality);
   The returned data will be freed with STBIW_FREE() (free() by default),
   so it must be heap allocated with STBIW_MALLOC() (malloc() by default),

UNICODE:

   If compiling for Windows and you wish to use Unicode filenames, compile
   with
       #define STBIW_WINDOWS_UTF8
   and pass utf8-encoded filenames. Call stbiw_convert_wchar_to_utf8 to convert
   Windows wchar_t filenames to utf8.

USAGE:

   There are five functions, one for each image file format:

     int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
     int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
     int stbi_write_tga(char const *filename, int w, int h, int comp, const void *data);
     int stbi_write_jpg(char const *filename, int w, int h, int comp, const void *data, int quality);
     int stbi_write_hdr(char const *filename, int w, int h, int comp, const float *data);

     void stbi_flip_vertically_on_write(int flag); // flag is non-zero to flip data vertically

   There are also five equivalent functions that use an arbitrary write function. You are
   expected to open/close your file-equivalent before and after calling these:

     int stbi_write_png_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data, int stride_in_bytes);
     int stbi_write_bmp_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data);
     int stbi_write_tga_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data);
     int stbi_write_hdr_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const float *data);
     int stbi_write_jpg_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const void *data, int quality);

   where the callback is:
      void stbi_write_func(void *context, void *data, int size);

   You can configure it with these global variables:
      int stbi_write_tga_with_rle;             // defaults to true; set to 0 to disable RLE
      int stbi_write_png_compression_level;    // defaults to 8; set to higher for more compression
      int stbi_write_force_png_filter;         // defaults to -1; set to 0..5 to force a filter mode


   You can define STBI_WRITE_NO_STDIO to disable the file variant of these
   functions, so the library will not use stdio.h at all. However, this will
   also disable HDR writing, because it requires stdio for formatted output.

   Each function returns 0 on failure and non-0 on success.

   The functions create an image file defined by the parameters. The image
   is a rectangle of pixels stored from left-to-right, top-to-bottom.
   Each pixel contains 'comp' channels of data stored interleaved with 8-bits
   per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is
   monochrome color.) The rectangle is 'w' pixels wide and 'h' pixels tall.
   The *data pointer points to the first byte of the top-left-most pixel.
   For PNG, "stride_in_bytes" is the distance in bytes from the first byte of
   a row of pixels to the first byte of the next row of pixels.

   PNG creates output files with the same number of components as the input.
   The BMP format expands Y to RGB in the file format and does not
   output alpha.

   PNG supports writing rectangles of data even when the bytes storing rows of
   data are not consecutive in memory (e.g. sub-rectangles of a larger image),
   by supplying the stride between the beginning of adjacent rows. The other
   formats do not. (Thus you cannot write a native-format BMP through the BMP
   writer, both because it is in BGR order and because it may have padding
   at the end of the line.)

   PNG allows you to set the deflate compression level by setting the global
   variable 'stbi_write_png_compression_level' (it defaults to 8).

   HDR expects linear float data. Since the format is always 32-bit rgb(e)
   data, alpha (if provided) is discarded, and for monochrome data it is
   replicated across all three channels.

   TGA supports RLE or non-RLE compressed data. To use non-RLE-compressed
   data, set the global variable 'stbi_write_tga_with_rle' to 0.

   JPEG does ignore alpha channels in input data; quality is between 1 and 100.
   Higher quality looks better but results in a bigger image.
   JPEG baseline (no JPEG progressive).

CREDITS:


   Sean Barrett           -    PNG/BMP/TGA
   Baldur Karlsson        -    HDR
   Jean-Sebastien Guay    -    TGA monochrome
   Tim Kelsey             -    misc enhancements
   Alan Hickman           -    TGA RLE
   Emmanuel Julien        -    initial file IO callback implementation
   Jon Olick              -    original jo_jpeg.cpp code
   Daniel Gibson          -    integrate JPEG, allow external zlib
   Aarni Koskela          -    allow choosing PNG filter

   bugfixes:
      github:Chribba
      Guillaume Chereau
      github:jry2
      github:romigrou
      Sergio Gonzalez
      Jonas Karlsson
      Filip Wasil
      Thatcher Ulrich
      github:poppolopoppo
      Patrick Boettcher
      github:xeekworx
      Cap Petschulat
      Simon Rodriguez
      Ivan Tikhonov
      github:ignotion
      Adam Schackart
      Andrew Kensler

LICENSE

  See end of file for license information.

*/

#ifndef INCLUDE_STB_IMAGE_WRITE_H
#define INCLUDE_STB_IMAGE_WRITE_H

#include <stdlib.h>

// if STB_IMAGE_WRITE_STATIC causes problems, try defining STBIWDEF to 'inline' or 'static inline'
#ifndef STBIWDEF
#ifdef STB_IMAGE_WRITE_STATIC
#define STBIWDEF  static
#else
#ifdef __cplusplus
#define STBIWDEF  extern "C"
#else
#define STBIWDEF  extern
#endif
#endif
#endif

#ifndef STB_IMAGE_WRITE_STATIC  // C++ forbids static forward declarations
STBIWDEF int stbi_write_tga_with_rle;
STBIWDEF int stbi_write_png_compression_level;
STBIWDEF int stbi_write_force_png_filter;
#endif

#ifndef STBI_WRITE_NO_STDIO
STBIWDEF int stbi_write_png(char const *filename, int w, int h, int comp, const void  *data, int stride_in_bytes);
STBIWDEF int stbi_write_bmp(char const *filename, int w, int h, int comp, const void  *data);
STBIWDEF int stbi_write_tga(char const *filename, int w, int h, int comp, const void  *data);
STBIWDEF int stbi_write_hdr(char const *filename, int w, int h, int comp, const float *data);
STBIWDEF int stbi_write_jpg(char const *filename, int x, int y, int comp, const void  *data, int quality);

#ifdef STBIW_WINDOWS_UTF8
STBIWDEF int stbiw_convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input);
#endif
#endif

typedef void stbi_write_func(void *context, void *data, int size);

STBIWDEF int stbi_write_png_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data, int stride_in_bytes);
STBIWDEF int stbi_write_bmp_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data);
STBIWDEF int stbi_write_tga_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data);
STBIWDEF int stbi_write_hdr_to_func(stbi_write_func *func, void *context, int w, int h, int comp, const float *data);
STBIWDEF int stbi_write_jpg_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const void  *data, int quality);

STBIWDEF void stbi_flip_vertically_on_write(int flip_boolean);

#endif//INCLUDE_STB_IMAGE_WRITE_H

#ifdef STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef _WIN32
   #ifndef _CRT_SECURE_NO_WARNINGS
   #define _CRT_SECURE_NO_WARNINGS
   #endif
   #ifndef _CRT_NONSTDC_NO_DEPRECATE
   #define _CRT_NONSTDC_NO_DEPRECATE
   #endif
#endif

#ifndef STBI_WRITE_NO_STDIO
#endif // STBI_WRITE_NO_STDIO

#include <stdarg.h>
#include <string.h>

#if defined(STBIW_MALLOC) && defined(STBIW_FREE) && (defined(STBIW_REALLOC) || defined(STBIW_REALLOC_SIZED))
// ok
#elif !defined(STBIW_MALLOC) && !defined(STBIW_FREE) && !defined(STBIW_REALLOC) && !defined(STBIW_REALLOC_SIZED)
// ok
#else
#error "Must define all or none of STBIW_MALLOC, STBIW_FREE, and STBIW_REALLOC (or STBIW_REALLOC_SIZED)."
#endif

#ifndef STBIW_MALLOC
#define STBIW_MALLOC(sz)        malloc(sz)
#define STBIW_REALLOC(p,newsz)  realloc(p,newsz)
#define STBIW_FREE(p)           free(p)
#endif

#ifndef STBIW_REALLOC_SIZED
#define STBIW_REALLOC_SIZED(p,oldsz,newsz) STBIW_REALLOC(p,newsz)
#endif


#ifndef STBIW_MEMMOVE
#define STBIW_MEMMOVE(a,b,sz) memmove(a,b,sz)
#endif


#ifndef STBIW_ASSERT
#include <assert.h>
#define STBIW_ASSERT(x) assert(x)
#endif

#define STBIW_UCHAR(x) (unsigned char) ((x) & 0xff)

#ifdef STB_IMAGE_WRITE_STATIC
static int stbi_write_png_compression_level = 8;
static int stbi_write_tga_with_rle = 1;
static int stbi_write_force_png_filter = -1;
#else
int stbi_write_png_compression_level = 8;
int stbi_write_tga_with_rle = 1;
int stbi_write_force_png_filter = -1;
#endif

static int stbi__flip_vertically_on_write = 0;

STBIWDEF void stbi_flip_vertically_on_write(int flag)
{
   stbi__flip_vertically_on_write = flag;
}

typedef struct
{
   stbi_write_func *func;
   void *context;
   unsigned char buffer[64];
   int buf_used;
} stbi__write_context;

// initialize a callback-based context
static void stbi__start_write_callbacks(stbi__write_context *s, stbi_write_func *c, void *context)
{
   s->func    = c;
   s->context = context;
}

#ifndef STBI_WRITE_NO_STDIO

static void stbi__stdio_write(void *context, void *data, int size)
{
   fwrite(data,1,size,(FILE*) context);
}

#if defined(_WIN32) && defined(STBIW_WINDOWS_UTF8)
#ifdef __cplusplus
#define STBIW_EXTERN extern "C"
#else
#define STBIW_EXTERN extern
#endif
STBIW_EXTERN __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int cp, unsigned long flags, const char *str, int cbmb, wchar_t *widestr, int cchwide);
STBIW_EXTERN __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags, const wchar_t *widestr, int cchwide, char *str, int cbmb, const char *defchar, int *used_default);

STBIWDEF int stbiw_convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input)
{
   return WideCharToMultiByte(65001 /* UTF8 */, 0, input, -1, buffer, (int) bufferlen, NULL, NULL);
}
#endif

static FILE *stbiw__fopen(char const *filename, char const *mode)
{
   FILE *f;
#if defined(_WIN32) && defined(STBIW_WINDOWS_UTF8)
   wchar_t wMode[64];
   wchar_t wFilename[1024];
   if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, filename, -1, wFilename, sizeof(wFilename)/sizeof(*wFilename)))
      return 0;

   if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, mode, -1, wMode, sizeof(wMode)/sizeof(*wMode)))
      return 0;

#if defined(_MSC_VER) && _MSC_VER >= 1400
   if (0 != _wfopen_s(&f, wFilename, wMode))
      f = 0;
#else
   f = _wfopen(wFilename, wMode);
#endif

#elif defined(_MSC_VER) && _MSC_VER >= 1400
   if (0 != fopen_s(&f, filename, mode))
      f=0;
#else
   f = fopen(filename, mode);
#endif
   return f;
}

static int stbi__start_write_file(stbi__write_context *s, const char *filename)
{
   FILE *f = stbiw__fopen(filename, "wb");
   stbi__start_write_callbacks(s, stbi__stdio_write, (void *) f);
   return f != NULL;
}

static void stbi__end_write_file(stbi__write_context *s)
{
   fclose((FILE *)s->context);
}

#endif // !STBI_WRITE_NO_STDIO

typedef unsigned int stbiw_uint32;
typedef int stb_image_write_test[sizeof(stbiw_uint32)==4 ? 1 : -1];

static void stbiw__writefv(stbi__write_context *s, const char *fmt, va_list v)
{
   while (*fmt) {
      switch (*fmt++) {
         case ' ': break;
         case '1': { unsigned char x = STBIW_UCHAR(va_arg(v, int));
                     s->func(s->context,&x,1);
                     break; }
         case '2': { int x = va_arg(v,int);
                     unsigned char b[2];
                     b[0] = STBIW_UCHAR(x);
                     b[1] = STBIW_UCHAR(x>>8);
                     s->func(s->context,b,2);
                     break; }
         case '4': { stbiw_uint32 x = va_arg(v,int);
                     unsigned char b[4];
                     b[0]=STBIW_UCHAR(x);
                     b[1]=STBIW_UCHAR(x>>8);
                     b[2]=STBIW_UCHAR(x>>16);
                     b[3]=STBIW_UCHAR(x>>24);
                     s->func(s->context,b,4);
                     break; }
         default:
            STBIW_ASSERT(0);
            return;
      }
   }
}

static void stbiw__writef(stbi__write_context *s, const char *fmt, ...)
{
   va_list v;
   va_start(v, fmt);
   stbiw__writefv(s, fmt, v);
   va_end(v);
}

static void stbiw__write_flush(stbi__write_context *s)
{
   if (s->buf_used) {
      s->func(s->context, &s->buffer, s->buf_used);
      s->buf_used = 0;
   }
}

static void stbiw__putc(stbi__write_context *s, unsigned char c)
{
   s->func(s->context, &c, 1);
}

static void stbiw__write1(stbi__write_context *s, unsigned char a)
{
   if ((size_t)s->buf_used + 1 > sizeof(s->buffer))
      stbiw__write_flush(s);
   s->buffer[s->buf_used++] = a;
}

static void stbiw__write3(stbi__write_context *s, unsigned char a, unsigned char b, unsigned char c)
{
   int n;
   if ((size_t)s->buf_used + 3 > sizeof(s->buffer))
      stbiw__write_flush(s);
   n = s->buf_used;
   s->buf_used = n+3;
   s->buffer[n+0] = a;
   s->buffer[n+1] = b;
   s->buffer[n+2] = c;
}

static void stbiw__write_pixel(stbi__write_context *s, int rgb_dir, int comp, int write_alpha, int expand_mono, unsigned char *d)
{
   unsigned char bg[3] = { 255, 0, 255}, px[3];
   int k;

   if (write_alpha < 0)
      stbiw__write1(s, d[comp - 1]);

   switch (comp) {
      case 2: // 2 pixels = mono + alpha, alpha is written separately, so same as 1-channel case
      case 1:
         if (expand_mono)
            stbiw__write3(s, d[0], d[0], d[0]); // monochrome bmp
         else
            stbiw__write1(s, d[0]);  // monochrome TGA
         break;
      case 4:
         if (!write_alpha) {
            // composite against pink background
            for (k = 0; k < 3; ++k)
               px[k] = bg[k] + ((d[k] - bg[k]) * d[3]) / 255;
            stbiw__write3(s, px[1 - rgb_dir], px[1], px[1 + rgb_dir]);
            break;
         }
         /* FALLTHROUGH */
      case 3:
         stbiw__write3(s, d[1 - rgb_dir], d[1], d[1 + rgb_dir]);
         break;
   }
   if (write_alpha > 0)
      stbiw__write1(s, d[comp - 1]);
}

static void stbiw__write_pixels(stbi__write_context *s, int rgb_dir, int vdir, int x, int y, int comp, void *data, int write_alpha, int scanline_pad, int expand_mono)
{
   stbiw_uint32 zero = 0;
   int i,j, j_end;

   if (y <= 0)
      return;

   if (stbi__flip_vertically_on_write)
      vdir *= -1;

   if (vdir < 0) {
      j_end = -1; j = y-1;
   } else {
      j_end =  y; j = 0;
   }

   for (; j != j_end; j += vdir) {
      for (i=0; i < x; ++i) {
         unsigned char *d = (unsigned char *) data + (j*x+i)*comp;
         stbiw__write_pixel(s, rgb_dir, comp, write_alpha, expand_mono, d);
      }
      stbiw__write_flush(s);
      s->func(s->context, &zero, scanline_pad);
   }
}

static int stbiw__outfile(stbi__write_context *s, int rgb_dir, int vdir, int x, int y, int comp, int expand_mono, void *data, int alpha, int pad, const char *fmt, ...)
{
   if (y < 0 || x < 0) {
      return 0;
   } else {
      va_list v;
      va_start(v, fmt);
      stbiw__writefv(s, fmt, v);
      va_end(v);
      stbiw__write_pixels(s,rgb_dir,vdir,x,y,comp,data,alpha,pad, expand_mono);
      return 1;
   }
}

static int stbi_write_bmp_core(stbi__write_context *s, int x, int y, int comp, const void *data)
{
   if (comp != 4) {
      // write RGB bitmap
      int pad = (-x*3) & 3;
      return stbiw__outfile(s,-1,-1,x,y,comp,1,(void *) data,0,pad,
              "11 4 22 4" "4 44 22 444444",
              'B', 'M', 14+40+(x*3+pad)*y, 0,0, 14+40,  // file header
               40, x,y, 1,24, 0,0,0,0,0,0);             // bitmap header
   } else {
      // RGBA bitmaps need a v4 header
      // use BI_BITFIELDS mode with 32bpp and alpha mask
      // (straight BI_RGB with alpha mask doesn't work in most readers)
      return stbiw__outfile(s,-1,-1,x,y,comp,1,(void *)data,1,0,
         "11 4 22 4" "4 44 22 444444 4444 4 444 444 444 444",
         'B', 'M', 14+108+x*y*4, 0, 0, 14+108, // file header
         108, x,y, 1,32, 3,0,0,0,0,0, 0xff0000,0xff00,0xff,0xff000000u, 0, 0,0,0, 0,0,0, 0,0,0, 0,0,0); // bitmap V4 header
   }
}

STBIWDEF int stbi_write_bmp_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const void *data)
{
   stbi__write_context s = { 0 };
   stbi__start_write_callbacks(&s, func, context);
   return stbi_write_bmp_core(&s, x, y, comp, data);
}

#ifndef STBI_WRITE_NO_STDIO
STBIWDEF int stbi_write_bmp(char const *filename, int x, int y, int comp, const void *data)
{
   stbi__write_context s = { 0 };
   if (stbi__start_write_file(&s,filename)) {
      int r = stbi_write_bmp_core(&s, x, y, comp, data);
      stbi__end_write_file(&s);
      return r;
   } else
      return 0;
}
#endif //!STBI_WRITE_NO_STDIO

static int stbi_write_tga_core(stbi__write_context *s, int x, int y, int comp, void *data)
{
   int has_alpha = (comp == 2 || comp == 4);
   int colorbytes = has_alpha ? comp-1 : comp;
   int format = colorbytes < 2 ? 3 : 2; // 3 color channels (RGB/RGBA) = 2, 1 color channel (Y/YA) = 3

   if (y < 0 || x < 0)
      return 0;

   if (!stbi_write_tga_with_rle) {
      return stbiw__outfile(s, -1, -1, x, y, comp, 0, (void *) data, has_alpha, 0,
         "111 221 2222 11", 0, 0, format, 0, 0, 0, 0, 0, x, y, (colorbytes + has_alpha) * 8, has_alpha * 8);
   } else {
      int i,j,k;
      int jend, jdir;

      stbiw__writef(s, "111 221 2222 11", 0,0,format+8, 0,0,0, 0,0,x,y, (colorbytes + has_alpha) * 8, has_alpha * 8);

      if (stbi__flip_vertically_on_write) {
         j = 0;
         jend = y;
         jdir = 1;
      } else {
         j = y-1;
         jend = -1;
         jdir = -1;
      }
      for (; j != jend; j += jdir) {
         unsigned char *row = (unsigned char *) data + j * x * comp;
         int len;

         for (i = 0; i < x; i += len) {
            unsigned char *begin = row + i * comp;
            int diff = 1;
            len = 1;

            if (i < x - 1) {
               ++len;
               diff = memcmp(begin, row + (i + 1) * comp, comp);
               if (diff) {
                  const unsigned char *prev = begin;
                  for (k = i + 2; k < x && len < 128; ++k) {
                     if (memcmp(prev, row + k * comp, comp)) {
                        prev += comp;
                        ++len;
                     } else {
                        --len;
                        break;
                     }
                  }
               } else {
                  for (k = i + 2; k < x && len < 128; ++k) {
                     if (!memcmp(begin, row + k * comp, comp)) {
                        ++len;
                     } else {
                        break;
                     }
                  }
               }
            }

            if (diff) {
               unsigned char header = STBIW_UCHAR(len - 1);
               stbiw__write1(s, header);
               for (k = 0; k < len; ++k) {
                  stbiw__write_pixel(s, -1, comp, has_alpha, 0, begin + k * comp);
               }
            } else {
               unsigned char header = STBIW_UCHAR(len - 129);
               stbiw__write1(s, header);
               stbiw__write_pixel(s, -1, comp, has_alpha, 0, begin);
            }
         }
      }
      stbiw__write_flush(s);
   }
   return 1;
}

STBIWDEF int stbi_write_tga_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const void *data)
{
   stbi__write_context s = { 0 };
   stbi__start_write_callbacks(&s, func, context);
   return stbi_write_tga_core(&s, x, y, comp, (void *) data);
}

#ifndef STBI_WRITE_NO_STDIO
STBIWDEF int stbi_write_tga(char const *filename, int x, int y, int comp, const void *data)
{
   stbi__write_context s = { 0 };
   if (stbi__start_write_file(&s,filename)) {
      int r = stbi_write_tga_core(&s, x, y, comp, (void *) data);
      stbi__end_write_file(&s);
      return r;
   } else
      return 0;
}
#endif

// *************************************************************************************************
// Radiance RGBE HDR writer
// by Baldur Karlsson

#define stbiw__max(a, b)  ((a) > (b) ? (a) : (b))

#ifndef STBI_WRITE_NO_STDIO

static void stbiw__linear_to_rgbe(unsigned char *rgbe, float *linear)
{
   int exponent;
   float maxcomp = stbiw__max(linear[0], stbiw__max(linear[1], linear[2]));

   if (maxcomp < 1e-32f) {
      rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
   } else {
      float normalize = (float) frexp(maxcomp, &exponent) * 256.0f/maxcomp;

      rgbe[0] = (unsigned char)(linear[0] * normalize);
      rgbe[1] = (unsigned char)(linear[1] * normalize);
      rgbe[2] = (unsigned char)(linear[2] * normalize);
      rgbe[3] = (unsigned char)(exponent + 128);
   }
}

static void stbiw__write_run_data(stbi__write_context *s, int length, unsigned char databyte)
{
   unsigned char lengthbyte = STBIW_UCHAR(length+128);
   STBIW_ASSERT(length+128 <= 255);
   s->func(s->context, &lengthbyte, 1);
   s->func(s->context, &databyte, 1);
}

static void stbiw__write_dump_data(stbi__write_context *s, int length, unsigned char *data)
{
   unsigned char lengthbyte = STBIW_UCHAR(length);
   STBIW_ASSERT(length <= 128); // inconsistent with spec but consistent with official code
   s->func(s->context, &lengthbyte, 1);
   s->func(s->context, data, length);
}

static void stbiw__write_hdr_scanline(stbi__write_context *s, int width, int ncomp, unsigned char *scratch, float *scanline)
{
   unsigned char scanlineheader[4] = { 2, 2, 0, 0 };
   unsigned char rgbe[4];
   float linear[3];
   int x;

   scanlineheader[2] = (width&0xff00)>>8;
   scanlineheader[3] = (width&0x00ff);

   /* skip RLE for images too small or large */
   if (width < 8 || width >= 32768) {
      for (x=0; x < width; x++) {
         switch (ncomp) {
            case 4: /* fallthrough */
            case 3: linear[2] = scanline[x*ncomp + 2];
                    linear[1] = scanline[x*ncomp + 1];
                    linear[0] = scanline[x*ncomp + 0];
                    break;
            default:
                    linear[0] = linear[1] = linear[2] = scanline[x*ncomp + 0];
                    break;
         }
         stbiw__linear_to_rgbe(rgbe, linear);
         s->func(s->context, rgbe, 4);
      }
   } else {
      int c,r;
      /* encode into scratch buffer */
      for (x=0; x < width; x++) {
         switch(ncomp) {
            case 4: /* fallthrough */
            case 3: linear[2] = scanline[x*ncomp + 2];
                    linear[1] = scanline[x*ncomp + 1];
                    linear[0] = scanline[x*ncomp + 0];
                    break;
            default:
                    linear[0] = linear[1] = linear[2] = scanline[x*ncomp + 0];
                    break;
         }
         stbiw__linear_to_rgbe(rgbe, linear);
         scratch[x + width*0] = rgbe[0];
         scratch[x + width*1] = rgbe[1];
         scratch[x + width*2] = rgbe[2];
         scratch[x + width*3] = rgbe[3];
      }

      s->func(s->context, scanlineheader, 4);

      /* RLE each component separately */
      for (c=0; c < 4; c++) {
         unsigned char *comp = &scratch[width*c];

         x = 0;
         while (x < width) {
            // find first run
            r = x;
            while (r+2 < width) {
               if (comp[r] == comp[r+1] && comp[r] == comp[r+2])
                  break;
               ++r;
            }
            if (r+2 >= width)
               r = width;
            // dump up to first run
            while (x < r) {
               int len = r-x;
               if (len > 128) len = 128;
               stbiw__write_dump_data(s, len, &comp[x]);
               x += len;
            }
            // if there's a run, output it
            if (r+2 < width) { // same test as what we break out of in search loop, so only true if we break'd
               // find next byte after run
               while (r < width && comp[r] == comp[x])
                  ++r;
               // output run up to r
               while (x < r) {
                  int len = r-x;
                  if (len > 127) len = 127;
                  stbiw__write_run_data(s, len, comp[x]);
                  x += len;
               }
            }
         }
      }
   }
}

static int stbi_write_hdr_core(stbi__write_context *s, int x, int y, int comp, float *data)
{
   if (y <= 0 || x <= 0 || data == NULL)
      return 0;
   else {
      // Each component is stored separately. Allocate scratch space for full output scanline.
      unsigned char *scratch = (unsigned char *) STBIW_MALLOC(x*4);
      int i, len;
      char buffer[128];
      char header[] = "#?RADIANCE\n# Written by stb_image_write.h\nFORMAT=32-bit_rle_rgbe\n";
      s->func(s->context, header, sizeof(header)-1);

#ifdef __STDC_LIB_EXT1__
      len = sprintf_s(buffer, sizeof(buffer), "EXPOSURE=          1.0000000000000\n\n-Y %d +X %d\n", y, x);
#else
      len = sprintf(buffer, "EXPOSURE=          1.0000000000000\n\n-Y %d +X %d\n", y, x);
#endif
      s->func(s->context, buffer, len);

      for(i=0; i < y; i++)
         stbiw__write_hdr_scanline(s, x, comp, scratch, data + comp*x*(stbi__flip_vertically_on_write ? y-1-i : i));
      STBIW_FREE(scratch);
      return 1;
   }
}

STBIWDEF int stbi_write_hdr_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const float *data)
{
   stbi__write_context s = { 0 };
   stbi__start_write_callbacks(&s, func, context);
   return stbi_write_hdr_core(&s, x, y, comp, (float *) data);
}

STBIWDEF int stbi_write_hdr(char const *filename, int x, int y, int comp, const float *data)
{
   stbi__write_context s = { 0 };
   if (stbi__start_write_file(&s,filename)) {
      int r = stbi_write_hdr_core(&s, x, y, comp, (float *) data);
      stbi__end_write_file(&s);
      return r;
   } else
      return 0;
}
#endif // STBI_WRITE_NO_STDIO


//////////////////////////////////////////////////////////////////////////////
//
// PNG writer
//

#ifndef STBIW_ZLIB_COMPRESS
// stretchy buffer; stbiw__sbpush() == vector<>::push_back() -- stbiw__sbcount() == vector<>::size()
#define stbiw__sbraw(a) ((int *) (void *) (a) - 2)
#define stbiw__sbm(a)   stbiw__sbraw(a)[0]
#define stbiw__sbn(a)   stbiw__sbraw(a)[1]

#define stbiw__sbneedgrow(a,n)  ((a)==0 || stbiw__sbn(a)+n >= stbiw__sbm(a))
#define stbiw__sbmaybegrow(a,n) (stbiw__sbneedgrow(a,(n)) ? stbiw__sbgrow(a,n) : 0)
#define stbiw__sbgrow(a,n)  stbiw__sbgrowf((void **) &(a), (n), sizeof(*(a)))

#define stbiw__sbpush(a, v)      (stbiw__sbmaybegrow(a,1), (a)[stbiw__sbn(a)++] = (v))
#define stbiw__sbcount(a)        ((a) ? stbiw__sbn(a) : 0)
#define stbiw__sbfree(a)         ((a) ? STBIW_FREE(stbiw__sbraw(a)),0 : 0)

static void *stbiw__sbgrowf(void **arr, int increment, int itemsize)
{
   int m = *arr ? 2*stbiw__sbm(*arr)+increment : increment+1;
   void *p = STBIW_REALLOC_SIZED(*arr ? stbiw__sbraw(*arr) : 0, *arr ? (stbiw__sbm(*arr)*itemsize + sizeof(int)*2) : 0, itemsize * m + sizeof(int)*2);
   STBIW_ASSERT(p);
   if (p) {
      if (!*arr) ((int *) p)[1] = 0;
      *arr = (void *) ((int *) p + 2);
      stbiw__sbm(*arr) = m;
   }
   return *arr;
}

static unsigned char *stbiw__zlib_flushf(unsigned char *data, unsigned int *bitbuffer, int *bitcount)
{
   while (*bitcount >= 8) {
      stbiw__sbpush(data, STBIW_UCHAR(*bitbuffer));
      *bitbuffer >>= 8;
      *bitcount -= 8;
   }
   return data;
}

static int stbiw__zlib_bitrev(int code, int codebits)
{
   int res=0;
   while (codebits--) {
      res = (res << 1) | (code & 1);
      code >>= 1;
   }
   return res;
}

static unsigned int stbiw__zlib_countm(unsigned char *a, unsigned char *b, int limit)
{
   int i;
   for (i=0; i < limit && i < 258; ++i)
      if (a[i] != b[i]) break;
   return i;
}

static unsigned int stbiw__zhash(unsigned char *data)
{
   stbiw_uint32 hash = data[0] + (data[1] << 8) + (data[2] << 16);
   hash ^= hash << 3;
   hash += hash >> 5;
   hash ^= hash << 4;
   hash += hash >> 17;
   hash ^= hash << 25;
   hash += hash >> 6;
   return hash;
}

#define stbiw__zlib_flush() (out = stbiw__zlib_flushf(out, &bitbuf, &bitcount))
#define stbiw__zlib_add(code,codebits) \
      (bitbuf |= (code) << bitcount, bitcount += (codebits), stbiw__zlib_flush())
#define stbiw__zlib_huffa(b,c)  stbiw__zlib_add(stbiw__zlib_bitrev(b,c),c)
// default huffman tables
#define stbiw__zlib_huff1(n)  stbiw__zlib_huffa(0x30 + (n), 8)
#define stbiw__zlib_huff2(n)  stbiw__zlib_huffa(0x190 + (n)-144, 9)
#define stbiw__zlib_huff3(n)  stbiw__zlib_huffa(0 + (n)-256,7)
#define stbiw__zlib_huff4(n)  stbiw__zlib_huffa(0xc0 + (n)-280,8)
#define stbiw__zlib_huff(n)  ((n) <= 143 ? stbiw__zlib_huff1(n) : (n) <= 255 ? stbiw__zlib_huff2(n) : (n) <= 279 ? stbiw__zlib_huff3(n) : stbiw__zlib_huff4(n))
#define stbiw__zlib_huffb(n) ((n) <= 143 ? stbiw__zlib_huff1(n) : stbiw__zlib_huff2(n))

#define stbiw__ZHASH   16384

#endif // STBIW_ZLIB_COMPRESS

STBIWDEF unsigned char * stbi_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality)
{
#ifdef STBIW_ZLIB_COMPRESS
   // user provided a zlib compress implementation, use that
   return STBIW_ZLIB_COMPRESS(data, data_len, out_len, quality);
#else // use builtin
   static unsigned short lengthc[] = { 3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258, 259 };
   static unsigned char  lengtheb[]= { 0,0,0,0,0,0,0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5,  0 };
   static unsigned short distc[]   = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577, 32768 };
   static unsigned char  disteb[]  = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };
   unsigned int bitbuf=0;
   int i,j, bitcount=0;
   unsigned char *out = NULL;
   unsigned char ***hash_table = (unsigned char***) STBIW_MALLOC(stbiw__ZHASH * sizeof(unsigned char**));
   if (hash_table == NULL)
      return NULL;
   if (quality < 5) quality = 5;

   stbiw__sbpush(out, 0x78);   // DEFLATE 32K window
   stbiw__sbpush(out, 0x5e);   // FLEVEL = 1
   stbiw__zlib_add(1,1);  // BFINAL = 1
   stbiw__zlib_add(1,2);  // BTYPE = 1 -- fixed huffman

   for (i=0; i < stbiw__ZHASH; ++i)
      hash_table[i] = NULL;

   i=0;
   while (i < data_len-3) {
      // hash next 3 bytes of data to be compressed
      int h = stbiw__zhash(data+i)&(stbiw__ZHASH-1), best=3;
      unsigned char *bestloc = 0;
      unsigned char **hlist = hash_table[h];
      int n = stbiw__sbcount(hlist);
      for (j=0; j < n; ++j) {
         if (hlist[j]-data > i-32768) { // if entry lies within window
            int d = stbiw__zlib_countm(hlist[j], data+i, data_len-i);
            if (d >= best) { best=d; bestloc=hlist[j]; }
         }
      }
      // when hash table entry is too long, delete half the entries
      if (hash_table[h] && stbiw__sbn(hash_table[h]) == 2*quality) {
         STBIW_MEMMOVE(hash_table[h], hash_table[h]+quality, sizeof(hash_table[h][0])*quality);
         stbiw__sbn(hash_table[h]) = quality;
      }
      stbiw__sbpush(hash_table[h],data+i);

      if (bestloc) {
         // "lazy matching" - check match at *next* byte, and if it's better, do cur byte as literal
         h = stbiw__zhash(data+i+1)&(stbiw__ZHASH-1);
         hlist = hash_table[h];
         n = stbiw__sbcount(hlist);
         for (j=0; j < n; ++j) {
            if (hlist[j]-data > i-32767) {
               int e = stbiw__zlib_countm(hlist[j], data+i+1, data_len-i-1);
               if (e > best) { // if next match is better, bail on current match
                  bestloc = NULL;
                  break;
               }
            }
         }
      }

      if (bestloc) {
         int d = (int) (data+i - bestloc); // distance back
         STBIW_ASSERT(d <= 32767 && best <= 258);
         for (j=0; best > lengthc[j+1]-1; ++j);
         stbiw__zlib_huff(j+257);
         if (lengtheb[j]) stbiw__zlib_add(best - lengthc[j], lengtheb[j]);
         for (j=0; d > distc[j+1]-1; ++j);
         stbiw__zlib_add(stbiw__zlib_bitrev(j,5),5);
         if (disteb[j]) stbiw__zlib_add(d - distc[j], disteb[j]);
         i += best;
      } else {
         stbiw__zlib_huffb(data[i]);
         ++i;
      }
   }
   // write out final bytes
   for (;i < data_len; ++i)
      stbiw__zlib_huffb(data[i]);
   stbiw__zlib_huff(256); // end of block
   // pad with 0 bits to byte boundary
   while (bitcount)
      stbiw__zlib_add(0,1);

   for (i=0; i < stbiw__ZHASH; ++i)
      (void) stbiw__sbfree(hash_table[i]);
   STBIW_FREE(hash_table);

   // store uncompressed instead if compression was worse
   if (stbiw__sbn(out) > data_len + 2 + ((data_len+32766)/32767)*5) {
      stbiw__sbn(out) = 2;  // truncate to DEFLATE 32K window and FLEVEL = 1
      for (j = 0; j < data_len;) {
         int blocklen = data_len - j;
         if (blocklen > 32767) blocklen = 32767;
         stbiw__sbpush(out, data_len - j == blocklen); // BFINAL = ?, BTYPE = 0 -- no compression
         stbiw__sbpush(out, STBIW_UCHAR(blocklen)); // LEN
         stbiw__sbpush(out, STBIW_UCHAR(blocklen >> 8));
         stbiw__sbpush(out, STBIW_UCHAR(~blocklen)); // NLEN
         stbiw__sbpush(out, STBIW_UCHAR(~blocklen >> 8));
         memcpy(out+stbiw__sbn(out), data+j, blocklen);
         stbiw__sbn(out) += blocklen;
         j += blocklen;
      }
   }

   {
      // compute adler32 on input
      unsigned int s1=1, s2=0;
      int blocklen = (int) (data_len % 5552);
      j=0;
      while (j < data_len) {
         for (i=0; i < blocklen; ++i) { s1 += data[j+i]; s2 += s1; }
         s1 %= 65521; s2 %= 65521;
         j += blocklen;
         blocklen = 5552;
      }
      stbiw__sbpush(out, STBIW_UCHAR(s2 >> 8));
      stbiw__sbpush(out, STBIW_UCHAR(s2));
      stbiw__sbpush(out, STBIW_UCHAR(s1 >> 8));
      stbiw__sbpush(out, STBIW_UCHAR(s1));
   }
   *out_len = stbiw__sbn(out);
   // make returned pointer freeable
   STBIW_MEMMOVE(stbiw__sbraw(out), out, *out_len);
   return (unsigned char *) stbiw__sbraw(out);
#endif // STBIW_ZLIB_COMPRESS
}

static unsigned int stbiw__crc32(unsigned char *buffer, int len)
{
#ifdef STBIW_CRC32
    return STBIW_CRC32(buffer, len);
#else
   static unsigned int crc_table[256] =
   {
      0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
      0x0eDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
      0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
      0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
      0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
      0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
      0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
      0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
      0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
      0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
      0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
      0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
      0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
      0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
      0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
      0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
      0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
      0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
      0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
      0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
      0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
      0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
      0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
      0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
      0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
      0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
      0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
      0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
      0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
      0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
      0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
      0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
   };

   unsigned int crc = ~0u;
   int i;
   for (i=0; i < len; ++i)
      crc = (crc >> 8) ^ crc_table[buffer[i] ^ (crc & 0xff)];
   return ~crc;
#endif
}

#define stbiw__wpng4(o,a,b,c,d) ((o)[0]=STBIW_UCHAR(a),(o)[1]=STBIW_UCHAR(b),(o)[2]=STBIW_UCHAR(c),(o)[3]=STBIW_UCHAR(d),(o)+=4)
#define stbiw__wp32(data,v) stbiw__wpng4(data, (v)>>24,(v)>>16,(v)>>8,(v));
#define stbiw__wptag(data,s) stbiw__wpng4(data, s[0],s[1],s[2],s[3])

static void stbiw__wpcrc(unsigned char **data, int len)
{
   unsigned int crc = stbiw__crc32(*data - len - 4, len+4);
   stbiw__wp32(*data, crc);
}

static unsigned char stbiw__paeth(int a, int b, int c)
{
   int p = a + b - c, pa = abs(p-a), pb = abs(p-b), pc = abs(p-c);
   if (pa <= pb && pa <= pc) return STBIW_UCHAR(a);
   if (pb <= pc) return STBIW_UCHAR(b);
   return STBIW_UCHAR(c);
}

// @OPTIMIZE: provide an option that always forces left-predict or paeth predict
static void stbiw__encode_png_line(unsigned char *pixels, int stride_bytes, int width, int height, int y, int n, int filter_type, signed char *line_buffer)
{
   static int mapping[] = { 0,1,2,3,4 };
   static int firstmap[] = { 0,1,0,5,6 };
   int *mymap = (y != 0) ? mapping : firstmap;
   int i;
   int type = mymap[filter_type];
   unsigned char *z = pixels + stride_bytes * (stbi__flip_vertically_on_write ? height-1-y : y);
   int signed_stride = stbi__flip_vertically_on_write ? -stride_bytes : stride_bytes;

   if (type==0) {
      memcpy(line_buffer, z, width*n);
      return;
   }

   // first loop isn't optimized since it's just one pixel
   for (i = 0; i < n; ++i) {
      switch (type) {
         case 1: line_buffer[i] = z[i]; break;
         case 2: line_buffer[i] = z[i] - z[i-signed_stride]; break;
         case 3: line_buffer[i] = z[i] - (z[i-signed_stride]>>1); break;
         case 4: line_buffer[i] = (signed char) (z[i] - stbiw__paeth(0,z[i-signed_stride],0)); break;
         case 5: line_buffer[i] = z[i]; break;
         case 6: line_buffer[i] = z[i]; break;
      }
   }
   switch (type) {
      case 1: for (i=n; i < width*n; ++i) line_buffer[i] = z[i] - z[i-n]; break;
      case 2: for (i=n; i < width*n; ++i) line_buffer[i] = z[i] - z[i-signed_stride]; break;
      case 3: for (i=n; i < width*n; ++i) line_buffer[i] = z[i] - ((z[i-n] + z[i-signed_stride])>>1); break;
      case 4: for (i=n; i < width*n; ++i) line_buffer[i] = z[i] - stbiw__paeth(z[i-n], z[i-signed_stride], z[i-signed_stride-n]); break;
      case 5: for (i=n; i < width*n; ++i) line_buffer[i] = z[i] - (z[i-n]>>1); break;
      case 6: for (i=n; i < width*n; ++i) line_buffer[i] = z[i] - stbiw__paeth(z[i-n], 0,0); break;
   }
}

STBIWDEF unsigned char *stbi_write_png_to_mem(const unsigned char *pixels, int stride_bytes, int x, int y, int n, int *out_len)
{
   int force_filter = stbi_write_force_png_filter;
   int ctype[5] = { -1, 0, 4, 2, 6 };
   unsigned char sig[8] = { 137,80,78,71,13,10,26,10 };
   unsigned char *out,*o, *filt, *zlib;
   signed char *line_buffer;
   int j,zlen;

   if (stride_bytes == 0)
      stride_bytes = x * n;

   if (force_filter >= 5) {
      force_filter = -1;
   }

   filt = (unsigned char *) STBIW_MALLOC((x*n+1) * y); if (!filt) return 0;
   line_buffer = (signed char *) STBIW_MALLOC(x * n); if (!line_buffer) { STBIW_FREE(filt); return 0; }
   for (j=0; j < y; ++j) {
      int filter_type;
      if (force_filter > -1) {
         filter_type = force_filter;
         stbiw__encode_png_line((unsigned char*)(pixels), stride_bytes, x, y, j, n, force_filter, line_buffer);
      } else { // Estimate the best filter by running through all of them:
         int best_filter = 0, best_filter_val = 0x7fffffff, est, i;
         for (filter_type = 0; filter_type < 5; filter_type++) {
            stbiw__encode_png_line((unsigned char*)(pixels), stride_bytes, x, y, j, n, filter_type, line_buffer);

            // Estimate the entropy of the line using this filter; the less, the better.
            est = 0;
            for (i = 0; i < x*n; ++i) {
               est += abs((signed char) line_buffer[i]);
            }
            if (est < best_filter_val) {
               best_filter_val = est;
               best_filter = filter_type;
            }
         }
         if (filter_type != best_filter) {  // If the last iteration already got us the best filter, don't redo it
            stbiw__encode_png_line((unsigned char*)(pixels), stride_bytes, x, y, j, n, best_filter, line_buffer);
            filter_type = best_filter;
         }
      }
      // when we get here, filter_type contains the filter type, and line_buffer contains the data
      filt[j*(x*n+1)] = (unsigned char) filter_type;
      STBIW_MEMMOVE(filt+j*(x*n+1)+1, line_buffer, x*n);
   }
   STBIW_FREE(line_buffer);
   zlib = stbi_zlib_compress(filt, y*( x*n+1), &zlen, stbi_write_png_compression_level);
   STBIW_FREE(filt);
   if (!zlib) return 0;

   // each tag requires 12 bytes of overhead
   out = (unsigned char *) STBIW_MALLOC(8 + 12+13 + 12+zlen + 12);
   if (!out) return 0;
   *out_len = 8 + 12+13 + 12+zlen + 12;

   o=out;
   STBIW_MEMMOVE(o,sig,8); o+= 8;
   stbiw__wp32(o, 13); // header length
   stbiw__wptag(o, "IHDR");
   stbiw__wp32(o, x);
   stbiw__wp32(o, y);
   *o++ = 8;
   *o++ = STBIW_UCHAR(ctype[n]);
   *o++ = 0;
   *o++ = 0;
   *o++ = 0;
   stbiw__wpcrc(&o,13);

   stbiw__wp32(o, zlen);
   stbiw__wptag(o, "IDAT");
   STBIW_MEMMOVE(o, zlib, zlen);
   o += zlen;
   STBIW_FREE(zlib);
   stbiw__wpcrc(&o, zlen);

   stbiw__wp32(o,0);
   stbiw__wptag(o, "IEND");
   stbiw__wpcrc(&o,0);

   STBIW_ASSERT(o == out + *out_len);

   return out;
}

#ifndef STBI_WRITE_NO_STDIO
STBIWDEF int stbi_write_png(char const *filename, int x, int y, int comp, const void *data, int stride_bytes)
{
   FILE *f;
   int len;
   unsigned char *png = stbi_write_png_to_mem((const unsigned char *) data, stride_bytes, x, y, comp, &len);
   if (png == NULL) return 0;

   f = stbiw__fopen(filename, "wb");
   if (!f) { STBIW_FREE(png); return 0; }
   fwrite(png, 1, len, f);
   fclose(f);
   STBIW_FREE(png);
   return 1;
}
#endif

STBIWDEF int stbi_write_png_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const void *data, int stride_bytes)
{
   int len;
   unsigned char *png = stbi_write_png_to_mem((const unsigned char *) data, stride_bytes, x, y, comp, &len);
   if (png == NULL) return 0;
   func(context, png, len);
   STBIW_FREE(png);
   return 1;
}


/* ***************************************************************************
 *
 * JPEG writer
 *
 * This is based on Jon Olick's jo_jpeg.cpp:
 * public domain Simple, Minimalistic JPEG writer - http://www.jonolick.com/code.html
 */

static const unsigned char stbiw__jpg_ZigZag[] = { 0,1,5,6,14,15,27,28,2,4,7,13,16,26,29,42,3,8,12,17,25,30,41,43,9,11,18,
      24,31,40,44,53,10,19,23,32,39,45,52,54,20,22,33,38,46,51,55,60,21,34,37,47,50,56,59,61,35,36,48,49,57,58,62,63 };

static void stbiw__jpg_writeBits(stbi__write_context *s, int *bitBufP, int *bitCntP, const unsigned short *bs) {
   int bitBuf = *bitBufP, bitCnt = *bitCntP;
   bitCnt += bs[1];
   bitBuf |= bs[0] << (24 - bitCnt);
   while(bitCnt >= 8) {
      unsigned char c = (bitBuf >> 16) & 255;
      stbiw__putc(s, c);
      if(c == 255) {
         stbiw__putc(s, 0);
      }
      bitBuf <<= 8;
      bitCnt -= 8;
   }
   *bitBufP = bitBuf;
   *bitCntP = bitCnt;
}

static void stbiw__jpg_DCT(float *d0p, float *d1p, float *d2p, float *d3p, float *d4p, float *d5p, float *d6p, float *d7p) {
   float d0 = *d0p, d1 = *d1p, d2 = *d2p, d3 = *d3p, d4 = *d4p, d5 = *d5p, d6 = *d6p, d7 = *d7p;
   float z1, z2, z3, z4, z5, z11, z13;

   float tmp0 = d0 + d7;
   float tmp7 = d0 - d7;
   float tmp1 = d1 + d6;
   float tmp6 = d1 - d6;
   float tmp2 = d2 + d5;
   float tmp5 = d2 - d5;
   float tmp3 = d3 + d4;
   float tmp4 = d3 - d4;

   // Even part
   float tmp10 = tmp0 + tmp3;   // phase 2
   float tmp13 = tmp0 - tmp3;
   float tmp11 = tmp1 + tmp2;
   float tmp12 = tmp1 - tmp2;

   d0 = tmp10 + tmp11;       // phase 3
   d4 = tmp10 - tmp11;

   z1 = (tmp12 + tmp13) * 0.707106781f; // c4
   d2 = tmp13 + z1;       // phase 5
   d6 = tmp13 - z1;

   // Odd part
   tmp10 = tmp4 + tmp5;       // phase 2
   tmp11 = tmp5 + tmp6;
   tmp12 = tmp6 + tmp7;

   // The rotator is modified from fig 4-8 to avoid extra negations.
   z5 = (tmp10 - tmp12) * 0.382683433f; // c6
   z2 = tmp10 * 0.541196100f + z5; // c2-c6
   z4 = tmp12 * 1.306562965f + z5; // c2+c6
   z3 = tmp11 * 0.707106781f; // c4

   z11 = tmp7 + z3;      // phase 5
   z13 = tmp7 - z3;

   *d5p = z13 + z2;         // phase 6
   *d3p = z13 - z2;
   *d1p = z11 + z4;
   *d7p = z11 - z4;

   *d0p = d0;  *d2p = d2;  *d4p = d4;  *d6p = d6;
}

static void stbiw__jpg_calcBits(int val, unsigned short bits[2]) {
   int tmp1 = val < 0 ? -val : val;
   val = val < 0 ? val-1 : val;
   bits[1] = 1;
   while(tmp1 >>= 1) {
      ++bits[1];
   }
   bits[0] = val & ((1<<bits[1])-1);
}

static int stbiw__jpg_processDU(stbi__write_context *s, int *bitBuf, int *bitCnt, float *CDU, int du_stride, float *fdtbl, int DC, const unsigned short HTDC[256][2], const unsigned short HTAC[256][2]) {
   const unsigned short EOB[2] = { HTAC[0x00][0], HTAC[0x00][1] };
   const unsigned short M16zeroes[2] = { HTAC[0xF0][0], HTAC[0xF0][1] };
   int dataOff, i, j, n, diff, end0pos, x, y;
   int DU[64];

   // DCT rows
   for(dataOff=0, n=du_stride*8; dataOff<n; dataOff+=du_stride) {
      stbiw__jpg_DCT(&CDU[dataOff], &CDU[dataOff+1], &CDU[dataOff+2], &CDU[dataOff+3], &CDU[dataOff+4], &CDU[dataOff+5], &CDU[dataOff+6], &CDU[dataOff+7]);
   }
   // DCT columns
   for(dataOff=0; dataOff<8; ++dataOff) {
      stbiw__jpg_DCT(&CDU[dataOff], &CDU[dataOff+du_stride], &CDU[dataOff+du_stride*2], &CDU[dataOff+du_stride*3], &CDU[dataOff+du_stride*4],
                     &CDU[dataOff+du_stride*5], &CDU[dataOff+du_stride*6], &CDU[dataOff+du_stride*7]);
   }
   // Quantize/descale/zigzag the coefficients
   for(y = 0, j=0; y < 8; ++y) {
      for(x = 0; x < 8; ++x,++j) {
         float v;
         i = y*du_stride+x;
         v = CDU[i]*fdtbl[j];
         // DU[stbiw__jpg_ZigZag[j]] = (int)(v < 0 ? ceilf(v - 0.5f) : floorf(v + 0.5f));
         // ceilf() and floorf() are C99, not C89, but I /think/ they're not needed here anyway?
         DU[stbiw__jpg_ZigZag[j]] = (int)(v < 0 ? v - 0.5f : v + 0.5f);
      }
   }

   // Encode DC
   diff = DU[0] - DC;
   if (diff == 0) {
      stbiw__jpg_writeBits(s, bitBuf, bitCnt, HTDC[0]);
   } else {
      unsigned short bits[2];
      stbiw__jpg_calcBits(diff, bits);
      stbiw__jpg_writeBits(s, bitBuf, bitCnt, HTDC[bits[1]]);
      stbiw__jpg_writeBits(s, bitBuf, bitCnt, bits);
   }
   // Encode ACs
   end0pos = 63;
   for(; (end0pos>0)&&(DU[end0pos]==0); --end0pos) {
   }
   // end0pos = first element in reverse order !=0
   if(end0pos == 0) {
      stbiw__jpg_writeBits(s, bitBuf, bitCnt, EOB);
      return DU[0];
   }
   for(i = 1; i <= end0pos; ++i) {
      int startpos = i;
      int nrzeroes;
      unsigned short bits[2];
      for (; DU[i]==0 && i<=end0pos; ++i) {
      }
      nrzeroes = i-startpos;
      if ( nrzeroes >= 16 ) {
         int lng = nrzeroes>>4;
         int nrmarker;
         for (nrmarker=1; nrmarker <= lng; ++nrmarker)
            stbiw__jpg_writeBits(s, bitBuf, bitCnt, M16zeroes);
         nrzeroes &= 15;
      }
      stbiw__jpg_calcBits(DU[i], bits);
      stbiw__jpg_writeBits(s, bitBuf, bitCnt, HTAC[(nrzeroes<<4)+bits[1]]);
      stbiw__jpg_writeBits(s, bitBuf, bitCnt, bits);
   }
   if(end0pos != 63) {
      stbiw__jpg_writeBits(s, bitBuf, bitCnt, EOB);
   }
   return DU[0];
}

static int stbi_write_jpg_core(stbi__write_context *s, int width, int height, int comp, const void* data, int quality) {
   // Constants that don't pollute global namespace
   static const unsigned char std_dc_luminance_nrcodes[] = {0,0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0};
   static const unsigned char std_dc_luminance_values[] = {0,1,2,3,4,5,6,7,8,9,10,11};
   static const unsigned char std_ac_luminance_nrcodes[] = {0,0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,0x7d};
   static const unsigned char std_ac_luminance_values[] = {
      0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,0x14,0x32,0x81,0x91,0xa1,0x08,
      0x23,0x42,0xb1,0xc1,0x15,0x52,0xd1,0xf0,0x24,0x33,0x62,0x72,0x82,0x09,0x0a,0x16,0x17,0x18,0x19,0x1a,0x25,0x26,0x27,0x28,
      0x29,0x2a,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
      0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
      0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,
      0xb7,0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xe1,0xe2,
      0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa
   };
   static const unsigned char std_dc_chrominance_nrcodes[] = {0,0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0};
   static const unsigned char std_dc_chrominance_values[] = {0,1,2,3,4,5,6,7,8,9,10,11};
   static const unsigned char std_ac_chrominance_nrcodes[] = {0,0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,0x77};
   static const unsigned char std_ac_chrominance_values[] = {
      0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,
      0xa1,0xb1,0xc1,0x09,0x23,0x33,0x52,0xf0,0x15,0x62,0x72,0xd1,0x0a,0x16,0x24,0x34,0xe1,0x25,0xf1,0x17,0x18,0x19,0x1a,0x26,
      0x27,0x28,0x29,0x2a,0x35,0x36,0x37,0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,
      0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x82,0x83,0x84,0x85,0x86,0x87,
      0x88,0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,
      0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,
      0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa
   };
   // Huffman tables
   static const unsigned short YDC_HT[256][2] = { {0,2},{2,3},{3,3},{4,3},{5,3},{6,3},{14,4},{30,5},{62,6},{126,7},{254,8},{510,9}};
   static const unsigned short UVDC_HT[256][2] = { {0,2},{1,2},{2,2},{6,3},{14,4},{30,5},{62,6},{126,7},{254,8},{510,9},{1022,10},{2046,11}};
   static const unsigned short YAC_HT[256][2] = {
      {10,4},{0,2},{1,2},{4,3},{11,4},{26,5},{120,7},{248,8},{1014,10},{65410,16},{65411,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {12,4},{27,5},{121,7},{502,9},{2038,11},{65412,16},{65413,16},{65414,16},{65415,16},{65416,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {28,5},{249,8},{1015,10},{4084,12},{65417,16},{65418,16},{65419,16},{65420,16},{65421,16},{65422,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {58,6},{503,9},{4085,12},{65423,16},{65424,16},{65425,16},{65426,16},{65427,16},{65428,16},{65429,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {59,6},{1016,10},{65430,16},{65431,16},{65432,16},{65433,16},{65434,16},{65435,16},{65436,16},{65437,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {122,7},{2039,11},{65438,16},{65439,16},{65440,16},{65441,16},{65442,16},{65443,16},{65444,16},{65445,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {123,7},{4086,12},{65446,16},{65447,16},{65448,16},{65449,16},{65450,16},{65451,16},{65452,16},{65453,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {250,8},{4087,12},{65454,16},{65455,16},{65456,16},{65457,16},{65458,16},{65459,16},{65460,16},{65461,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {504,9},{32704,15},{65462,16},{65463,16},{65464,16},{65465,16},{65466,16},{65467,16},{65468,16},{65469,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {505,9},{65470,16},{65471,16},{65472,16},{65473,16},{65474,16},{65475,16},{65476,16},{65477,16},{65478,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {506,9},{65479,16},{65480,16},{65481,16},{65482,16},{65483,16},{65484,16},{65485,16},{65486,16},{65487,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {1017,10},{65488,16},{65489,16},{65490,16},{65491,16},{65492,16},{65493,16},{65494,16},{65495,16},{65496,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {1018,10},{65497,16},{65498,16},{65499,16},{65500,16},{65501,16},{65502,16},{65503,16},{65504,16},{65505,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {2040,11},{65506,16},{65507,16},{65508,16},{65509,16},{65510,16},{65511,16},{65512,16},{65513,16},{65514,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {65515,16},{65516,16},{65517,16},{65518,16},{65519,16},{65520,16},{65521,16},{65522,16},{65523,16},{65524,16},{0,0},{0,0},{0,0},{0,0},{0,0},
      {2041,11},{65525,16},{65526,16},{65527,16},{65528,16},{65529,16},{65530,16},{65531,16},{65532,16},{65533,16},{65534,16},{0,0},{0,0},{0,0},{0,0},{0,0}
   };
   static const unsigned short UVAC_HT[256][2] = {
      {0,2},{1,2},{4,3},{10,4},{24,5},{25,5},{56,6},{120,7},{500,9},{1014,10},{4084,12},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {11,4},{57,6},{246,8},{501,9},{2038,11},{4085,12},{65416,16},{65417,16},{65418,16},{65419,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {26,5},{247,8},{1015,10},{4086,12},{32706,15},{65420,16},{65421,16},{65422,16},{65423,16},{65424,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {27,5},{248,8},{1016,10},{4087,12},{65425,16},{65426,16},{65427,16},{65428,16},{65429,16},{65430,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {58,6},{502,9},{65431,16},{65432,16},{65433,16},{65434,16},{65435,16},{65436,16},{65437,16},{65438,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {59,6},{1017,10},{65439,16},{65440,16},{65441,16},{65442,16},{65443,16},{65444,16},{65445,16},{65446,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {121,7},{2039,11},{65447,16},{65448,16},{65449,16},{65450,16},{65451,16},{65452,16},{65453,16},{65454,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {122,7},{2040,11},{65455,16},{65456,16},{65457,16},{65458,16},{65459,16},{65460,16},{65461,16},{65462,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {249,8},{65463,16},{65464,16},{65465,16},{65466,16},{65467,16},{65468,16},{65469,16},{65470,16},{65471,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {503,9},{65472,16},{65473,16},{65474,16},{65475,16},{65476,16},{65477,16},{65478,16},{65479,16},{65480,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {504,9},{65481,16},{65482,16},{65483,16},{65484,16},{65485,16},{65486,16},{65487,16},{65488,16},{65489,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {505,9},{65490,16},{65491,16},{65492,16},{65493,16},{65494,16},{65495,16},{65496,16},{65497,16},{65498,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {506,9},{65499,16},{65500,16},{65501,16},{65502,16},{65503,16},{65504,16},{65505,16},{65506,16},{65507,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {2041,11},{65508,16},{65509,16},{65510,16},{65511,16},{65512,16},{65513,16},{65514,16},{65515,16},{65516,16},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {16352,14},{65517,16},{65518,16},{65519,16},{65520,16},{65521,16},{65522,16},{65523,16},{65524,16},{65525,16},{0,0},{0,0},{0,0},{0,0},{0,0},
      {1018,10},{32707,15},{65526,16},{65527,16},{65528,16},{65529,16},{65530,16},{65531,16},{65532,16},{65533,16},{65534,16},{0,0},{0,0},{0,0},{0,0},{0,0}
   };
   static const int YQT[] = {16,11,10,16,24,40,51,61,12,12,14,19,26,58,60,55,14,13,16,24,40,57,69,56,14,17,22,29,51,87,80,62,18,22,
                             37,56,68,109,103,77,24,35,55,64,81,104,113,92,49,64,78,87,103,121,120,101,72,92,95,98,112,100,103,99};
   static const int UVQT[] = {17,18,24,47,99,99,99,99,18,21,26,66,99,99,99,99,24,26,56,99,99,99,99,99,47,66,99,99,99,99,99,99,
                              99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99};
   static const float aasf[] = { 1.0f * 2.828427125f, 1.387039845f * 2.828427125f, 1.306562965f * 2.828427125f, 1.175875602f * 2.828427125f,
                                 1.0f * 2.828427125f, 0.785694958f * 2.828427125f, 0.541196100f * 2.828427125f, 0.275899379f * 2.828427125f };

   int row, col, i, k, subsample;
   float fdtbl_Y[64], fdtbl_UV[64];
   unsigned char YTable[64], UVTable[64];

   if(!data || !width || !height || comp > 4 || comp < 1) {
      return 0;
   }

   quality = quality ? quality : 90;
   subsample = quality <= 90 ? 1 : 0;
   quality = quality < 1 ? 1 : quality > 100 ? 100 : quality;
   quality = quality < 50 ? 5000 / quality : 200 - quality * 2;

   for(i = 0; i < 64; ++i) {
      int uvti, yti = (YQT[i]*quality+50)/100;
      YTable[stbiw__jpg_ZigZag[i]] = (unsigned char) (yti < 1 ? 1 : yti > 255 ? 255 : yti);
      uvti = (UVQT[i]*quality+50)/100;
      UVTable[stbiw__jpg_ZigZag[i]] = (unsigned char) (uvti < 1 ? 1 : uvti > 255 ? 255 : uvti);
   }

   for(row = 0, k = 0; row < 8; ++row) {
      for(col = 0; col < 8; ++col, ++k) {
         fdtbl_Y[k]  = 1 / (YTable [stbiw__jpg_ZigZag[k]] * aasf[row] * aasf[col]);
         fdtbl_UV[k] = 1 / (UVTable[stbiw__jpg_ZigZag[k]] * aasf[row] * aasf[col]);
      }
   }

   // Write Headers
   {
      static const unsigned char head0[] = { 0xFF,0xD8,0xFF,0xE0,0,0x10,'J','F','I','F',0,1,1,0,0,1,0,1,0,0,0xFF,0xDB,0,0x84,0 };
      static const unsigned char head2[] = { 0xFF,0xDA,0,0xC,3,1,0,2,0x11,3,0x11,0,0x3F,0 };
      const unsigned char head1[] = { 0xFF,0xC0,0,0x11,8,(unsigned char)(height>>8),STBIW_UCHAR(height),(unsigned char)(width>>8),STBIW_UCHAR(width),
                                      3,1,(unsigned char)(subsample?0x22:0x11),0,2,0x11,1,3,0x11,1,0xFF,0xC4,0x01,0xA2,0 };
      s->func(s->context, (void*)head0, sizeof(head0));
      s->func(s->context, (void*)YTable, sizeof(YTable));
      stbiw__putc(s, 1);
      s->func(s->context, UVTable, sizeof(UVTable));
      s->func(s->context, (void*)head1, sizeof(head1));
      s->func(s->context, (void*)(std_dc_luminance_nrcodes+1), sizeof(std_dc_luminance_nrcodes)-1);
      s->func(s->context, (void*)std_dc_luminance_values, sizeof(std_dc_luminance_values));
      stbiw__putc(s, 0x10); // HTYACinfo
      s->func(s->context, (void*)(std_ac_luminance_nrcodes+1), sizeof(std_ac_luminance_nrcodes)-1);
      s->func(s->context, (void*)std_ac_luminance_values, sizeof(std_ac_luminance_values));
      stbiw__putc(s, 1); // HTUDCinfo
      s->func(s->context, (void*)(std_dc_chrominance_nrcodes+1), sizeof(std_dc_chrominance_nrcodes)-1);
      s->func(s->context, (void*)std_dc_chrominance_values, sizeof(std_dc_chrominance_values));
      stbiw__putc(s, 0x11); // HTUACinfo
      s->func(s->context, (void*)(std_ac_chrominance_nrcodes+1), sizeof(std_ac_chrominance_nrcodes)-1);
      s->func(s->context, (void*)std_ac_chrominance_values, sizeof(std_ac_chrominance_values));
      s->func(s->context, (void*)head2, sizeof(head2));
   }

   // Encode 8x8 macroblocks
   {
      static const unsigned short fillBits[] = {0x7F, 7};
      int DCY=0, DCU=0, DCV=0;
      int bitBuf=0, bitCnt=0;
      // comp == 2 is grey+alpha (alpha is ignored)
      int ofsG = comp > 2 ? 1 : 0, ofsB = comp > 2 ? 2 : 0;
      const unsigned char *dataR = (const unsigned char *)data;
      const unsigned char *dataG = dataR + ofsG;
      const unsigned char *dataB = dataR + ofsB;
      int x, y, pos;
      if(subsample) {
         for(y = 0; y < height; y += 16) {
            for(x = 0; x < width; x += 16) {
               float Y[256], U[256], V[256];
               for(row = y, pos = 0; row < y+16; ++row) {
                  // row >= height => use last input row
                  int clamped_row = (row < height) ? row : height - 1;
                  int base_p = (stbi__flip_vertically_on_write ? (height-1-clamped_row) : clamped_row)*width*comp;
                  for(col = x; col < x+16; ++col, ++pos) {
                     // if col >= width => use pixel from last input column
                     int p = base_p + ((col < width) ? col : (width-1))*comp;
                     float r = dataR[p], g = dataG[p], b = dataB[p];
                     Y[pos]= +0.29900f*r + 0.58700f*g + 0.11400f*b - 128;
                     U[pos]= -0.16874f*r - 0.33126f*g + 0.50000f*b;
                     V[pos]= +0.50000f*r - 0.41869f*g - 0.08131f*b;
                  }
               }
               DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y+0,   16, fdtbl_Y, DCY, YDC_HT, YAC_HT);
               DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y+8,   16, fdtbl_Y, DCY, YDC_HT, YAC_HT);
               DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y+128, 16, fdtbl_Y, DCY, YDC_HT, YAC_HT);
               DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y+136, 16, fdtbl_Y, DCY, YDC_HT, YAC_HT);

               // subsample U,V
               {
                  float subU[64], subV[64];
                  int yy, xx;
                  for(yy = 0, pos = 0; yy < 8; ++yy) {
                     for(xx = 0; xx < 8; ++xx, ++pos) {
                        int j = yy*32+xx*2;
                        subU[pos] = (U[j+0] + U[j+1] + U[j+16] + U[j+17]) * 0.25f;
                        subV[pos] = (V[j+0] + V[j+1] + V[j+16] + V[j+17]) * 0.25f;
                     }
                  }
                  DCU = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, subU, 8, fdtbl_UV, DCU, UVDC_HT, UVAC_HT);
                  DCV = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, subV, 8, fdtbl_UV, DCV, UVDC_HT, UVAC_HT);
               }
            }
         }
      } else {
         for(y = 0; y < height; y += 8) {
            for(x = 0; x < width; x += 8) {
               float Y[64], U[64], V[64];
               for(row = y, pos = 0; row < y+8; ++row) {
                  // row >= height => use last input row
                  int clamped_row = (row < height) ? row : height - 1;
                  int base_p = (stbi__flip_vertically_on_write ? (height-1-clamped_row) : clamped_row)*width*comp;
                  for(col = x; col < x+8; ++col, ++pos) {
                     // if col >= width => use pixel from last input column
                     int p = base_p + ((col < width) ? col : (width-1))*comp;
                     float r = dataR[p], g = dataG[p], b = dataB[p];
                     Y[pos]= +0.29900f*r + 0.58700f*g + 0.11400f*b - 128;
                     U[pos]= -0.16874f*r - 0.33126f*g + 0.50000f*b;
                     V[pos]= +0.50000f*r - 0.41869f*g - 0.08131f*b;
                  }
               }

               DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y, 8, fdtbl_Y,  DCY, YDC_HT, YAC_HT);
               DCU = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, U, 8, fdtbl_UV, DCU, UVDC_HT, UVAC_HT);
               DCV = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, V, 8, fdtbl_UV, DCV, UVDC_HT, UVAC_HT);
            }
         }
      }

      // Do the bit alignment of the EOI marker
      stbiw__jpg_writeBits(s, &bitBuf, &bitCnt, fillBits);
   }

   // EOI
   stbiw__putc(s, 0xFF);
   stbiw__putc(s, 0xD9);

   return 1;
}

STBIWDEF int stbi_write_jpg_to_func(stbi_write_func *func, void *context, int x, int y, int comp, const void *data, int quality)
{
   stbi__write_context s = { 0 };
   stbi__start_write_callbacks(&s, func, context);
   return stbi_write_jpg_core(&s, x, y, comp, (void *) data, quality);
}


#ifndef STBI_WRITE_NO_STDIO
STBIWDEF int stbi_write_jpg(char const *filename, int x, int y, int comp, const void *data, int quality)
{
   stbi__write_context s = { 0 };
   if (stbi__start_write_file(&s,filename)) {
      int r = stbi_write_jpg_core(&s, x, y, comp, data, quality);
      stbi__end_write_file(&s);
      return r;
   } else
      return 0;
}
#endif

#endif // STB_IMAGE_WRITE_IMPLEMENTATION

/* Revision history
      1.16  (2021-07-11)
             make Deflate code emit uncompressed blocks when it would otherwise expand
             support writing BMPs with alpha channel
      1.15  (2020-07-13) unknown
      1.14  (2020-02-02) updated JPEG writer to downsample chroma channels
      1.13
      1.12
      1.11  (2019-08-11)

      1.10  (2019-02-07)
             support utf8 filenames in Windows; fix warnings and platform ifdefs
      1.09  (2018-02-11)
             fix typo in zlib quality API, improve STB_I_W_STATIC in C++
      1.08  (2018-01-29)
             add stbi__flip_vertically_on_write, external zlib, zlib quality, choose PNG filter
      1.07  (2017-07-24)
             doc fix
      1.06 (2017-07-23)
             writing JPEG (using Jon Olick's code)
      1.05   ???
      1.04 (2017-03-03)
             monochrome BMP expansion
      1.03   ???
      1.02 (2016-04-02)
             avoid allocating large structures on the stack
      1.01 (2016-01-16)
             STBIW_REALLOC_SIZED: support allocators with no realloc support
             avoid race-condition in crc initialization
             minor compile issues
      1.00 (2015-09-14)
             installable file IO function
      0.99 (2015-09-13)
             warning fixes; TGA rle support
      0.98 (2015-04-08)
             added STBIW_MALLOC, STBIW_ASSERT etc
      0.97 (2015-01-18)
             fixed HDR asserts, rewrote HDR rle logic
      0.96 (2015-01-17)
             add HDR output
             fix monochrome BMP
      0.95 (2014-08-17)
             add monochrome TGA output
      0.94 (2014-05-31)
             rename private functions to avoid conflicts with stb_image.h
      0.93 (2014-05-27)
             warning fixes
      0.92 (2010-08-01)
             casts to unsigned char to fix warnings
      0.91 (2010-07-17)
             first public release
      0.90   first internal release
*/

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp continued                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/stb/stb_image.h included by src/immvision/internal/image.cpp    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* stb_image - v2.29 - public domain image loader - http://nothings.org/stb
                                  no warranty implied; use at your own risk

   Do this:
      #define STB_IMAGE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #include ...
   #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"

   You can #define STBI_ASSERT(x) before the #include to avoid using assert.h.
   And #define STBI_MALLOC, STBI_REALLOC, and STBI_FREE to avoid using malloc,realloc,free


   QUICK NOTES:
      Primarily of interest to game developers and other people who can
          avoid problematic images and only need the trivial interface

      JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
      PNG 1/2/4/8/16-bit-per-channel

      TGA (not sure what subset, if a subset)
      BMP non-1bpp, non-RLE
      PSD (composited view only, no extra channels, 8/16 bit-per-channel)

      GIF (*comp always reports as 4-channel)
      HDR (radiance rgbE format)
      PIC (Softimage PIC)
      PNM (PPM and PGM binary only)

      Animated GIF still needs a proper API, but here's one way to do it:
          http://gist.github.com/urraka/685d9a6340b26b830d49

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)

   Full documentation under "DOCUMENTATION" below.


LICENSE

  See end of file for license information.

RECENT REVISION HISTORY:

      2.29  (2023-05-xx) optimizations
      2.28  (2023-01-29) many error fixes, security errors, just tons of stuff
      2.27  (2021-07-11) document stbi_info better, 16-bit PNM support, bug fixes
      2.26  (2020-07-13) many minor fixes
      2.25  (2020-02-02) fix warnings
      2.24  (2020-02-02) fix warnings; thread-local failure_reason and flip_vertically
      2.23  (2019-08-11) fix clang static analysis warning
      2.22  (2019-03-04) gif fixes, fix warnings
      2.21  (2019-02-25) fix typo in comment
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings and platform ifdefs
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) bugfix, 1-bit BMP, 16-bitness query, fix warnings
      2.16  (2017-07-23) all functions have 16-bit variants; optimizations; bugfixes
      2.15  (2017-03-18) fix png-1,2,4; all Imagenet JPGs; no runtime SSE detection on GCC
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-12-04) experimental 16-bit API, only for PNG so far; fixes
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) 16-bit PNGS; enable SSE2 in non-gcc x64
                         RGB-format JPEG; remove white matting in PSD;
                         allocate large structures on the stack;
                         correct channel count for PNG & BMP
      2.10  (2016-01-22) avoid warning introduced in 2.09
      2.09  (2016-01-16) 16-bit TGA; comments in PNM files; STBI_REALLOC_SIZED

   See end of file for full revision history.


 ============================    Contributors    =========================

 Image formats                          Extensions, features
    Sean Barrett (jpeg, png, bmp)          Jetro Lauha (stbi_info)
    Nicolas Schulz (hdr, psd)              Martin "SpartanJ" Golini (stbi_info)
    Jonathan Dummer (tga)                  James "moose2000" Brown (iPhone PNG)
    Jean-Marc Lienher (gif)                Ben "Disch" Wenger (io callbacks)
    Tom Seddon (pic)                       Omar Cornut (1/2/4-bit PNG)
    Thatcher Ulrich (psd)                  Nicolas Guillemot (vertical flip)
    Ken Miller (pgm, ppm)                  Richard Mitton (16-bit PSD)
    github:urraka (animated gif)           Junggon Kim (PNM comments)
    Christopher Forseth (animated gif)     Daniel Gibson (16-bit TGA)
                                           socks-the-fox (16-bit PNG)
                                           Jeremy Sawicki (handle all ImageNet JPGs)
 Optimizations & bugfixes                  Mikhail Morozov (1-bit BMP)
    Fabian "ryg" Giesen                    Anael Seghezzi (is-16-bit query)
    Arseny Kapoulkine                      Simon Breuss (16-bit PNM)
    John-Mark Allen
    Carmelo J Fdez-Aguera

 Bug & warning fixes
    Marc LeBlanc            David Woo          Guillaume George     Martins Mozeiko
    Christpher Lloyd        Jerry Jansson      Joseph Thomson       Blazej Dariusz Roszkowski
    Phil Jordan                                Dave Moore           Roy Eltham
    Hayaki Saito            Nathan Reed        Won Chun
    Luke Graham             Johan Duparc       Nick Verigakis       the Horde3D community
    Thomas Ruf              Ronny Chevalier                         github:rlyeh
    Janez Zemva             John Bartholomew   Michal Cichon        github:romigrou
    Jonathan Blow           Ken Hamada         Tero Hanninen        github:svdijk
    Eugene Golushkov        Laurent Gomila     Cort Stratton        github:snagar
    Aruelien Pocheville     Sergio Gonzalez    Thibault Reuille     github:Zelex
    Cass Everitt            Ryamond Barbiero                        github:grim210
    Paul Du Bois            Engin Manap        Aldo Culquicondor    github:sammyhw
    Philipp Wiesemann       Dale Weiler        Oriol Ferrer Mesia   github:phprus
    Josh Tobin              Neil Bickford      Matthew Gregan       github:poppolopoppo
    Julian Raschke          Gregory Mullen     Christian Floisand   github:darealshinji
    Baldur Karlsson         Kevin Schmidt      JR Smith             github:Michaelangel007
                            Brad Weinberger    Matvey Cherevko      github:mosra
    Luca Sas                Alexander Veselov  Zack Middleton       [reserved]
    Ryan C. Gordon          [reserved]                              [reserved]
                     DO NOT ADD YOUR NAME HERE

                     Jacko Dirks

  To add your name to the credits, pick a random blank space in the middle and fill it.
  80% of merge conflicts on stb PRs are due to people adding their name at the end
  of the credits.
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Limitations:
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - GIF always returns *comp=4
//
// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data);
//
// Standard parameters:
//    int *x                 -- outputs image width in pixels
//    int *y                 -- outputs image height in pixels
//    int *channels_in_file  -- outputs # of image components in image file
//    int desired_channels   -- if non-zero, # of image components requested in result
//
// The return value from an image loader is an 'unsigned char *' which points
// to the pixel data, or NULL on an allocation failure or if the image is
// corrupt or invalid. The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components; the first
// pixel pointed to is top-left-most in the image. There is no padding between
// image scanlines or between pixels, regardless of format. The number of
// components N is 'desired_channels' if desired_channels is non-zero, or
// *channels_in_file otherwise. If desired_channels is non-zero,
// *channels_in_file has the number of components that _would_ have been
// output otherwise. E.g. if you set desired_channels to 4, you will always
// get RGBA output, but you can check *channels_in_file to see if it's trivially
// opaque because e.g. there were only 3 channels in the source image.
//
// An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
// If image loading fails for any reason, the return value will be NULL,
// and *x, *y, *channels_in_file will be unchanged. The function
// stbi_failure_reason() can be queried for an extremely brief, end-user
// unfriendly explanation of why the load failed. Define STBI_NO_FAILURE_STRINGS
// to avoid compiling these strings at all, and STBI_FAILURE_USERMSG to get slightly
// more user-friendly ones.
//
// Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
//
// To query the width, height and component count of an image without having to
// decode the full file, you can use the stbi_info family of functions:
//
//   int x,y,n,ok;
//   ok = stbi_info(filename, &x, &y, &n);
//   // returns ok=1 and sets x, y, n if image is a supported format,
//   // 0 otherwise.
//
// Note that stb_image pervasively uses ints in its public API for sizes,
// including sizes of memory buffers. This is now part of the API and thus
// hard to change without causing breakage. As a result, the various image
// loaders all have certain limits on image size; these differ somewhat
// by format but generally boil down to either just under 2GB or just under
// 1GB. When the decoded image would be larger than this, stb_image decoding
// will fail.
//
// Additionally, stb_image will reject image files that have any of their
// dimensions set to a larger value than the configurable STBI_MAX_DIMENSIONS,
// which defaults to 2**24 = 16777216 pixels. Due to the above memory limit,
// the only way to have an image with such dimensions load correctly
// is for it to have a rather extreme aspect ratio. Either way, the
// assumption here is that such larger images are likely to be malformed
// or malicious. If you do need to load an image with individual dimensions
// larger than that, and it still fits in the overall size limit, you can
// #define STBI_MAX_DIMENSIONS on your own to be something larger.
//
// ===========================================================================
//
// UNICODE:
//
//   If compiling for Windows and you wish to use Unicode filenames, compile
//   with
//       #define STBI_WINDOWS_UTF8
//   and pass utf8-encoded filenames. Call stbi_convert_wchar_to_utf8 to convert
//   Windows wchar_t filenames to utf8.
//
// ===========================================================================
//
// Philosophy
//
// stb libraries are designed with the following priorities:
//
//    1. easy to use
//    2. easy to maintain
//    3. good performance
//
// Sometimes I let "good performance" creep up in priority over "easy to maintain",
// and for best performance I may provide less-easy-to-use APIs that give higher
// performance, in addition to the easy-to-use ones. Nevertheless, it's important
// to keep in mind that from the standpoint of you, a client of this library,
// all you care about is #1 and #3, and stb libraries DO NOT emphasize #3 above all.
//
// Some secondary priorities arise directly from the first two, some of which
// provide more explicit reasons why performance can't be emphasized.
//
//    - Portable ("ease of use")
//    - Small source code footprint ("easy to maintain")
//    - No dependencies ("ease of use")
//
// ===========================================================================
//
// I/O callbacks
//
// I/O callbacks allow you to read from arbitrary sources, like packaged
// files or some other source. Data read from callbacks are processed
// through a small internal buffer (currently 128 bytes) to try to reduce
// overhead.
//
// The three functions you must define are "read" (reads some bytes of data),
// "skip" (skips some bytes of data), "eof" (reports if the stream is at the end).
//
// ===========================================================================
//
// SIMD support
//
// The JPEG decoder will try to automatically use SIMD kernels on x86 when
// supported by the compiler. For ARM Neon support, you must explicitly
// request it.
//
// (The old do-it-yourself SIMD API is no longer supported in the current
// code.)
//
// On x86, SSE2 will automatically be used when available based on a run-time
// test; if not, the generic C versions are used as a fall-back. On ARM targets,
// the typical path is to have separate builds for NEON and non-NEON devices
// (at least this is true for iOS and Android). Therefore, the NEON support is
// toggled by a build flag: define STBI_NEON to get NEON loops.
//
// If for some reason you do not want to use any of SIMD code, or if
// you have issues compiling it, you can disable it entirely by
// defining STBI_NO_SIMD.
//
// ===========================================================================
//
// HDR image support   (disable by defining STBI_NO_HDR)
//
// stb_image supports loading HDR images in general, and currently the Radiance
// .HDR file format specifically. You can still load any file through the existing
// interface; if you attempt to load an HDR file, it will be automatically remapped
// to LDR, assuming gamma 2.2 and an arbitrary scale factor defaulting to 1;
// both of these constants can be reconfigured through this interface:
//
//     stbi_hdr_to_ldr_gamma(2.2f);
//     stbi_hdr_to_ldr_scale(1.0f);
//
// (note, do not use _inverse_ constants; stbi_image will invert them
// appropriately).
//
// Additionally, there is a new, parallel interface for loading files as
// (linear) floats to preserve the full dynamic range:
//
//    float *data = stbi_loadf(filename, &x, &y, &n, 0);
//
// If you load LDR images through this interface, those images will
// be promoted to floating point values, run through the inverse of
// constants corresponding to the above:
//
//     stbi_ldr_to_hdr_scale(1.0f);
//     stbi_ldr_to_hdr_gamma(2.2f);
//
// Finally, given a filename (or an open file or memory block--see header
// file for details) containing image data, you can query for the "most
// appropriate" interface to use (that is, whether the image is HDR or
// not), using:
//
//     stbi_is_hdr(char *filename);
//
// ===========================================================================
//
// iPhone PNG support:
//
// We optionally support converting iPhone-formatted PNGs (which store
// premultiplied BGRA) back to RGB, even though they're internally encoded
// differently. To enable this conversion, call
// stbi_convert_iphone_png_to_rgb(1).
//
// Call stbi_set_unpremultiply_on_load(1) as well to force a divide per
// pixel to remove any premultiplied alpha *only* if the image file explicitly
// says there's premultiplied data (currently only happens in iPhone images,
// and only if iPhone convert-to-rgb processing is on).
//
// ===========================================================================
//
// ADDITIONAL CONFIGURATION
//
//  - You can suppress implementation of any of the decoders to reduce
//    your code footprint by #defining one or more of the following
//    symbols before creating the implementation.
//
//        STBI_NO_JPEG
//        STBI_NO_PNG
//        STBI_NO_BMP
//        STBI_NO_PSD
//        STBI_NO_TGA
//        STBI_NO_GIF
//        STBI_NO_HDR
//        STBI_NO_PIC
//        STBI_NO_PNM   (.ppm and .pgm)
//
//  - You can request *only* certain decoders and suppress all other ones
//    (this will be more forward-compatible, as addition of new decoders
//    doesn't require you to disable them explicitly):
//
//        STBI_ONLY_JPEG
//        STBI_ONLY_PNG
//        STBI_ONLY_BMP
//        STBI_ONLY_PSD
//        STBI_ONLY_TGA
//        STBI_ONLY_GIF
//        STBI_ONLY_HDR
//        STBI_ONLY_PIC
//        STBI_ONLY_PNM   (.ppm and .pgm)
//
//   - If you use STBI_NO_PNG (or _ONLY_ without PNG), and you still
//     want the zlib decoder to be available, #define STBI_SUPPORT_ZLIB
//
//  - If you define STBI_MAX_DIMENSIONS, stb_image will reject images greater
//    than that size (in either width or height) without further processing.
//    This is to let programs in the wild set an upper bound to prevent
//    denial-of-service attacks on untrusted data, as one could generate a
//    valid image of gigantic dimensions and force stb_image to allocate a
//    huge block of memory and spend disproportionate time decoding it. By
//    default this is set to (1 << 24), which is 16777216, but that's still
//    very big.

#ifndef STBI_NO_STDIO
#endif // STBI_NO_STDIO

#define STBI_VERSION 1

enum
{
   STBI_default = 0, // only used for desired_channels

   STBI_grey       = 1,
   STBI_grey_alpha = 2,
   STBI_rgb        = 3,
   STBI_rgb_alpha  = 4
};

typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STBIDEF
#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//
// PRIMARY API - works on images of any type
//

//
// load image by filename, open file, or memory buffer
//

typedef struct
{
   int      (*read)  (void *user,char *data,int size);   // fill 'data' with 'size' bytes.  return number of bytes actually read
   void     (*skip)  (void *user,int n);                 // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
   int      (*eof)   (void *user);                       // returns nonzero if we are at end of file/data
} stbi_io_callbacks;

////////////////////////////////////
//
// 8-bits-per-channel interface
//

STBIDEF stbi_uc *stbi_load_from_memory   (stbi_uc           const *buffer, int len   , int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_uc *stbi_load_from_callbacks(stbi_io_callbacks const *clbk  , void *user, int *x, int *y, int *channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_uc *stbi_load            (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_uc *stbi_load_from_file  (FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);
// for stbi_load_from_file, file pointer is left pointing immediately after image
#endif

#ifndef STBI_NO_GIF
STBIDEF stbi_uc *stbi_load_gif_from_memory(stbi_uc const *buffer, int len, int **delays, int *x, int *y, int *z, int *comp, int req_comp);
#endif

#ifdef STBI_WINDOWS_UTF8
STBIDEF int stbi_convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input);
#endif

////////////////////////////////////
//
// 16-bits-per-channel interface
//

STBIDEF stbi_us *stbi_load_16_from_memory   (stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_us *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_us *stbi_load_16          (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_us *stbi_load_from_file_16(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);
#endif

////////////////////////////////////
//
// float-per-channel interface
//
#ifndef STBI_NO_LINEAR
   STBIDEF float *stbi_loadf_from_memory     (stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
   STBIDEF float *stbi_loadf_from_callbacks  (stbi_io_callbacks const *clbk, void *user, int *x, int *y,  int *channels_in_file, int desired_channels);

   #ifndef STBI_NO_STDIO
   STBIDEF float *stbi_loadf            (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
   STBIDEF float *stbi_loadf_from_file  (FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);
   #endif
#endif

#ifndef STBI_NO_HDR
   STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma);
   STBIDEF void   stbi_hdr_to_ldr_scale(float scale);
#endif // STBI_NO_HDR

#ifndef STBI_NO_LINEAR
   STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma);
   STBIDEF void   stbi_ldr_to_hdr_scale(float scale);
#endif // STBI_NO_LINEAR

// stbi_is_hdr is always defined, but always returns false if STBI_NO_HDR
STBIDEF int    stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk, void *user);
STBIDEF int    stbi_is_hdr_from_memory(stbi_uc const *buffer, int len);
#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr          (char const *filename);
STBIDEF int      stbi_is_hdr_from_file(FILE *f);
#endif // STBI_NO_STDIO


// get a VERY brief reason for failure
// on most compilers (and ALL modern mainstream compilers) this is threadsafe
STBIDEF const char *stbi_failure_reason  (void);

// free the loaded image -- this is just free()
STBIDEF void     stbi_image_free      (void *retval_from_stbi_load);

// get image dimensions & components without fully decoding
STBIDEF int      stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp);
STBIDEF int      stbi_info_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp);
STBIDEF int      stbi_is_16_bit_from_memory(stbi_uc const *buffer, int len);
STBIDEF int      stbi_is_16_bit_from_callbacks(stbi_io_callbacks const *clbk, void *user);

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_info               (char const *filename,     int *x, int *y, int *comp);
STBIDEF int      stbi_info_from_file     (FILE *f,                  int *x, int *y, int *comp);
STBIDEF int      stbi_is_16_bit          (char const *filename);
STBIDEF int      stbi_is_16_bit_from_file(FILE *f);
#endif



// for image formats that explicitly notate that they have premultiplied alpha,
// we just return the colors as stored in the file. set this flag to force
// unpremultiplication. results are undefined if the unpremultiply overflow.
STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// indicate whether we should process iphone images back to canonical format,
// or just pass them through "as-is"
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// flip the image vertically, so the first pixel in the output array is the bottom left
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// as above, but only applies to images loaded on the thread that calls the function
// this function is only available if your compiler supports thread-local variables;
// calling it will fail to link if your compiler doesn't
STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply);
STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert);
STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip);

// ZLIB client - used by PNG, available for other purposes

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen);
STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header);
STBIDEF char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);

STBIDEF char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);


#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

#if defined(STBI_ONLY_JPEG) || defined(STBI_ONLY_PNG) || defined(STBI_ONLY_BMP) \
  || defined(STBI_ONLY_TGA) || defined(STBI_ONLY_GIF) || defined(STBI_ONLY_PSD) \
  || defined(STBI_ONLY_HDR) || defined(STBI_ONLY_PIC) || defined(STBI_ONLY_PNM) \
  || defined(STBI_ONLY_ZLIB)
   #ifndef STBI_ONLY_JPEG
   #define STBI_NO_JPEG
   #endif
   #ifndef STBI_ONLY_PNG
   #define STBI_NO_PNG
   #endif
   #ifndef STBI_ONLY_BMP
   #define STBI_NO_BMP
   #endif
   #ifndef STBI_ONLY_PSD
   #define STBI_NO_PSD
   #endif
   #ifndef STBI_ONLY_TGA
   #define STBI_NO_TGA
   #endif
   #ifndef STBI_ONLY_GIF
   #define STBI_NO_GIF
   #endif
   #ifndef STBI_ONLY_HDR
   #define STBI_NO_HDR
   #endif
   #ifndef STBI_ONLY_PIC
   #define STBI_NO_PIC
   #endif
   #ifndef STBI_ONLY_PNM
   #define STBI_NO_PNM
   #endif
#endif

#if defined(STBI_NO_PNG) && !defined(STBI_SUPPORT_ZLIB) && !defined(STBI_NO_ZLIB)
#define STBI_NO_ZLIB
#endif


#include <stddef.h> // ptrdiff_t on osx
#include <limits.h>

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR)
#include <math.h>  // ldexp, pow
#endif

#ifndef STBI_NO_STDIO
#endif

#ifndef STBI_ASSERT
#define STBI_ASSERT(x) assert(x)
#endif

#ifdef __cplusplus
#define STBI_EXTERN extern "C"
#else
#define STBI_EXTERN extern
#endif


#ifndef _MSC_VER
   #ifdef __cplusplus
   #define stbi_inline inline
   #else
   #define stbi_inline
   #endif
#else
   #define stbi_inline __forceinline
#endif

#ifndef STBI_NO_THREAD_LOCALS
   #if defined(__cplusplus) &&  __cplusplus >= 201103L
      #define STBI_THREAD_LOCAL       thread_local
   #elif defined(__GNUC__) && __GNUC__ < 5
      #define STBI_THREAD_LOCAL       __thread
   #elif defined(_MSC_VER)
      #define STBI_THREAD_LOCAL       __declspec(thread)
   #elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
      #define STBI_THREAD_LOCAL       _Thread_local
   #endif

   #ifndef STBI_THREAD_LOCAL
      #if defined(__GNUC__)
        #define STBI_THREAD_LOCAL       __thread
      #endif
   #endif
#endif

#if defined(_MSC_VER) || defined(__SYMBIAN32__)
typedef unsigned short stbi__uint16;
typedef   signed short stbi__int16;
typedef unsigned int   stbi__uint32;
typedef   signed int   stbi__int32;
#else
#include <stdint.h>
typedef uint16_t stbi__uint16;
typedef int16_t  stbi__int16;
typedef uint32_t stbi__uint32;
typedef int32_t  stbi__int32;
#endif

// should produce compiler error if size is wrong
typedef unsigned char validate_uint32[sizeof(stbi__uint32)==4 ? 1 : -1];

#ifdef _MSC_VER
#define STBI_NOTUSED(v)  (void)(v)
#else
#define STBI_NOTUSED(v)  (void)sizeof(v)
#endif

#ifdef _MSC_VER
#define STBI_HAS_LROTL
#endif

#ifdef STBI_HAS_LROTL
   #define stbi_lrot(x,y)  _lrotl(x,y)
#else
   #define stbi_lrot(x,y)  (((x) << (y)) | ((x) >> (-(y) & 31)))
#endif

#if defined(STBI_MALLOC) && defined(STBI_FREE) && (defined(STBI_REALLOC) || defined(STBI_REALLOC_SIZED))
// ok
#elif !defined(STBI_MALLOC) && !defined(STBI_FREE) && !defined(STBI_REALLOC) && !defined(STBI_REALLOC_SIZED)
// ok
#else
#error "Must define all or none of STBI_MALLOC, STBI_FREE, and STBI_REALLOC (or STBI_REALLOC_SIZED)."
#endif

#ifndef STBI_MALLOC
#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_REALLOC(p,newsz)     realloc(p,newsz)
#define STBI_FREE(p)              free(p)
#endif

#ifndef STBI_REALLOC_SIZED
#define STBI_REALLOC_SIZED(p,oldsz,newsz) STBI_REALLOC(p,newsz)
#endif

// x86/x64 detection
#if defined(__x86_64__) || defined(_M_X64)
#define STBI__X64_TARGET
#elif defined(__i386) || defined(_M_IX86)
#define STBI__X86_TARGET
#endif

#if defined(__GNUC__) && defined(STBI__X86_TARGET) && !defined(__SSE2__) && !defined(STBI_NO_SIMD)
// gcc doesn't support sse2 intrinsics unless you compile with -msse2,
// which in turn means it gets to use SSE2 everywhere. This is unfortunate,
// but previous attempts to provide the SSE2 functions with runtime
// detection caused numerous issues. The way architecture extensions are
// exposed in GCC/Clang is, sadly, not really suited for one-file libs.
// New behavior: if compiled with -msse2, we use SSE2 without any
// detection; if not, we don't use it at all.
#define STBI_NO_SIMD
#endif

#if defined(__MINGW32__) && defined(STBI__X86_TARGET) && !defined(STBI_MINGW_ENABLE_SSE2) && !defined(STBI_NO_SIMD)
// Note that __MINGW32__ doesn't actually mean 32-bit, so we have to avoid STBI__X64_TARGET
//
// 32-bit MinGW wants ESP to be 16-byte aligned, but this is not in the
// Windows ABI and VC++ as well as Windows DLLs don't maintain that invariant.
// As a result, enabling SSE2 on 32-bit MinGW is dangerous when not
// simultaneously enabling "-mstackrealign".
//
// See https://github.com/nothings/stb/issues/81 for more information.
//
// So default to no SSE2 on 32-bit MinGW. If you've read this far and added
// -mstackrealign to your build settings, feel free to #define STBI_MINGW_ENABLE_SSE2.
#define STBI_NO_SIMD
#endif

#if !defined(STBI_NO_SIMD) && (defined(STBI__X86_TARGET) || defined(STBI__X64_TARGET))
#define STBI_SSE2
#include <emmintrin.h>

#ifdef _MSC_VER

#if _MSC_VER >= 1400  // not VC6
#include <intrin.h> // __cpuid
static int stbi__cpuid3(void)
{
   int info[4];
   __cpuid(info,1);
   return info[3];
}
#else
static int stbi__cpuid3(void)
{
   int res;
   __asm {
      mov  eax,1
      cpuid
      mov  res,edx
   }
   return res;
}
#endif

#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
   int info3 = stbi__cpuid3();
   return ((info3 >> 26) & 1) != 0;
}
#endif

#else // assume GCC-style if not VC++
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
   // If we're even attempting to compile this on GCC/Clang, that means
   // -msse2 is on, which means the compiler is allowed to use SSE2
   // instructions at will, and so are we.
   return 1;
}
#endif

#endif
#endif

// ARM NEON
#if defined(STBI_NO_SIMD) && defined(STBI_NEON)
#undef STBI_NEON
#endif

#ifdef STBI_NEON
#include <arm_neon.h>
#ifdef _MSC_VER
#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name
#else
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))
#endif
#endif

#ifndef STBI_SIMD_ALIGN
#define STBI_SIMD_ALIGN(type, name) type name
#endif

#ifndef STBI_MAX_DIMENSIONS
#define STBI_MAX_DIMENSIONS (1 << 24)
#endif

///////////////////////////////////////////////
//
//  stbi__context struct and start_xxx functions

// stbi__context structure is our basic context used by all images, so it
// contains all the IO context, plus some basic image information
typedef struct
{
   stbi__uint32 img_x, img_y;
   int img_n, img_out_n;

   stbi_io_callbacks io;
   void *io_user_data;

   int read_from_callbacks;
   int buflen;
   stbi_uc buffer_start[128];
   int callback_already_read;

   stbi_uc *img_buffer, *img_buffer_end;
   stbi_uc *img_buffer_original, *img_buffer_original_end;
} stbi__context;


static void stbi__refill_buffer(stbi__context *s);

// initialize a memory-decode context
static void stbi__start_mem(stbi__context *s, stbi_uc const *buffer, int len)
{
   s->io.read = NULL;
   s->read_from_callbacks = 0;
   s->callback_already_read = 0;
   s->img_buffer = s->img_buffer_original = (stbi_uc *) buffer;
   s->img_buffer_end = s->img_buffer_original_end = (stbi_uc *) buffer+len;
}

// initialize a callback-based context
static void stbi__start_callbacks(stbi__context *s, stbi_io_callbacks *c, void *user)
{
   s->io = *c;
   s->io_user_data = user;
   s->buflen = sizeof(s->buffer_start);
   s->read_from_callbacks = 1;
   s->callback_already_read = 0;
   s->img_buffer = s->img_buffer_original = s->buffer_start;
   stbi__refill_buffer(s);
   s->img_buffer_original_end = s->img_buffer_end;
}

#ifndef STBI_NO_STDIO

static int stbi__stdio_read(void *user, char *data, int size)
{
   return (int) fread(data,1,size,(FILE*) user);
}

static void stbi__stdio_skip(void *user, int n)
{
   int ch;
   fseek((FILE*) user, n, SEEK_CUR);
   ch = fgetc((FILE*) user);  /* have to read a byte to reset feof()'s flag */
   if (ch != EOF) {
      ungetc(ch, (FILE *) user);  /* push byte back onto stream if valid. */
   }
}

static int stbi__stdio_eof(void *user)
{
   return feof((FILE*) user) || ferror((FILE *) user);
}

static stbi_io_callbacks stbi__stdio_callbacks =
{
   stbi__stdio_read,
   stbi__stdio_skip,
   stbi__stdio_eof,
};

static void stbi__start_file(stbi__context *s, FILE *f)
{
   stbi__start_callbacks(s, &stbi__stdio_callbacks, (void *) f);
}

//static void stop_file(stbi__context *s) { }

#endif // !STBI_NO_STDIO

static void stbi__rewind(stbi__context *s)
{
   // conceptually rewind SHOULD rewind to the beginning of the stream,
   // but we just rewind to the beginning of the initial buffer, because
   // we only use it after doing 'test', which only ever looks at at most 92 bytes
   s->img_buffer = s->img_buffer_original;
   s->img_buffer_end = s->img_buffer_original_end;
}

enum
{
   STBI_ORDER_RGB,
   STBI_ORDER_BGR
};

typedef struct
{
   int bits_per_channel;
   int num_channels;
   int channel_order;
} stbi__result_info;

#ifndef STBI_NO_JPEG
static int      stbi__jpeg_test(stbi__context *s);
static void    *stbi__jpeg_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__jpeg_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PNG
static int      stbi__png_test(stbi__context *s);
static void    *stbi__png_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__png_info(stbi__context *s, int *x, int *y, int *comp);
static int      stbi__png_is16(stbi__context *s);
#endif

#ifndef STBI_NO_BMP
static int      stbi__bmp_test(stbi__context *s);
static void    *stbi__bmp_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__bmp_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_TGA
static int      stbi__tga_test(stbi__context *s);
static void    *stbi__tga_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__tga_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PSD
static int      stbi__psd_test(stbi__context *s);
static void    *stbi__psd_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri, int bpc);
static int      stbi__psd_info(stbi__context *s, int *x, int *y, int *comp);
static int      stbi__psd_is16(stbi__context *s);
#endif

#ifndef STBI_NO_HDR
static int      stbi__hdr_test(stbi__context *s);
static float   *stbi__hdr_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__hdr_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PIC
static int      stbi__pic_test(stbi__context *s);
static void    *stbi__pic_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__pic_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_GIF
static int      stbi__gif_test(stbi__context *s);
static void    *stbi__gif_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static void    *stbi__load_gif_main(stbi__context *s, int **delays, int *x, int *y, int *z, int *comp, int req_comp);
static int      stbi__gif_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PNM
static int      stbi__pnm_test(stbi__context *s);
static void    *stbi__pnm_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__pnm_info(stbi__context *s, int *x, int *y, int *comp);
static int      stbi__pnm_is16(stbi__context *s);
#endif

static
#ifdef STBI_THREAD_LOCAL
STBI_THREAD_LOCAL
#endif
const char *stbi__g_failure_reason;

STBIDEF const char *stbi_failure_reason(void)
{
   return stbi__g_failure_reason;
}

#ifndef STBI_NO_FAILURE_STRINGS
static int stbi__err(const char *str)
{
   stbi__g_failure_reason = str;
   return 0;
}
#endif

static void *stbi__malloc(size_t size)
{
    return STBI_MALLOC(size);
}

// stb_image uses ints pervasively, including for offset calculations.
// therefore the largest decoded image size we can support with the
// current code, even on 64-bit targets, is INT_MAX. this is not a
// significant limitation for the intended use case.
//
// we do, however, need to make sure our size calculations don't
// overflow. hence a few helper functions for size calculations that
// multiply integers together, making sure that they're non-negative
// and no overflow occurs.

// return 1 if the sum is valid, 0 on overflow.
// negative terms are considered invalid.
static int stbi__addsizes_valid(int a, int b)
{
   if (b < 0) return 0;
   // now 0 <= b <= INT_MAX, hence also
   // 0 <= INT_MAX - b <= INTMAX.
   // And "a + b <= INT_MAX" (which might overflow) is the
   // same as a <= INT_MAX - b (no overflow)
   return a <= INT_MAX - b;
}

// returns 1 if the product is valid, 0 on overflow.
// negative factors are considered invalid.
static int stbi__mul2sizes_valid(int a, int b)
{
   if (a < 0 || b < 0) return 0;
   if (b == 0) return 1; // mul-by-0 is always safe
   // portable way to check for no overflows in a*b
   return a <= INT_MAX/b;
}

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// returns 1 if "a*b + add" has no negative terms/factors and doesn't overflow
static int stbi__mad2sizes_valid(int a, int b, int add)
{
   return stbi__mul2sizes_valid(a, b) && stbi__addsizes_valid(a*b, add);
}
#endif

// returns 1 if "a*b*c + add" has no negative terms/factors and doesn't overflow
static int stbi__mad3sizes_valid(int a, int b, int c, int add)
{
   return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a*b, c) &&
      stbi__addsizes_valid(a*b*c, add);
}

// returns 1 if "a*b*c*d + add" has no negative terms/factors and doesn't overflow
#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static int stbi__mad4sizes_valid(int a, int b, int c, int d, int add)
{
   return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a*b, c) &&
      stbi__mul2sizes_valid(a*b*c, d) && stbi__addsizes_valid(a*b*c*d, add);
}
#endif

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// mallocs with size overflow checking
static void *stbi__malloc_mad2(int a, int b, int add)
{
   if (!stbi__mad2sizes_valid(a, b, add)) return NULL;
   return stbi__malloc(a*b + add);
}
#endif

static void *stbi__malloc_mad3(int a, int b, int c, int add)
{
   if (!stbi__mad3sizes_valid(a, b, c, add)) return NULL;
   return stbi__malloc(a*b*c + add);
}

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static void *stbi__malloc_mad4(int a, int b, int c, int d, int add)
{
   if (!stbi__mad4sizes_valid(a, b, c, d, add)) return NULL;
   return stbi__malloc(a*b*c*d + add);
}
#endif

// returns 1 if the sum of two signed ints is valid (between -2^31 and 2^31-1 inclusive), 0 on overflow.
static int stbi__addints_valid(int a, int b)
{
   if ((a >= 0) != (b >= 0)) return 1; // a and b have different signs, so no overflow
   if (a < 0 && b < 0) return a >= INT_MIN - b; // same as a + b >= INT_MIN; INT_MIN - b cannot overflow since b < 0.
   return a <= INT_MAX - b;
}

// returns 1 if the product of two ints fits in a signed short, 0 on overflow.
static int stbi__mul2shorts_valid(int a, int b)
{
   if (b == 0 || b == -1) return 1; // multiplication by 0 is always 0; check for -1 so SHRT_MIN/b doesn't overflow
   if ((a >= 0) == (b >= 0)) return a <= SHRT_MAX/b; // product is positive, so similar to mul2sizes_valid
   if (b < 0) return a <= SHRT_MIN / b; // same as a * b >= SHRT_MIN
   return a >= SHRT_MIN / b;
}

// stbi__err - error
// stbi__errpf - error returning pointer to float
// stbi__errpuc - error returning pointer to unsigned char

#ifdef STBI_NO_FAILURE_STRINGS
   #define stbi__err(x,y)  0
#elif defined(STBI_FAILURE_USERMSG)
   #define stbi__err(x,y)  stbi__err(y)
#else
   #define stbi__err(x,y)  stbi__err(x)
#endif

#define stbi__errpf(x,y)   ((float *)(size_t) (stbi__err(x,y)?NULL:NULL))
#define stbi__errpuc(x,y)  ((unsigned char *)(size_t) (stbi__err(x,y)?NULL:NULL))

STBIDEF void stbi_image_free(void *retval_from_stbi_load)
{
   STBI_FREE(retval_from_stbi_load);
}

#ifndef STBI_NO_LINEAR
static float   *stbi__ldr_to_hdr(stbi_uc *data, int x, int y, int comp);
#endif

#ifndef STBI_NO_HDR
static stbi_uc *stbi__hdr_to_ldr(float   *data, int x, int y, int comp);
#endif

static int stbi__vertically_flip_on_load_global = 0;

STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
   stbi__vertically_flip_on_load_global = flag_true_if_should_flip;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__vertically_flip_on_load  stbi__vertically_flip_on_load_global
#else
static STBI_THREAD_LOCAL int stbi__vertically_flip_on_load_local, stbi__vertically_flip_on_load_set;

STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip)
{
   stbi__vertically_flip_on_load_local = flag_true_if_should_flip;
   stbi__vertically_flip_on_load_set = 1;
}

#define stbi__vertically_flip_on_load  (stbi__vertically_flip_on_load_set       \
                                         ? stbi__vertically_flip_on_load_local  \
                                         : stbi__vertically_flip_on_load_global)
#endif // STBI_THREAD_LOCAL

static void *stbi__load_main(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri, int bpc)
{
   memset(ri, 0, sizeof(*ri)); // make sure it's initialized if we add new fields
   ri->bits_per_channel = 8; // default is 8 so most paths don't have to be changed
   ri->channel_order = STBI_ORDER_RGB; // all current input & output are this, but this is here so we can add BGR order
   ri->num_channels = 0;

   // test the formats with a very explicit header first (at least a FOURCC
   // or distinctive magic number first)
   #ifndef STBI_NO_PNG
   if (stbi__png_test(s))  return stbi__png_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_BMP
   if (stbi__bmp_test(s))  return stbi__bmp_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_GIF
   if (stbi__gif_test(s))  return stbi__gif_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_PSD
   if (stbi__psd_test(s))  return stbi__psd_load(s,x,y,comp,req_comp, ri, bpc);
   #else
   STBI_NOTUSED(bpc);
   #endif
   #ifndef STBI_NO_PIC
   if (stbi__pic_test(s))  return stbi__pic_load(s,x,y,comp,req_comp, ri);
   #endif

   // then the formats that can end up attempting to load with just 1 or 2
   // bytes matching expectations; these are prone to false positives, so
   // try them later
   #ifndef STBI_NO_JPEG
   if (stbi__jpeg_test(s)) return stbi__jpeg_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_PNM
   if (stbi__pnm_test(s))  return stbi__pnm_load(s,x,y,comp,req_comp, ri);
   #endif

   #ifndef STBI_NO_HDR
   if (stbi__hdr_test(s)) {
      float *hdr = stbi__hdr_load(s, x,y,comp,req_comp, ri);
      return stbi__hdr_to_ldr(hdr, *x, *y, req_comp ? req_comp : *comp);
   }
   #endif

   #ifndef STBI_NO_TGA
   // test tga last because it's a crappy test!
   if (stbi__tga_test(s))
      return stbi__tga_load(s,x,y,comp,req_comp, ri);
   #endif

   return stbi__errpuc("unknown image type", "Image not of any known type, or corrupt");
}

static stbi_uc *stbi__convert_16_to_8(stbi__uint16 *orig, int w, int h, int channels)
{
   int i;
   int img_len = w * h * channels;
   stbi_uc *reduced;

   reduced = (stbi_uc *) stbi__malloc(img_len);
   if (reduced == NULL) return stbi__errpuc("outofmem", "Out of memory");

   for (i = 0; i < img_len; ++i)
      reduced[i] = (stbi_uc)((orig[i] >> 8) & 0xFF); // top half of each byte is sufficient approx of 16->8 bit scaling

   STBI_FREE(orig);
   return reduced;
}

static stbi__uint16 *stbi__convert_8_to_16(stbi_uc *orig, int w, int h, int channels)
{
   int i;
   int img_len = w * h * channels;
   stbi__uint16 *enlarged;

   enlarged = (stbi__uint16 *) stbi__malloc(img_len*2);
   if (enlarged == NULL) return (stbi__uint16 *) stbi__errpuc("outofmem", "Out of memory");

   for (i = 0; i < img_len; ++i)
      enlarged[i] = (stbi__uint16)((orig[i] << 8) + orig[i]); // replicate to high and low byte, maps 0->0, 255->0xffff

   STBI_FREE(orig);
   return enlarged;
}

static void stbi__vertical_flip(void *image, int w, int h, int bytes_per_pixel)
{
   int row;
   size_t bytes_per_row = (size_t)w * bytes_per_pixel;
   stbi_uc temp[2048];
   stbi_uc *bytes = (stbi_uc *)image;

   for (row = 0; row < (h>>1); row++) {
      stbi_uc *row0 = bytes + row*bytes_per_row;
      stbi_uc *row1 = bytes + (h - row - 1)*bytes_per_row;
      // swap row0 with row1
      size_t bytes_left = bytes_per_row;
      while (bytes_left) {
         size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
         memcpy(temp, row0, bytes_copy);
         memcpy(row0, row1, bytes_copy);
         memcpy(row1, temp, bytes_copy);
         row0 += bytes_copy;
         row1 += bytes_copy;
         bytes_left -= bytes_copy;
      }
   }
}

#ifndef STBI_NO_GIF
static void stbi__vertical_flip_slices(void *image, int w, int h, int z, int bytes_per_pixel)
{
   int slice;
   int slice_size = w * h * bytes_per_pixel;

   stbi_uc *bytes = (stbi_uc *)image;
   for (slice = 0; slice < z; ++slice) {
      stbi__vertical_flip(bytes, w, h, bytes_per_pixel);
      bytes += slice_size;
   }
}
#endif

static unsigned char *stbi__load_and_postprocess_8bit(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
   stbi__result_info ri;
   void *result = stbi__load_main(s, x, y, comp, req_comp, &ri, 8);

   if (result == NULL)
      return NULL;

   // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
   STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

   if (ri.bits_per_channel != 8) {
      result = stbi__convert_16_to_8((stbi__uint16 *) result, *x, *y, req_comp == 0 ? *comp : req_comp);
      ri.bits_per_channel = 8;
   }

   // @TODO: move stbi__convert_format to here

   if (stbi__vertically_flip_on_load) {
      int channels = req_comp ? req_comp : *comp;
      stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi_uc));
   }

   return (unsigned char *) result;
}

static stbi__uint16 *stbi__load_and_postprocess_16bit(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
   stbi__result_info ri;
   void *result = stbi__load_main(s, x, y, comp, req_comp, &ri, 16);

   if (result == NULL)
      return NULL;

   // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
   STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

   if (ri.bits_per_channel != 16) {
      result = stbi__convert_8_to_16((stbi_uc *) result, *x, *y, req_comp == 0 ? *comp : req_comp);
      ri.bits_per_channel = 16;
   }

   // @TODO: move stbi__convert_format16 to here
   // @TODO: special case RGB-to-Y (and RGBA-to-YA) for 8-bit-to-16-bit case to keep more precision

   if (stbi__vertically_flip_on_load) {
      int channels = req_comp ? req_comp : *comp;
      stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi__uint16));
   }

   return (stbi__uint16 *) result;
}

#if !defined(STBI_NO_HDR) && !defined(STBI_NO_LINEAR)
static void stbi__float_postprocess(float *result, int *x, int *y, int *comp, int req_comp)
{
   if (stbi__vertically_flip_on_load && result != NULL) {
      int channels = req_comp ? req_comp : *comp;
      stbi__vertical_flip(result, *x, *y, channels * sizeof(float));
   }
}
#endif

#ifndef STBI_NO_STDIO

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBI_EXTERN __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int cp, unsigned long flags, const char *str, int cbmb, wchar_t *widestr, int cchwide);
STBI_EXTERN __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags, const wchar_t *widestr, int cchwide, char *str, int cbmb, const char *defchar, int *used_default);
#endif

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBIDEF int stbi_convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input)
{
	return WideCharToMultiByte(65001 /* UTF8 */, 0, input, -1, buffer, (int) bufferlen, NULL, NULL);
}
#endif

static FILE *stbi__fopen(char const *filename, char const *mode)
{
   FILE *f;
#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
   wchar_t wMode[64];
   wchar_t wFilename[1024];
	if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, filename, -1, wFilename, sizeof(wFilename)/sizeof(*wFilename)))
      return 0;

	if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, mode, -1, wMode, sizeof(wMode)/sizeof(*wMode)))
      return 0;

#if defined(_MSC_VER) && _MSC_VER >= 1400
	if (0 != _wfopen_s(&f, wFilename, wMode))
		f = 0;
#else
   f = _wfopen(wFilename, wMode);
#endif

#elif defined(_MSC_VER) && _MSC_VER >= 1400
   if (0 != fopen_s(&f, filename, mode))
      f=0;
#else
   f = fopen(filename, mode);
#endif
   return f;
}


STBIDEF stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   FILE *f = stbi__fopen(filename, "rb");
   unsigned char *result;
   if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
   result = stbi_load_from_file(f,x,y,comp,req_comp);
   fclose(f);
   return result;
}

STBIDEF stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   unsigned char *result;
   stbi__context s;
   stbi__start_file(&s,f);
   result = stbi__load_and_postprocess_8bit(&s,x,y,comp,req_comp);
   if (result) {
      // need to 'unget' all the characters in the IO buffer
      fseek(f, - (int) (s.img_buffer_end - s.img_buffer), SEEK_CUR);
   }
   return result;
}

STBIDEF stbi__uint16 *stbi_load_from_file_16(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   stbi__uint16 *result;
   stbi__context s;
   stbi__start_file(&s,f);
   result = stbi__load_and_postprocess_16bit(&s,x,y,comp,req_comp);
   if (result) {
      // need to 'unget' all the characters in the IO buffer
      fseek(f, - (int) (s.img_buffer_end - s.img_buffer), SEEK_CUR);
   }
   return result;
}

STBIDEF stbi_us *stbi_load_16(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   FILE *f = stbi__fopen(filename, "rb");
   stbi__uint16 *result;
   if (!f) return (stbi_us *) stbi__errpuc("can't fopen", "Unable to open file");
   result = stbi_load_from_file_16(f,x,y,comp,req_comp);
   fclose(f);
   return result;
}


#endif //!STBI_NO_STDIO

STBIDEF stbi_us *stbi_load_16_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__load_and_postprocess_16bit(&s,x,y,channels_in_file,desired_channels);
}

STBIDEF stbi_us *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *channels_in_file, int desired_channels)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
   return stbi__load_and_postprocess_16bit(&s,x,y,channels_in_file,desired_channels);
}

STBIDEF stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__load_and_postprocess_8bit(&s,x,y,comp,req_comp);
}

STBIDEF stbi_uc *stbi_load_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__load_and_postprocess_8bit(&s,x,y,comp,req_comp);
}

#ifndef STBI_NO_GIF
STBIDEF stbi_uc *stbi_load_gif_from_memory(stbi_uc const *buffer, int len, int **delays, int *x, int *y, int *z, int *comp, int req_comp)
{
   unsigned char *result;
   stbi__context s;
   stbi__start_mem(&s,buffer,len);

   result = (unsigned char*) stbi__load_gif_main(&s, delays, x, y, z, comp, req_comp);
   if (stbi__vertically_flip_on_load) {
      stbi__vertical_flip_slices( result, *x, *y, *z, *comp );
   }

   return result;
}
#endif

#ifndef STBI_NO_LINEAR
static float *stbi__loadf_main(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
   unsigned char *data;
   #ifndef STBI_NO_HDR
   if (stbi__hdr_test(s)) {
      stbi__result_info ri;
      float *hdr_data = stbi__hdr_load(s,x,y,comp,req_comp, &ri);
      if (hdr_data)
         stbi__float_postprocess(hdr_data,x,y,comp,req_comp);
      return hdr_data;
   }
   #endif
   data = stbi__load_and_postprocess_8bit(s, x, y, comp, req_comp);
   if (data)
      return stbi__ldr_to_hdr(data, *x, *y, req_comp ? req_comp : *comp);
   return stbi__errpf("unknown image type", "Image not of any known type, or corrupt");
}

STBIDEF float *stbi_loadf_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__loadf_main(&s,x,y,comp,req_comp);
}

STBIDEF float *stbi_loadf_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__loadf_main(&s,x,y,comp,req_comp);
}

#ifndef STBI_NO_STDIO
STBIDEF float *stbi_loadf(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   float *result;
   FILE *f = stbi__fopen(filename, "rb");
   if (!f) return stbi__errpf("can't fopen", "Unable to open file");
   result = stbi_loadf_from_file(f,x,y,comp,req_comp);
   fclose(f);
   return result;
}

STBIDEF float *stbi_loadf_from_file(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_file(&s,f);
   return stbi__loadf_main(&s,x,y,comp,req_comp);
}
#endif // !STBI_NO_STDIO

#endif // !STBI_NO_LINEAR

// these is-hdr-or-not is defined independent of whether STBI_NO_LINEAR is
// defined, for API simplicity; if STBI_NO_LINEAR is defined, it always
// reports false!

STBIDEF int stbi_is_hdr_from_memory(stbi_uc const *buffer, int len)
{
   #ifndef STBI_NO_HDR
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__hdr_test(&s);
   #else
   STBI_NOTUSED(buffer);
   STBI_NOTUSED(len);
   return 0;
   #endif
}

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr          (char const *filename)
{
   FILE *f = stbi__fopen(filename, "rb");
   int result=0;
   if (f) {
      result = stbi_is_hdr_from_file(f);
      fclose(f);
   }
   return result;
}

STBIDEF int stbi_is_hdr_from_file(FILE *f)
{
   #ifndef STBI_NO_HDR
   long pos = ftell(f);
   int res;
   stbi__context s;
   stbi__start_file(&s,f);
   res = stbi__hdr_test(&s);
   fseek(f, pos, SEEK_SET);
   return res;
   #else
   STBI_NOTUSED(f);
   return 0;
   #endif
}
#endif // !STBI_NO_STDIO

STBIDEF int      stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk, void *user)
{
   #ifndef STBI_NO_HDR
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__hdr_test(&s);
   #else
   STBI_NOTUSED(clbk);
   STBI_NOTUSED(user);
   return 0;
   #endif
}

#ifndef STBI_NO_LINEAR
static float stbi__l2h_gamma=2.2f, stbi__l2h_scale=1.0f;

STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma) { stbi__l2h_gamma = gamma; }
STBIDEF void   stbi_ldr_to_hdr_scale(float scale) { stbi__l2h_scale = scale; }
#endif

static float stbi__h2l_gamma_i=1.0f/2.2f, stbi__h2l_scale_i=1.0f;

STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma) { stbi__h2l_gamma_i = 1/gamma; }
STBIDEF void   stbi_hdr_to_ldr_scale(float scale) { stbi__h2l_scale_i = 1/scale; }


//////////////////////////////////////////////////////////////////////////////
//
// Common code used by all image loaders
//

enum
{
   STBI__SCAN_load=0,
   STBI__SCAN_type,
   STBI__SCAN_header
};

static void stbi__refill_buffer(stbi__context *s)
{
   int n = (s->io.read)(s->io_user_data,(char*)s->buffer_start,s->buflen);
   s->callback_already_read += (int) (s->img_buffer - s->img_buffer_original);
   if (n == 0) {
      // at end of file, treat same as if from memory, but need to handle case
      // where s->img_buffer isn't pointing to safe memory, e.g. 0-byte file
      s->read_from_callbacks = 0;
      s->img_buffer = s->buffer_start;
      s->img_buffer_end = s->buffer_start+1;
      *s->img_buffer = 0;
   } else {
      s->img_buffer = s->buffer_start;
      s->img_buffer_end = s->buffer_start + n;
   }
}

stbi_inline static stbi_uc stbi__get8(stbi__context *s)
{
   if (s->img_buffer < s->img_buffer_end)
      return *s->img_buffer++;
   if (s->read_from_callbacks) {
      stbi__refill_buffer(s);
      return *s->img_buffer++;
   }
   return 0;
}

#if defined(STBI_NO_JPEG) && defined(STBI_NO_HDR) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
stbi_inline static int stbi__at_eof(stbi__context *s)
{
   if (s->io.read) {
      if (!(s->io.eof)(s->io_user_data)) return 0;
      // if feof() is true, check if buffer = end
      // special case: we've only got the special 0 character at the end
      if (s->read_from_callbacks == 0) return 1;
   }

   return s->img_buffer >= s->img_buffer_end;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC)
// nothing
#else
static void stbi__skip(stbi__context *s, int n)
{
   if (n == 0) return;  // already there!
   if (n < 0) {
      s->img_buffer = s->img_buffer_end;
      return;
   }
   if (s->io.read) {
      int blen = (int) (s->img_buffer_end - s->img_buffer);
      if (blen < n) {
         s->img_buffer = s->img_buffer_end;
         (s->io.skip)(s->io_user_data, n - blen);
         return;
      }
   }
   s->img_buffer += n;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_TGA) && defined(STBI_NO_HDR) && defined(STBI_NO_PNM)
// nothing
#else
static int stbi__getn(stbi__context *s, stbi_uc *buffer, int n)
{
   if (s->io.read) {
      int blen = (int) (s->img_buffer_end - s->img_buffer);
      if (blen < n) {
         int res, count;

         memcpy(buffer, s->img_buffer, blen);

         count = (s->io.read)(s->io_user_data, (char*) buffer + blen, n - blen);
         res = (count == (n-blen));
         s->img_buffer = s->img_buffer_end;
         return res;
      }
   }

   if (s->img_buffer+n <= s->img_buffer_end) {
      memcpy(buffer, s->img_buffer, n);
      s->img_buffer += n;
      return 1;
   } else
      return 0;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// nothing
#else
static int stbi__get16be(stbi__context *s)
{
   int z = stbi__get8(s);
   return (z << 8) + stbi__get8(s);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// nothing
#else
static stbi__uint32 stbi__get32be(stbi__context *s)
{
   stbi__uint32 z = stbi__get16be(s);
   return (z << 16) + stbi__get16be(s);
}
#endif

#if defined(STBI_NO_BMP) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF)
// nothing
#else
static int stbi__get16le(stbi__context *s)
{
   int z = stbi__get8(s);
   return z + (stbi__get8(s) << 8);
}
#endif

#ifndef STBI_NO_BMP
static stbi__uint32 stbi__get32le(stbi__context *s)
{
   stbi__uint32 z = stbi__get16le(s);
   z += (stbi__uint32)stbi__get16le(s) << 16;
   return z;
}
#endif

#define STBI__BYTECAST(x)  ((stbi_uc) ((x) & 255))  // truncate int to byte without warnings

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
//////////////////////////////////////////////////////////////////////////////
//
//  generic converter from built-in img_n to req_comp
//    individual types do this automatically as much as possible (e.g. jpeg
//    does all cases internally since it needs to colorspace convert anyway,
//    and it never has alpha, so very few cases ). png can automatically
//    interleave an alpha=255 channel, but falls back to this for other cases
//
//  assume data buffer is malloced, so malloc a new one and free that one
//  only failure mode is malloc failing

static stbi_uc stbi__compute_y(int r, int g, int b)
{
   return (stbi_uc) (((r*77) + (g*150) +  (29*b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
static unsigned char *stbi__convert_format(unsigned char *data, int img_n, int req_comp, unsigned int x, unsigned int y)
{
   int i,j;
   unsigned char *good;

   if (req_comp == img_n) return data;
   STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

   good = (unsigned char *) stbi__malloc_mad3(req_comp, x, y, 0);
   if (good == NULL) {
      STBI_FREE(data);
      return stbi__errpuc("outofmem", "Out of memory");
   }

   for (j=0; j < (int) y; ++j) {
      unsigned char *src  = data + j * x * img_n   ;
      unsigned char *dest = good + j * x * req_comp;

      #define STBI__COMBO(a,b)  ((a)*8+(b))
      #define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
      // convert source image with img_n components to one with req_comp components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch (STBI__COMBO(img_n, req_comp)) {
         STBI__CASE(1,2) { dest[0]=src[0]; dest[1]=255;                                     } break;
         STBI__CASE(1,3) { dest[0]=dest[1]=dest[2]=src[0];                                  } break;
         STBI__CASE(1,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=255;                     } break;
         STBI__CASE(2,1) { dest[0]=src[0];                                                  } break;
         STBI__CASE(2,3) { dest[0]=dest[1]=dest[2]=src[0];                                  } break;
         STBI__CASE(2,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=src[1];                  } break;
         STBI__CASE(3,4) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];dest[3]=255;        } break;
         STBI__CASE(3,1) { dest[0]=stbi__compute_y(src[0],src[1],src[2]);                   } break;
         STBI__CASE(3,2) { dest[0]=stbi__compute_y(src[0],src[1],src[2]); dest[1] = 255;    } break;
         STBI__CASE(4,1) { dest[0]=stbi__compute_y(src[0],src[1],src[2]);                   } break;
         STBI__CASE(4,2) { dest[0]=stbi__compute_y(src[0],src[1],src[2]); dest[1] = src[3]; } break;
         STBI__CASE(4,3) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];                    } break;
         default: STBI_ASSERT(0); STBI_FREE(data); STBI_FREE(good); return stbi__errpuc("unsupported", "Unsupported format conversion");
      }
      #undef STBI__CASE
   }

   STBI_FREE(data);
   return good;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// nothing
#else
static stbi__uint16 stbi__compute_y_16(int r, int g, int b)
{
   return (stbi__uint16) (((r*77) + (g*150) +  (29*b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// nothing
#else
static stbi__uint16 *stbi__convert_format16(stbi__uint16 *data, int img_n, int req_comp, unsigned int x, unsigned int y)
{
   int i,j;
   stbi__uint16 *good;

   if (req_comp == img_n) return data;
   STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

   good = (stbi__uint16 *) stbi__malloc(req_comp * x * y * 2);
   if (good == NULL) {
      STBI_FREE(data);
      return (stbi__uint16 *) stbi__errpuc("outofmem", "Out of memory");
   }

   for (j=0; j < (int) y; ++j) {
      stbi__uint16 *src  = data + j * x * img_n   ;
      stbi__uint16 *dest = good + j * x * req_comp;

      #define STBI__COMBO(a,b)  ((a)*8+(b))
      #define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
      // convert source image with img_n components to one with req_comp components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch (STBI__COMBO(img_n, req_comp)) {
         STBI__CASE(1,2) { dest[0]=src[0]; dest[1]=0xffff;                                     } break;
         STBI__CASE(1,3) { dest[0]=dest[1]=dest[2]=src[0];                                     } break;
         STBI__CASE(1,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=0xffff;                     } break;
         STBI__CASE(2,1) { dest[0]=src[0];                                                     } break;
         STBI__CASE(2,3) { dest[0]=dest[1]=dest[2]=src[0];                                     } break;
         STBI__CASE(2,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=src[1];                     } break;
         STBI__CASE(3,4) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];dest[3]=0xffff;        } break;
         STBI__CASE(3,1) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]);                   } break;
         STBI__CASE(3,2) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]); dest[1] = 0xffff; } break;
         STBI__CASE(4,1) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]);                   } break;
         STBI__CASE(4,2) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]); dest[1] = src[3]; } break;
         STBI__CASE(4,3) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];                       } break;
         default: STBI_ASSERT(0); STBI_FREE(data); STBI_FREE(good); return (stbi__uint16*) stbi__errpuc("unsupported", "Unsupported format conversion");
      }
      #undef STBI__CASE
   }

   STBI_FREE(data);
   return good;
}
#endif

#ifndef STBI_NO_LINEAR
static float   *stbi__ldr_to_hdr(stbi_uc *data, int x, int y, int comp)
{
   int i,k,n;
   float *output;
   if (!data) return NULL;
   output = (float *) stbi__malloc_mad4(x, y, comp, sizeof(float), 0);
   if (output == NULL) { STBI_FREE(data); return stbi__errpf("outofmem", "Out of memory"); }
   // compute number of non-alpha components
   if (comp & 1) n = comp; else n = comp-1;
   for (i=0; i < x*y; ++i) {
      for (k=0; k < n; ++k) {
         output[i*comp + k] = (float) (pow(data[i*comp+k]/255.0f, stbi__l2h_gamma) * stbi__l2h_scale);
      }
   }
   if (n < comp) {
      for (i=0; i < x*y; ++i) {
         output[i*comp + n] = data[i*comp + n]/255.0f;
      }
   }
   STBI_FREE(data);
   return output;
}
#endif

#ifndef STBI_NO_HDR
#define stbi__float2int(x)   ((int) (x))
static stbi_uc *stbi__hdr_to_ldr(float   *data, int x, int y, int comp)
{
   int i,k,n;
   stbi_uc *output;
   if (!data) return NULL;
   output = (stbi_uc *) stbi__malloc_mad3(x, y, comp, 0);
   if (output == NULL) { STBI_FREE(data); return stbi__errpuc("outofmem", "Out of memory"); }
   // compute number of non-alpha components
   if (comp & 1) n = comp; else n = comp-1;
   for (i=0; i < x*y; ++i) {
      for (k=0; k < n; ++k) {
         float z = (float) pow(data[i*comp+k]*stbi__h2l_scale_i, stbi__h2l_gamma_i) * 255 + 0.5f;
         if (z < 0) z = 0;
         if (z > 255) z = 255;
         output[i*comp + k] = (stbi_uc) stbi__float2int(z);
      }
      if (k < comp) {
         float z = data[i*comp+k] * 255 + 0.5f;
         if (z < 0) z = 0;
         if (z > 255) z = 255;
         output[i*comp + k] = (stbi_uc) stbi__float2int(z);
      }
   }
   STBI_FREE(data);
   return output;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  "baseline" JPEG/JFIF decoder
//
//    simple implementation
//      - doesn't support delayed output of y-dimension
//      - simple interface (only one output format: 8-bit interleaved RGB)
//      - doesn't try to recover corrupt jpegs
//      - doesn't allow partial loading, loading multiple at once
//      - still fast on x86 (copying globals into locals doesn't help x86)
//      - allocates lots of intermediate memory (full size of all components)
//        - non-interleaved case requires this anyway
//        - allows good upsampling (see next)
//    high-quality
//      - upsampled channels are bilinearly interpolated, even across blocks
//      - quality integer IDCT derived from IJG's 'slow'
//    performance
//      - fast huffman; reasonable integer IDCT
//      - some SIMD kernels for common paths on targets with SSE2/NEON
//      - uses a lot of intermediate memory, could cache poorly

#ifndef STBI_NO_JPEG

// huffman decoding acceleration
#define FAST_BITS   9  // larger handles more cases; smaller stomps less cache

typedef struct
{
   stbi_uc  fast[1 << FAST_BITS];
   // weirdly, repacking this into AoS is a 10% speed loss, instead of a win
   stbi__uint16 code[256];
   stbi_uc  values[256];
   stbi_uc  size[257];
   unsigned int maxcode[18];
   int    delta[17];   // old 'firstsymbol' - old 'firstcode'
} stbi__huffman;

typedef struct
{
   stbi__context *s;
   stbi__huffman huff_dc[4];
   stbi__huffman huff_ac[4];
   stbi__uint16 dequant[4][64];
   stbi__int16 fast_ac[4][1 << FAST_BITS];

// sizes for components, interleaved MCUs
   int img_h_max, img_v_max;
   int img_mcu_x, img_mcu_y;
   int img_mcu_w, img_mcu_h;

// definition of jpeg image component
   struct
   {
      int id;
      int h,v;
      int tq;
      int hd,ha;
      int dc_pred;

      int x,y,w2,h2;
      stbi_uc *data;
      void *raw_data, *raw_coeff;
      stbi_uc *linebuf;
      short   *coeff;   // progressive only
      int      coeff_w, coeff_h; // number of 8x8 coefficient blocks
   } img_comp[4];

   stbi__uint32   code_buffer; // jpeg entropy-coded buffer
   int            code_bits;   // number of valid bits
   unsigned char  marker;      // marker seen while filling entropy buffer
   int            nomore;      // flag if we saw a marker so must stop

   int            progressive;
   int            spec_start;
   int            spec_end;
   int            succ_high;
   int            succ_low;
   int            eob_run;
   int            jfif;
   int            app14_color_transform; // Adobe APP14 tag
   int            rgb;

   int scan_n, order[4];
   int restart_interval, todo;

// kernels
   void (*idct_block_kernel)(stbi_uc *out, int out_stride, short data[64]);
   void (*YCbCr_to_RGB_kernel)(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step);
   stbi_uc *(*resample_row_hv_2_kernel)(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
} stbi__jpeg;

static int stbi__build_huffman(stbi__huffman *h, int *count)
{
   int i,j,k=0;
   unsigned int code;
   // build size list for each symbol (from JPEG spec)
   for (i=0; i < 16; ++i) {
      for (j=0; j < count[i]; ++j) {
         h->size[k++] = (stbi_uc) (i+1);
         if(k >= 257) return stbi__err("bad size list","Corrupt JPEG");
      }
   }
   h->size[k] = 0;

   // compute actual symbols (from jpeg spec)
   code = 0;
   k = 0;
   for(j=1; j <= 16; ++j) {
      // compute delta to add to code to compute symbol id
      h->delta[j] = k - code;
      if (h->size[k] == j) {
         while (h->size[k] == j)
            h->code[k++] = (stbi__uint16) (code++);
         if (code-1 >= (1u << j)) return stbi__err("bad code lengths","Corrupt JPEG");
      }
      // compute largest code + 1 for this size, preshifted as needed later
      h->maxcode[j] = code << (16-j);
      code <<= 1;
   }
   h->maxcode[j] = 0xffffffff;

   // build non-spec acceleration table; 255 is flag for not-accelerated
   memset(h->fast, 255, 1 << FAST_BITS);
   for (i=0; i < k; ++i) {
      int s = h->size[i];
      if (s <= FAST_BITS) {
         int c = h->code[i] << (FAST_BITS-s);
         int m = 1 << (FAST_BITS-s);
         for (j=0; j < m; ++j) {
            h->fast[c+j] = (stbi_uc) i;
         }
      }
   }
   return 1;
}

// build a table that decodes both magnitude and value of small ACs in
// one go.
static void stbi__build_fast_ac(stbi__int16 *fast_ac, stbi__huffman *h)
{
   int i;
   for (i=0; i < (1 << FAST_BITS); ++i) {
      stbi_uc fast = h->fast[i];
      fast_ac[i] = 0;
      if (fast < 255) {
         int rs = h->values[fast];
         int run = (rs >> 4) & 15;
         int magbits = rs & 15;
         int len = h->size[fast];

         if (magbits && len + magbits <= FAST_BITS) {
            // magnitude code followed by receive_extend code
            int k = ((i << len) & ((1 << FAST_BITS) - 1)) >> (FAST_BITS - magbits);
            int m = 1 << (magbits - 1);
            if (k < m) k += (~0U << magbits) + 1;
            // if the result is small enough, we can fit it in fast_ac table
            if (k >= -128 && k <= 127)
               fast_ac[i] = (stbi__int16) ((k * 256) + (run * 16) + (len + magbits));
         }
      }
   }
}

static void stbi__grow_buffer_unsafe(stbi__jpeg *j)
{
   do {
      unsigned int b = j->nomore ? 0 : stbi__get8(j->s);
      if (b == 0xff) {
         int c = stbi__get8(j->s);
         while (c == 0xff) c = stbi__get8(j->s); // consume fill bytes
         if (c != 0) {
            j->marker = (unsigned char) c;
            j->nomore = 1;
            return;
         }
      }
      j->code_buffer |= b << (24 - j->code_bits);
      j->code_bits += 8;
   } while (j->code_bits <= 24);
}

// (1 << n) - 1
static const stbi__uint32 stbi__bmask[17]={0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535};

// decode a jpeg huffman value from the bitstream
stbi_inline static int stbi__jpeg_huff_decode(stbi__jpeg *j, stbi__huffman *h)
{
   unsigned int temp;
   int c,k;

   if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);

   // look at the top FAST_BITS and determine what symbol ID it is,
   // if the code is <= FAST_BITS
   c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS)-1);
   k = h->fast[c];
   if (k < 255) {
      int s = h->size[k];
      if (s > j->code_bits)
         return -1;
      j->code_buffer <<= s;
      j->code_bits -= s;
      return h->values[k];
   }

   // naive test is to shift the code_buffer down so k bits are
   // valid, then test against maxcode. To speed this up, we've
   // preshifted maxcode left so that it has (16-k) 0s at the
   // end; in other words, regardless of the number of bits, it
   // wants to be compared against something shifted to have 16;
   // that way we don't need to shift inside the loop.
   temp = j->code_buffer >> 16;
   for (k=FAST_BITS+1 ; ; ++k)
      if (temp < h->maxcode[k])
         break;
   if (k == 17) {
      // error! code not found
      j->code_bits -= 16;
      return -1;
   }

   if (k > j->code_bits)
      return -1;

   // convert the huffman code to the symbol id
   c = ((j->code_buffer >> (32 - k)) & stbi__bmask[k]) + h->delta[k];
   if(c < 0 || c >= 256) // symbol id out of bounds!
       return -1;
   STBI_ASSERT((((j->code_buffer) >> (32 - h->size[c])) & stbi__bmask[h->size[c]]) == h->code[c]);

   // convert the id to a symbol
   j->code_bits -= k;
   j->code_buffer <<= k;
   return h->values[c];
}

// bias[n] = (-1<<n) + 1
static const int stbi__jbias[16] = {0,-1,-3,-7,-15,-31,-63,-127,-255,-511,-1023,-2047,-4095,-8191,-16383,-32767};

// combined JPEG 'receive' and JPEG 'extend', since baseline
// always extends everything it receives.
stbi_inline static int stbi__extend_receive(stbi__jpeg *j, int n)
{
   unsigned int k;
   int sgn;
   if (j->code_bits < n) stbi__grow_buffer_unsafe(j);
   if (j->code_bits < n) return 0; // ran out of bits from stream, return 0s intead of continuing

   sgn = j->code_buffer >> 31; // sign bit always in MSB; 0 if MSB clear (positive), 1 if MSB set (negative)
   k = stbi_lrot(j->code_buffer, n);
   j->code_buffer = k & ~stbi__bmask[n];
   k &= stbi__bmask[n];
   j->code_bits -= n;
   return k + (stbi__jbias[n] & (sgn - 1));
}

// get some unsigned bits
stbi_inline static int stbi__jpeg_get_bits(stbi__jpeg *j, int n)
{
   unsigned int k;
   if (j->code_bits < n) stbi__grow_buffer_unsafe(j);
   if (j->code_bits < n) return 0; // ran out of bits from stream, return 0s intead of continuing
   k = stbi_lrot(j->code_buffer, n);
   j->code_buffer = k & ~stbi__bmask[n];
   k &= stbi__bmask[n];
   j->code_bits -= n;
   return k;
}

stbi_inline static int stbi__jpeg_get_bit(stbi__jpeg *j)
{
   unsigned int k;
   if (j->code_bits < 1) stbi__grow_buffer_unsafe(j);
   if (j->code_bits < 1) return 0; // ran out of bits from stream, return 0s intead of continuing
   k = j->code_buffer;
   j->code_buffer <<= 1;
   --j->code_bits;
   return k & 0x80000000;
}

// given a value that's at position X in the zigzag stream,
// where does it appear in the 8x8 matrix coded as row-major?
static const stbi_uc stbi__jpeg_dezigzag[64+15] =
{
    0,  1,  8, 16,  9,  2,  3, 10,
   17, 24, 32, 25, 18, 11,  4,  5,
   12, 19, 26, 33, 40, 48, 41, 34,
   27, 20, 13,  6,  7, 14, 21, 28,
   35, 42, 49, 56, 57, 50, 43, 36,
   29, 22, 15, 23, 30, 37, 44, 51,
   58, 59, 52, 45, 38, 31, 39, 46,
   53, 60, 61, 54, 47, 55, 62, 63,
   // let corrupt input sample past end
   63, 63, 63, 63, 63, 63, 63, 63,
   63, 63, 63, 63, 63, 63, 63
};

// decode one 64-entry block--
static int stbi__jpeg_decode_block(stbi__jpeg *j, short data[64], stbi__huffman *hdc, stbi__huffman *hac, stbi__int16 *fac, int b, stbi__uint16 *dequant)
{
   int diff,dc,k;
   int t;

   if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
   t = stbi__jpeg_huff_decode(j, hdc);
   if (t < 0 || t > 15) return stbi__err("bad huffman code","Corrupt JPEG");

   // 0 all the ac values now so we can do it 32-bits at a time
   memset(data,0,64*sizeof(data[0]));

   diff = t ? stbi__extend_receive(j, t) : 0;
   if (!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) return stbi__err("bad delta","Corrupt JPEG");
   dc = j->img_comp[b].dc_pred + diff;
   j->img_comp[b].dc_pred = dc;
   if (!stbi__mul2shorts_valid(dc, dequant[0])) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
   data[0] = (short) (dc * dequant[0]);

   // decode AC components, see JPEG spec
   k = 1;
   do {
      unsigned int zig;
      int c,r,s;
      if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
      c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS)-1);
      r = fac[c];
      if (r) { // fast-AC path
         k += (r >> 4) & 15; // run
         s = r & 15; // combined length
         if (s > j->code_bits) return stbi__err("bad huffman code", "Combined length longer than code bits available");
         j->code_buffer <<= s;
         j->code_bits -= s;
         // decode into unzigzag'd location
         zig = stbi__jpeg_dezigzag[k++];
         data[zig] = (short) ((r >> 8) * dequant[zig]);
      } else {
         int rs = stbi__jpeg_huff_decode(j, hac);
         if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
         s = rs & 15;
         r = rs >> 4;
         if (s == 0) {
            if (rs != 0xf0) break; // end block
            k += 16;
         } else {
            k += r;
            // decode into unzigzag'd location
            zig = stbi__jpeg_dezigzag[k++];
            data[zig] = (short) (stbi__extend_receive(j,s) * dequant[zig]);
         }
      }
   } while (k < 64);
   return 1;
}

static int stbi__jpeg_decode_block_prog_dc(stbi__jpeg *j, short data[64], stbi__huffman *hdc, int b)
{
   int diff,dc;
   int t;
   if (j->spec_end != 0) return stbi__err("can't merge dc and ac", "Corrupt JPEG");

   if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);

   if (j->succ_high == 0) {
      // first scan for DC coefficient, must be first
      memset(data,0,64*sizeof(data[0])); // 0 all the ac values now
      t = stbi__jpeg_huff_decode(j, hdc);
      if (t < 0 || t > 15) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
      diff = t ? stbi__extend_receive(j, t) : 0;

      if (!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) return stbi__err("bad delta", "Corrupt JPEG");
      dc = j->img_comp[b].dc_pred + diff;
      j->img_comp[b].dc_pred = dc;
      if (!stbi__mul2shorts_valid(dc, 1 << j->succ_low)) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
      data[0] = (short) (dc * (1 << j->succ_low));
   } else {
      // refinement scan for DC coefficient
      if (stbi__jpeg_get_bit(j))
         data[0] += (short) (1 << j->succ_low);
   }
   return 1;
}

// @OPTIMIZE: store non-zigzagged during the decode passes,
// and only de-zigzag when dequantizing
static int stbi__jpeg_decode_block_prog_ac(stbi__jpeg *j, short data[64], stbi__huffman *hac, stbi__int16 *fac)
{
   int k;
   if (j->spec_start == 0) return stbi__err("can't merge dc and ac", "Corrupt JPEG");

   if (j->succ_high == 0) {
      int shift = j->succ_low;

      if (j->eob_run) {
         --j->eob_run;
         return 1;
      }

      k = j->spec_start;
      do {
         unsigned int zig;
         int c,r,s;
         if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
         c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS)-1);
         r = fac[c];
         if (r) { // fast-AC path
            k += (r >> 4) & 15; // run
            s = r & 15; // combined length
            if (s > j->code_bits) return stbi__err("bad huffman code", "Combined length longer than code bits available");
            j->code_buffer <<= s;
            j->code_bits -= s;
            zig = stbi__jpeg_dezigzag[k++];
            data[zig] = (short) ((r >> 8) * (1 << shift));
         } else {
            int rs = stbi__jpeg_huff_decode(j, hac);
            if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
            s = rs & 15;
            r = rs >> 4;
            if (s == 0) {
               if (r < 15) {
                  j->eob_run = (1 << r);
                  if (r)
                     j->eob_run += stbi__jpeg_get_bits(j, r);
                  --j->eob_run;
                  break;
               }
               k += 16;
            } else {
               k += r;
               zig = stbi__jpeg_dezigzag[k++];
               data[zig] = (short) (stbi__extend_receive(j,s) * (1 << shift));
            }
         }
      } while (k <= j->spec_end);
   } else {
      // refinement scan for these AC coefficients

      short bit = (short) (1 << j->succ_low);

      if (j->eob_run) {
         --j->eob_run;
         for (k = j->spec_start; k <= j->spec_end; ++k) {
            short *p = &data[stbi__jpeg_dezigzag[k]];
            if (*p != 0)
               if (stbi__jpeg_get_bit(j))
                  if ((*p & bit)==0) {
                     if (*p > 0)
                        *p += bit;
                     else
                        *p -= bit;
                  }
         }
      } else {
         k = j->spec_start;
         do {
            int r,s;
            int rs = stbi__jpeg_huff_decode(j, hac); // @OPTIMIZE see if we can use the fast path here, advance-by-r is so slow, eh
            if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
            s = rs & 15;
            r = rs >> 4;
            if (s == 0) {
               if (r < 15) {
                  j->eob_run = (1 << r) - 1;
                  if (r)
                     j->eob_run += stbi__jpeg_get_bits(j, r);
                  r = 64; // force end of block
               } else {
                  // r=15 s=0 should write 16 0s, so we just do
                  // a run of 15 0s and then write s (which is 0),
                  // so we don't have to do anything special here
               }
            } else {
               if (s != 1) return stbi__err("bad huffman code", "Corrupt JPEG");
               // sign bit
               if (stbi__jpeg_get_bit(j))
                  s = bit;
               else
                  s = -bit;
            }

            // advance by r
            while (k <= j->spec_end) {
               short *p = &data[stbi__jpeg_dezigzag[k++]];
               if (*p != 0) {
                  if (stbi__jpeg_get_bit(j))
                     if ((*p & bit)==0) {
                        if (*p > 0)
                           *p += bit;
                        else
                           *p -= bit;
                     }
               } else {
                  if (r == 0) {
                     *p = (short) s;
                     break;
                  }
                  --r;
               }
            }
         } while (k <= j->spec_end);
      }
   }
   return 1;
}

// take a -128..127 value and stbi__clamp it and convert to 0..255
stbi_inline static stbi_uc stbi__clamp(int x)
{
   // trick to use a single test to catch both cases
   if ((unsigned int) x > 255) {
      if (x < 0) return 0;
      if (x > 255) return 255;
   }
   return (stbi_uc) x;
}

#define stbi__f2f(x)  ((int) (((x) * 4096 + 0.5)))
#define stbi__fsh(x)  ((x) * 4096)

// derived from jidctint -- DCT_ISLOW
#define STBI__IDCT_1D(s0,s1,s2,s3,s4,s5,s6,s7) \
   int t0,t1,t2,t3,p1,p2,p3,p4,p5,x0,x1,x2,x3; \
   p2 = s2;                                    \
   p3 = s6;                                    \
   p1 = (p2+p3) * stbi__f2f(0.5411961f);       \
   t2 = p1 + p3*stbi__f2f(-1.847759065f);      \
   t3 = p1 + p2*stbi__f2f( 0.765366865f);      \
   p2 = s0;                                    \
   p3 = s4;                                    \
   t0 = stbi__fsh(p2+p3);                      \
   t1 = stbi__fsh(p2-p3);                      \
   x0 = t0+t3;                                 \
   x3 = t0-t3;                                 \
   x1 = t1+t2;                                 \
   x2 = t1-t2;                                 \
   t0 = s7;                                    \
   t1 = s5;                                    \
   t2 = s3;                                    \
   t3 = s1;                                    \
   p3 = t0+t2;                                 \
   p4 = t1+t3;                                 \
   p1 = t0+t3;                                 \
   p2 = t1+t2;                                 \
   p5 = (p3+p4)*stbi__f2f( 1.175875602f);      \
   t0 = t0*stbi__f2f( 0.298631336f);           \
   t1 = t1*stbi__f2f( 2.053119869f);           \
   t2 = t2*stbi__f2f( 3.072711026f);           \
   t3 = t3*stbi__f2f( 1.501321110f);           \
   p1 = p5 + p1*stbi__f2f(-0.899976223f);      \
   p2 = p5 + p2*stbi__f2f(-2.562915447f);      \
   p3 = p3*stbi__f2f(-1.961570560f);           \
   p4 = p4*stbi__f2f(-0.390180644f);           \
   t3 += p1+p4;                                \
   t2 += p2+p3;                                \
   t1 += p2+p4;                                \
   t0 += p1+p3;

static void stbi__idct_block(stbi_uc *out, int out_stride, short data[64])
{
   int i,val[64],*v=val;
   stbi_uc *o;
   short *d = data;

   // columns
   for (i=0; i < 8; ++i,++d, ++v) {
      // if all zeroes, shortcut -- this avoids dequantizing 0s and IDCTing
      if (d[ 8]==0 && d[16]==0 && d[24]==0 && d[32]==0
           && d[40]==0 && d[48]==0 && d[56]==0) {
         //    no shortcut                 0     seconds
         //    (1|2|3|4|5|6|7)==0          0     seconds
         //    all separate               -0.047 seconds
         //    1 && 2|3 && 4|5 && 6|7:    -0.047 seconds
         int dcterm = d[0]*4;
         v[0] = v[8] = v[16] = v[24] = v[32] = v[40] = v[48] = v[56] = dcterm;
      } else {
         STBI__IDCT_1D(d[ 0],d[ 8],d[16],d[24],d[32],d[40],d[48],d[56])
         // constants scaled things up by 1<<12; let's bring them back
         // down, but keep 2 extra bits of precision
         x0 += 512; x1 += 512; x2 += 512; x3 += 512;
         v[ 0] = (x0+t3) >> 10;
         v[56] = (x0-t3) >> 10;
         v[ 8] = (x1+t2) >> 10;
         v[48] = (x1-t2) >> 10;
         v[16] = (x2+t1) >> 10;
         v[40] = (x2-t1) >> 10;
         v[24] = (x3+t0) >> 10;
         v[32] = (x3-t0) >> 10;
      }
   }

   for (i=0, v=val, o=out; i < 8; ++i,v+=8,o+=out_stride) {
      // no fast case since the first 1D IDCT spread components out
      STBI__IDCT_1D(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7])
      // constants scaled things up by 1<<12, plus we had 1<<2 from first
      // loop, plus horizontal and vertical each scale by sqrt(8) so together
      // we've got an extra 1<<3, so 1<<17 total we need to remove.
      // so we want to round that, which means adding 0.5 * 1<<17,
      // aka 65536. Also, we'll end up with -128 to 127 that we want
      // to encode as 0..255 by adding 128, so we'll add that before the shift
      x0 += 65536 + (128<<17);
      x1 += 65536 + (128<<17);
      x2 += 65536 + (128<<17);
      x3 += 65536 + (128<<17);
      // tried computing the shifts into temps, or'ing the temps to see
      // if any were out of range, but that was slower
      o[0] = stbi__clamp((x0+t3) >> 17);
      o[7] = stbi__clamp((x0-t3) >> 17);
      o[1] = stbi__clamp((x1+t2) >> 17);
      o[6] = stbi__clamp((x1-t2) >> 17);
      o[2] = stbi__clamp((x2+t1) >> 17);
      o[5] = stbi__clamp((x2-t1) >> 17);
      o[3] = stbi__clamp((x3+t0) >> 17);
      o[4] = stbi__clamp((x3-t0) >> 17);
   }
}

#ifdef STBI_SSE2
// sse2 integer IDCT. not the fastest possible implementation but it
// produces bit-identical results to the generic C version so it's
// fully "transparent".
static void stbi__idct_simd(stbi_uc *out, int out_stride, short data[64])
{
   // This is constructed to match our regular (generic) integer IDCT exactly.
   __m128i row0, row1, row2, row3, row4, row5, row6, row7;
   __m128i tmp;

   // dot product constant: even elems=x, odd elems=y
   #define dct_const(x,y)  _mm_setr_epi16((x),(y),(x),(y),(x),(y),(x),(y))

   // out(0) = c0[even]*x + c0[odd]*y   (c0, x, y 16-bit, out 32-bit)
   // out(1) = c1[even]*x + c1[odd]*y
   #define dct_rot(out0,out1, x,y,c0,c1) \
      __m128i c0##lo = _mm_unpacklo_epi16((x),(y)); \
      __m128i c0##hi = _mm_unpackhi_epi16((x),(y)); \
      __m128i out0##_l = _mm_madd_epi16(c0##lo, c0); \
      __m128i out0##_h = _mm_madd_epi16(c0##hi, c0); \
      __m128i out1##_l = _mm_madd_epi16(c0##lo, c1); \
      __m128i out1##_h = _mm_madd_epi16(c0##hi, c1)

   // out = in << 12  (in 16-bit, out 32-bit)
   #define dct_widen(out, in) \
      __m128i out##_l = _mm_srai_epi32(_mm_unpacklo_epi16(_mm_setzero_si128(), (in)), 4); \
      __m128i out##_h = _mm_srai_epi32(_mm_unpackhi_epi16(_mm_setzero_si128(), (in)), 4)

   // wide add
   #define dct_wadd(out, a, b) \
      __m128i out##_l = _mm_add_epi32(a##_l, b##_l); \
      __m128i out##_h = _mm_add_epi32(a##_h, b##_h)

   // wide sub
   #define dct_wsub(out, a, b) \
      __m128i out##_l = _mm_sub_epi32(a##_l, b##_l); \
      __m128i out##_h = _mm_sub_epi32(a##_h, b##_h)

   // butterfly a/b, add bias, then shift by "s" and pack
   #define dct_bfly32o(out0, out1, a,b,bias,s) \
      { \
         __m128i abiased_l = _mm_add_epi32(a##_l, bias); \
         __m128i abiased_h = _mm_add_epi32(a##_h, bias); \
         dct_wadd(sum, abiased, b); \
         dct_wsub(dif, abiased, b); \
         out0 = _mm_packs_epi32(_mm_srai_epi32(sum_l, s), _mm_srai_epi32(sum_h, s)); \
         out1 = _mm_packs_epi32(_mm_srai_epi32(dif_l, s), _mm_srai_epi32(dif_h, s)); \
      }

   // 8-bit interleave step (for transposes)
   #define dct_interleave8(a, b) \
      tmp = a; \
      a = _mm_unpacklo_epi8(a, b); \
      b = _mm_unpackhi_epi8(tmp, b)

   // 16-bit interleave step (for transposes)
   #define dct_interleave16(a, b) \
      tmp = a; \
      a = _mm_unpacklo_epi16(a, b); \
      b = _mm_unpackhi_epi16(tmp, b)

   #define dct_pass(bias,shift) \
      { \
         /* even part */ \
         dct_rot(t2e,t3e, row2,row6, rot0_0,rot0_1); \
         __m128i sum04 = _mm_add_epi16(row0, row4); \
         __m128i dif04 = _mm_sub_epi16(row0, row4); \
         dct_widen(t0e, sum04); \
         dct_widen(t1e, dif04); \
         dct_wadd(x0, t0e, t3e); \
         dct_wsub(x3, t0e, t3e); \
         dct_wadd(x1, t1e, t2e); \
         dct_wsub(x2, t1e, t2e); \
         /* odd part */ \
         dct_rot(y0o,y2o, row7,row3, rot2_0,rot2_1); \
         dct_rot(y1o,y3o, row5,row1, rot3_0,rot3_1); \
         __m128i sum17 = _mm_add_epi16(row1, row7); \
         __m128i sum35 = _mm_add_epi16(row3, row5); \
         dct_rot(y4o,y5o, sum17,sum35, rot1_0,rot1_1); \
         dct_wadd(x4, y0o, y4o); \
         dct_wadd(x5, y1o, y5o); \
         dct_wadd(x6, y2o, y5o); \
         dct_wadd(x7, y3o, y4o); \
         dct_bfly32o(row0,row7, x0,x7,bias,shift); \
         dct_bfly32o(row1,row6, x1,x6,bias,shift); \
         dct_bfly32o(row2,row5, x2,x5,bias,shift); \
         dct_bfly32o(row3,row4, x3,x4,bias,shift); \
      }

   __m128i rot0_0 = dct_const(stbi__f2f(0.5411961f), stbi__f2f(0.5411961f) + stbi__f2f(-1.847759065f));
   __m128i rot0_1 = dct_const(stbi__f2f(0.5411961f) + stbi__f2f( 0.765366865f), stbi__f2f(0.5411961f));
   __m128i rot1_0 = dct_const(stbi__f2f(1.175875602f) + stbi__f2f(-0.899976223f), stbi__f2f(1.175875602f));
   __m128i rot1_1 = dct_const(stbi__f2f(1.175875602f), stbi__f2f(1.175875602f) + stbi__f2f(-2.562915447f));
   __m128i rot2_0 = dct_const(stbi__f2f(-1.961570560f) + stbi__f2f( 0.298631336f), stbi__f2f(-1.961570560f));
   __m128i rot2_1 = dct_const(stbi__f2f(-1.961570560f), stbi__f2f(-1.961570560f) + stbi__f2f( 3.072711026f));
   __m128i rot3_0 = dct_const(stbi__f2f(-0.390180644f) + stbi__f2f( 2.053119869f), stbi__f2f(-0.390180644f));
   __m128i rot3_1 = dct_const(stbi__f2f(-0.390180644f), stbi__f2f(-0.390180644f) + stbi__f2f( 1.501321110f));

   // rounding biases in column/row passes, see stbi__idct_block for explanation.
   __m128i bias_0 = _mm_set1_epi32(512);
   __m128i bias_1 = _mm_set1_epi32(65536 + (128<<17));

   // load
   row0 = _mm_load_si128((const __m128i *) (data + 0*8));
   row1 = _mm_load_si128((const __m128i *) (data + 1*8));
   row2 = _mm_load_si128((const __m128i *) (data + 2*8));
   row3 = _mm_load_si128((const __m128i *) (data + 3*8));
   row4 = _mm_load_si128((const __m128i *) (data + 4*8));
   row5 = _mm_load_si128((const __m128i *) (data + 5*8));
   row6 = _mm_load_si128((const __m128i *) (data + 6*8));
   row7 = _mm_load_si128((const __m128i *) (data + 7*8));

   // column pass
   dct_pass(bias_0, 10);

   {
      // 16bit 8x8 transpose pass 1
      dct_interleave16(row0, row4);
      dct_interleave16(row1, row5);
      dct_interleave16(row2, row6);
      dct_interleave16(row3, row7);

      // transpose pass 2
      dct_interleave16(row0, row2);
      dct_interleave16(row1, row3);
      dct_interleave16(row4, row6);
      dct_interleave16(row5, row7);

      // transpose pass 3
      dct_interleave16(row0, row1);
      dct_interleave16(row2, row3);
      dct_interleave16(row4, row5);
      dct_interleave16(row6, row7);
   }

   // row pass
   dct_pass(bias_1, 17);

   {
      // pack
      __m128i p0 = _mm_packus_epi16(row0, row1); // a0a1a2a3...a7b0b1b2b3...b7
      __m128i p1 = _mm_packus_epi16(row2, row3);
      __m128i p2 = _mm_packus_epi16(row4, row5);
      __m128i p3 = _mm_packus_epi16(row6, row7);

      // 8bit 8x8 transpose pass 1
      dct_interleave8(p0, p2); // a0e0a1e1...
      dct_interleave8(p1, p3); // c0g0c1g1...

      // transpose pass 2
      dct_interleave8(p0, p1); // a0c0e0g0...
      dct_interleave8(p2, p3); // b0d0f0h0...

      // transpose pass 3
      dct_interleave8(p0, p2); // a0b0c0d0...
      dct_interleave8(p1, p3); // a4b4c4d4...

      // store
      _mm_storel_epi64((__m128i *) out, p0); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p0, 0x4e)); out += out_stride;
      _mm_storel_epi64((__m128i *) out, p2); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p2, 0x4e)); out += out_stride;
      _mm_storel_epi64((__m128i *) out, p1); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p1, 0x4e)); out += out_stride;
      _mm_storel_epi64((__m128i *) out, p3); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p3, 0x4e));
   }

#undef dct_const
#undef dct_rot
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_interleave8
#undef dct_interleave16
#undef dct_pass
}

#endif // STBI_SSE2

#ifdef STBI_NEON

// NEON integer IDCT. should produce bit-identical
// results to the generic C version.
static void stbi__idct_simd(stbi_uc *out, int out_stride, short data[64])
{
   int16x8_t row0, row1, row2, row3, row4, row5, row6, row7;

   int16x4_t rot0_0 = vdup_n_s16(stbi__f2f(0.5411961f));
   int16x4_t rot0_1 = vdup_n_s16(stbi__f2f(-1.847759065f));
   int16x4_t rot0_2 = vdup_n_s16(stbi__f2f( 0.765366865f));
   int16x4_t rot1_0 = vdup_n_s16(stbi__f2f( 1.175875602f));
   int16x4_t rot1_1 = vdup_n_s16(stbi__f2f(-0.899976223f));
   int16x4_t rot1_2 = vdup_n_s16(stbi__f2f(-2.562915447f));
   int16x4_t rot2_0 = vdup_n_s16(stbi__f2f(-1.961570560f));
   int16x4_t rot2_1 = vdup_n_s16(stbi__f2f(-0.390180644f));
   int16x4_t rot3_0 = vdup_n_s16(stbi__f2f( 0.298631336f));
   int16x4_t rot3_1 = vdup_n_s16(stbi__f2f( 2.053119869f));
   int16x4_t rot3_2 = vdup_n_s16(stbi__f2f( 3.072711026f));
   int16x4_t rot3_3 = vdup_n_s16(stbi__f2f( 1.501321110f));

#define dct_long_mul(out, inq, coeff) \
   int32x4_t out##_l = vmull_s16(vget_low_s16(inq), coeff); \
   int32x4_t out##_h = vmull_s16(vget_high_s16(inq), coeff)

#define dct_long_mac(out, acc, inq, coeff) \
   int32x4_t out##_l = vmlal_s16(acc##_l, vget_low_s16(inq), coeff); \
   int32x4_t out##_h = vmlal_s16(acc##_h, vget_high_s16(inq), coeff)

#define dct_widen(out, inq) \
   int32x4_t out##_l = vshll_n_s16(vget_low_s16(inq), 12); \
   int32x4_t out##_h = vshll_n_s16(vget_high_s16(inq), 12)

// wide add
#define dct_wadd(out, a, b) \
   int32x4_t out##_l = vaddq_s32(a##_l, b##_l); \
   int32x4_t out##_h = vaddq_s32(a##_h, b##_h)

// wide sub
#define dct_wsub(out, a, b) \
   int32x4_t out##_l = vsubq_s32(a##_l, b##_l); \
   int32x4_t out##_h = vsubq_s32(a##_h, b##_h)

// butterfly a/b, then shift using "shiftop" by "s" and pack
#define dct_bfly32o(out0,out1, a,b,shiftop,s) \
   { \
      dct_wadd(sum, a, b); \
      dct_wsub(dif, a, b); \
      out0 = vcombine_s16(shiftop(sum_l, s), shiftop(sum_h, s)); \
      out1 = vcombine_s16(shiftop(dif_l, s), shiftop(dif_h, s)); \
   }

#define dct_pass(shiftop, shift) \
   { \
      /* even part */ \
      int16x8_t sum26 = vaddq_s16(row2, row6); \
      dct_long_mul(p1e, sum26, rot0_0); \
      dct_long_mac(t2e, p1e, row6, rot0_1); \
      dct_long_mac(t3e, p1e, row2, rot0_2); \
      int16x8_t sum04 = vaddq_s16(row0, row4); \
      int16x8_t dif04 = vsubq_s16(row0, row4); \
      dct_widen(t0e, sum04); \
      dct_widen(t1e, dif04); \
      dct_wadd(x0, t0e, t3e); \
      dct_wsub(x3, t0e, t3e); \
      dct_wadd(x1, t1e, t2e); \
      dct_wsub(x2, t1e, t2e); \
      /* odd part */ \
      int16x8_t sum15 = vaddq_s16(row1, row5); \
      int16x8_t sum17 = vaddq_s16(row1, row7); \
      int16x8_t sum35 = vaddq_s16(row3, row5); \
      int16x8_t sum37 = vaddq_s16(row3, row7); \
      int16x8_t sumodd = vaddq_s16(sum17, sum35); \
      dct_long_mul(p5o, sumodd, rot1_0); \
      dct_long_mac(p1o, p5o, sum17, rot1_1); \
      dct_long_mac(p2o, p5o, sum35, rot1_2); \
      dct_long_mul(p3o, sum37, rot2_0); \
      dct_long_mul(p4o, sum15, rot2_1); \
      dct_wadd(sump13o, p1o, p3o); \
      dct_wadd(sump24o, p2o, p4o); \
      dct_wadd(sump23o, p2o, p3o); \
      dct_wadd(sump14o, p1o, p4o); \
      dct_long_mac(x4, sump13o, row7, rot3_0); \
      dct_long_mac(x5, sump24o, row5, rot3_1); \
      dct_long_mac(x6, sump23o, row3, rot3_2); \
      dct_long_mac(x7, sump14o, row1, rot3_3); \
      dct_bfly32o(row0,row7, x0,x7,shiftop,shift); \
      dct_bfly32o(row1,row6, x1,x6,shiftop,shift); \
      dct_bfly32o(row2,row5, x2,x5,shiftop,shift); \
      dct_bfly32o(row3,row4, x3,x4,shiftop,shift); \
   }

   // load
   row0 = vld1q_s16(data + 0*8);
   row1 = vld1q_s16(data + 1*8);
   row2 = vld1q_s16(data + 2*8);
   row3 = vld1q_s16(data + 3*8);
   row4 = vld1q_s16(data + 4*8);
   row5 = vld1q_s16(data + 5*8);
   row6 = vld1q_s16(data + 6*8);
   row7 = vld1q_s16(data + 7*8);

   // add DC bias
   row0 = vaddq_s16(row0, vsetq_lane_s16(1024, vdupq_n_s16(0), 0));

   // column pass
   dct_pass(vrshrn_n_s32, 10);

   // 16bit 8x8 transpose
   {
// these three map to a single VTRN.16, VTRN.32, and VSWP, respectively.
// whether compilers actually get this is another story, sadly.
#define dct_trn16(x, y) { int16x8x2_t t = vtrnq_s16(x, y); x = t.val[0]; y = t.val[1]; }
#define dct_trn32(x, y) { int32x4x2_t t = vtrnq_s32(vreinterpretq_s32_s16(x), vreinterpretq_s32_s16(y)); x = vreinterpretq_s16_s32(t.val[0]); y = vreinterpretq_s16_s32(t.val[1]); }
#define dct_trn64(x, y) { int16x8_t x0 = x; int16x8_t y0 = y; x = vcombine_s16(vget_low_s16(x0), vget_low_s16(y0)); y = vcombine_s16(vget_high_s16(x0), vget_high_s16(y0)); }

      // pass 1
      dct_trn16(row0, row1); // a0b0a2b2a4b4a6b6
      dct_trn16(row2, row3);
      dct_trn16(row4, row5);
      dct_trn16(row6, row7);

      // pass 2
      dct_trn32(row0, row2); // a0b0c0d0a4b4c4d4
      dct_trn32(row1, row3);
      dct_trn32(row4, row6);
      dct_trn32(row5, row7);

      // pass 3
      dct_trn64(row0, row4); // a0b0c0d0e0f0g0h0
      dct_trn64(row1, row5);
      dct_trn64(row2, row6);
      dct_trn64(row3, row7);

#undef dct_trn16
#undef dct_trn32
#undef dct_trn64
   }

   // row pass
   // vrshrn_n_s32 only supports shifts up to 16, we need
   // 17. so do a non-rounding shift of 16 first then follow
   // up with a rounding shift by 1.
   dct_pass(vshrn_n_s32, 16);

   {
      // pack and round
      uint8x8_t p0 = vqrshrun_n_s16(row0, 1);
      uint8x8_t p1 = vqrshrun_n_s16(row1, 1);
      uint8x8_t p2 = vqrshrun_n_s16(row2, 1);
      uint8x8_t p3 = vqrshrun_n_s16(row3, 1);
      uint8x8_t p4 = vqrshrun_n_s16(row4, 1);
      uint8x8_t p5 = vqrshrun_n_s16(row5, 1);
      uint8x8_t p6 = vqrshrun_n_s16(row6, 1);
      uint8x8_t p7 = vqrshrun_n_s16(row7, 1);

      // again, these can translate into one instruction, but often don't.
#define dct_trn8_8(x, y) { uint8x8x2_t t = vtrn_u8(x, y); x = t.val[0]; y = t.val[1]; }
#define dct_trn8_16(x, y) { uint16x4x2_t t = vtrn_u16(vreinterpret_u16_u8(x), vreinterpret_u16_u8(y)); x = vreinterpret_u8_u16(t.val[0]); y = vreinterpret_u8_u16(t.val[1]); }
#define dct_trn8_32(x, y) { uint32x2x2_t t = vtrn_u32(vreinterpret_u32_u8(x), vreinterpret_u32_u8(y)); x = vreinterpret_u8_u32(t.val[0]); y = vreinterpret_u8_u32(t.val[1]); }

      // sadly can't use interleaved stores here since we only write
      // 8 bytes to each scan line!

      // 8x8 8-bit transpose pass 1
      dct_trn8_8(p0, p1);
      dct_trn8_8(p2, p3);
      dct_trn8_8(p4, p5);
      dct_trn8_8(p6, p7);

      // pass 2
      dct_trn8_16(p0, p2);
      dct_trn8_16(p1, p3);
      dct_trn8_16(p4, p6);
      dct_trn8_16(p5, p7);

      // pass 3
      dct_trn8_32(p0, p4);
      dct_trn8_32(p1, p5);
      dct_trn8_32(p2, p6);
      dct_trn8_32(p3, p7);

      // store
      vst1_u8(out, p0); out += out_stride;
      vst1_u8(out, p1); out += out_stride;
      vst1_u8(out, p2); out += out_stride;
      vst1_u8(out, p3); out += out_stride;
      vst1_u8(out, p4); out += out_stride;
      vst1_u8(out, p5); out += out_stride;
      vst1_u8(out, p6); out += out_stride;
      vst1_u8(out, p7);

#undef dct_trn8_8
#undef dct_trn8_16
#undef dct_trn8_32
   }

#undef dct_long_mul
#undef dct_long_mac
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_pass
}

#endif // STBI_NEON

#define STBI__MARKER_none  0xff
// if there's a pending marker from the entropy stream, return that
// otherwise, fetch from the stream and get a marker. if there's no
// marker, return 0xff, which is never a valid marker value
static stbi_uc stbi__get_marker(stbi__jpeg *j)
{
   stbi_uc x;
   if (j->marker != STBI__MARKER_none) { x = j->marker; j->marker = STBI__MARKER_none; return x; }
   x = stbi__get8(j->s);
   if (x != 0xff) return STBI__MARKER_none;
   while (x == 0xff)
      x = stbi__get8(j->s); // consume repeated 0xff fill bytes
   return x;
}

// in each scan, we'll have scan_n components, and the order
// of the components is specified by order[]
#define STBI__RESTART(x)     ((x) >= 0xd0 && (x) <= 0xd7)

// after a restart interval, stbi__jpeg_reset the entropy decoder and
// the dc prediction
static void stbi__jpeg_reset(stbi__jpeg *j)
{
   j->code_bits = 0;
   j->code_buffer = 0;
   j->nomore = 0;
   j->img_comp[0].dc_pred = j->img_comp[1].dc_pred = j->img_comp[2].dc_pred = j->img_comp[3].dc_pred = 0;
   j->marker = STBI__MARKER_none;
   j->todo = j->restart_interval ? j->restart_interval : 0x7fffffff;
   j->eob_run = 0;
   // no more than 1<<31 MCUs if no restart_interal? that's plenty safe,
   // since we don't even allow 1<<30 pixels
}

static int stbi__parse_entropy_coded_data(stbi__jpeg *z)
{
   stbi__jpeg_reset(z);
   if (!z->progressive) {
      if (z->scan_n == 1) {
         int i,j;
         STBI_SIMD_ALIGN(short, data[64]);
         int n = z->order[0];
         // non-interleaved data, we just need to process one block at a time,
         // in trivial scanline order
         // number of blocks to do just depends on how many actual "pixels" this
         // component has, independent of interleaved MCU blocking and such
         int w = (z->img_comp[n].x+7) >> 3;
         int h = (z->img_comp[n].y+7) >> 3;
         for (j=0; j < h; ++j) {
            for (i=0; i < w; ++i) {
               int ha = z->img_comp[n].ha;
               if (!stbi__jpeg_decode_block(z, data, z->huff_dc+z->img_comp[n].hd, z->huff_ac+ha, z->fast_ac[ha], n, z->dequant[z->img_comp[n].tq])) return 0;
               z->idct_block_kernel(z->img_comp[n].data+z->img_comp[n].w2*j*8+i*8, z->img_comp[n].w2, data);
               // every data block is an MCU, so countdown the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  // if it's NOT a restart, then just bail, so we get corrupt data
                  // rather than no data
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      } else { // interleaved
         int i,j,k,x,y;
         STBI_SIMD_ALIGN(short, data[64]);
         for (j=0; j < z->img_mcu_y; ++j) {
            for (i=0; i < z->img_mcu_x; ++i) {
               // scan an interleaved mcu... process scan_n components in order
               for (k=0; k < z->scan_n; ++k) {
                  int n = z->order[k];
                  // scan out an mcu's worth of this component; that's just determined
                  // by the basic H and V specified for the component
                  for (y=0; y < z->img_comp[n].v; ++y) {
                     for (x=0; x < z->img_comp[n].h; ++x) {
                        int x2 = (i*z->img_comp[n].h + x)*8;
                        int y2 = (j*z->img_comp[n].v + y)*8;
                        int ha = z->img_comp[n].ha;
                        if (!stbi__jpeg_decode_block(z, data, z->huff_dc+z->img_comp[n].hd, z->huff_ac+ha, z->fast_ac[ha], n, z->dequant[z->img_comp[n].tq])) return 0;
                        z->idct_block_kernel(z->img_comp[n].data+z->img_comp[n].w2*y2+x2, z->img_comp[n].w2, data);
                     }
                  }
               }
               // after all interleaved components, that's an interleaved MCU,
               // so now count down the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      }
   } else {
      if (z->scan_n == 1) {
         int i,j;
         int n = z->order[0];
         // non-interleaved data, we just need to process one block at a time,
         // in trivial scanline order
         // number of blocks to do just depends on how many actual "pixels" this
         // component has, independent of interleaved MCU blocking and such
         int w = (z->img_comp[n].x+7) >> 3;
         int h = (z->img_comp[n].y+7) >> 3;
         for (j=0; j < h; ++j) {
            for (i=0; i < w; ++i) {
               short *data = z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
               if (z->spec_start == 0) {
                  if (!stbi__jpeg_decode_block_prog_dc(z, data, &z->huff_dc[z->img_comp[n].hd], n))
                     return 0;
               } else {
                  int ha = z->img_comp[n].ha;
                  if (!stbi__jpeg_decode_block_prog_ac(z, data, &z->huff_ac[ha], z->fast_ac[ha]))
                     return 0;
               }
               // every data block is an MCU, so countdown the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      } else { // interleaved
         int i,j,k,x,y;
         for (j=0; j < z->img_mcu_y; ++j) {
            for (i=0; i < z->img_mcu_x; ++i) {
               // scan an interleaved mcu... process scan_n components in order
               for (k=0; k < z->scan_n; ++k) {
                  int n = z->order[k];
                  // scan out an mcu's worth of this component; that's just determined
                  // by the basic H and V specified for the component
                  for (y=0; y < z->img_comp[n].v; ++y) {
                     for (x=0; x < z->img_comp[n].h; ++x) {
                        int x2 = (i*z->img_comp[n].h + x);
                        int y2 = (j*z->img_comp[n].v + y);
                        short *data = z->img_comp[n].coeff + 64 * (x2 + y2 * z->img_comp[n].coeff_w);
                        if (!stbi__jpeg_decode_block_prog_dc(z, data, &z->huff_dc[z->img_comp[n].hd], n))
                           return 0;
                     }
                  }
               }
               // after all interleaved components, that's an interleaved MCU,
               // so now count down the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      }
   }
}

static void stbi__jpeg_dequantize(short *data, stbi__uint16 *dequant)
{
   int i;
   for (i=0; i < 64; ++i)
      data[i] *= dequant[i];
}

static void stbi__jpeg_finish(stbi__jpeg *z)
{
   if (z->progressive) {
      // dequantize and idct the data
      int i,j,n;
      for (n=0; n < z->s->img_n; ++n) {
         int w = (z->img_comp[n].x+7) >> 3;
         int h = (z->img_comp[n].y+7) >> 3;
         for (j=0; j < h; ++j) {
            for (i=0; i < w; ++i) {
               short *data = z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
               stbi__jpeg_dequantize(data, z->dequant[z->img_comp[n].tq]);
               z->idct_block_kernel(z->img_comp[n].data+z->img_comp[n].w2*j*8+i*8, z->img_comp[n].w2, data);
            }
         }
      }
   }
}

static int stbi__process_marker(stbi__jpeg *z, int m)
{
   int L;
   switch (m) {
      case STBI__MARKER_none: // no marker found
         return stbi__err("expected marker","Corrupt JPEG");

      case 0xDD: // DRI - specify restart interval
         if (stbi__get16be(z->s) != 4) return stbi__err("bad DRI len","Corrupt JPEG");
         z->restart_interval = stbi__get16be(z->s);
         return 1;

      case 0xDB: // DQT - define quantization table
         L = stbi__get16be(z->s)-2;
         while (L > 0) {
            int q = stbi__get8(z->s);
            int p = q >> 4, sixteen = (p != 0);
            int t = q & 15,i;
            if (p != 0 && p != 1) return stbi__err("bad DQT type","Corrupt JPEG");
            if (t > 3) return stbi__err("bad DQT table","Corrupt JPEG");

            for (i=0; i < 64; ++i)
               z->dequant[t][stbi__jpeg_dezigzag[i]] = (stbi__uint16)(sixteen ? stbi__get16be(z->s) : stbi__get8(z->s));
            L -= (sixteen ? 129 : 65);
         }
         return L==0;

      case 0xC4: // DHT - define huffman table
         L = stbi__get16be(z->s)-2;
         while (L > 0) {
            stbi_uc *v;
            int sizes[16],i,n=0;
            int q = stbi__get8(z->s);
            int tc = q >> 4;
            int th = q & 15;
            if (tc > 1 || th > 3) return stbi__err("bad DHT header","Corrupt JPEG");
            for (i=0; i < 16; ++i) {
               sizes[i] = stbi__get8(z->s);
               n += sizes[i];
            }
            if(n > 256) return stbi__err("bad DHT header","Corrupt JPEG"); // Loop over i < n would write past end of values!
            L -= 17;
            if (tc == 0) {
               if (!stbi__build_huffman(z->huff_dc+th, sizes)) return 0;
               v = z->huff_dc[th].values;
            } else {
               if (!stbi__build_huffman(z->huff_ac+th, sizes)) return 0;
               v = z->huff_ac[th].values;
            }
            for (i=0; i < n; ++i)
               v[i] = stbi__get8(z->s);
            if (tc != 0)
               stbi__build_fast_ac(z->fast_ac[th], z->huff_ac + th);
            L -= n;
         }
         return L==0;
   }

   // check for comment block or APP blocks
   if ((m >= 0xE0 && m <= 0xEF) || m == 0xFE) {
      L = stbi__get16be(z->s);
      if (L < 2) {
         if (m == 0xFE)
            return stbi__err("bad COM len","Corrupt JPEG");
         else
            return stbi__err("bad APP len","Corrupt JPEG");
      }
      L -= 2;

      if (m == 0xE0 && L >= 5) { // JFIF APP0 segment
         static const unsigned char tag[5] = {'J','F','I','F','\0'};
         int ok = 1;
         int i;
         for (i=0; i < 5; ++i)
            if (stbi__get8(z->s) != tag[i])
               ok = 0;
         L -= 5;
         if (ok)
            z->jfif = 1;
      } else if (m == 0xEE && L >= 12) { // Adobe APP14 segment
         static const unsigned char tag[6] = {'A','d','o','b','e','\0'};
         int ok = 1;
         int i;
         for (i=0; i < 6; ++i)
            if (stbi__get8(z->s) != tag[i])
               ok = 0;
         L -= 6;
         if (ok) {
            stbi__get8(z->s); // version
            stbi__get16be(z->s); // flags0
            stbi__get16be(z->s); // flags1
            z->app14_color_transform = stbi__get8(z->s); // color transform
            L -= 6;
         }
      }

      stbi__skip(z->s, L);
      return 1;
   }

   return stbi__err("unknown marker","Corrupt JPEG");
}

// after we see SOS
static int stbi__process_scan_header(stbi__jpeg *z)
{
   int i;
   int Ls = stbi__get16be(z->s);
   z->scan_n = stbi__get8(z->s);
   if (z->scan_n < 1 || z->scan_n > 4 || z->scan_n > (int) z->s->img_n) return stbi__err("bad SOS component count","Corrupt JPEG");
   if (Ls != 6+2*z->scan_n) return stbi__err("bad SOS len","Corrupt JPEG");
   for (i=0; i < z->scan_n; ++i) {
      int id = stbi__get8(z->s), which;
      int q = stbi__get8(z->s);
      for (which = 0; which < z->s->img_n; ++which)
         if (z->img_comp[which].id == id)
            break;
      if (which == z->s->img_n) return 0; // no match
      z->img_comp[which].hd = q >> 4;   if (z->img_comp[which].hd > 3) return stbi__err("bad DC huff","Corrupt JPEG");
      z->img_comp[which].ha = q & 15;   if (z->img_comp[which].ha > 3) return stbi__err("bad AC huff","Corrupt JPEG");
      z->order[i] = which;
   }

   {
      int aa;
      z->spec_start = stbi__get8(z->s);
      z->spec_end   = stbi__get8(z->s); // should be 63, but might be 0
      aa = stbi__get8(z->s);
      z->succ_high = (aa >> 4);
      z->succ_low  = (aa & 15);
      if (z->progressive) {
         if (z->spec_start > 63 || z->spec_end > 63  || z->spec_start > z->spec_end || z->succ_high > 13 || z->succ_low > 13)
            return stbi__err("bad SOS", "Corrupt JPEG");
      } else {
         if (z->spec_start != 0) return stbi__err("bad SOS","Corrupt JPEG");
         if (z->succ_high != 0 || z->succ_low != 0) return stbi__err("bad SOS","Corrupt JPEG");
         z->spec_end = 63;
      }
   }

   return 1;
}

static int stbi__free_jpeg_components(stbi__jpeg *z, int ncomp, int why)
{
   int i;
   for (i=0; i < ncomp; ++i) {
      if (z->img_comp[i].raw_data) {
         STBI_FREE(z->img_comp[i].raw_data);
         z->img_comp[i].raw_data = NULL;
         z->img_comp[i].data = NULL;
      }
      if (z->img_comp[i].raw_coeff) {
         STBI_FREE(z->img_comp[i].raw_coeff);
         z->img_comp[i].raw_coeff = 0;
         z->img_comp[i].coeff = 0;
      }
      if (z->img_comp[i].linebuf) {
         STBI_FREE(z->img_comp[i].linebuf);
         z->img_comp[i].linebuf = NULL;
      }
   }
   return why;
}

static int stbi__process_frame_header(stbi__jpeg *z, int scan)
{
   stbi__context *s = z->s;
   int Lf,p,i,q, h_max=1,v_max=1,c;
   Lf = stbi__get16be(s);         if (Lf < 11) return stbi__err("bad SOF len","Corrupt JPEG"); // JPEG
   p  = stbi__get8(s);            if (p != 8) return stbi__err("only 8-bit","JPEG format not supported: 8-bit only"); // JPEG baseline
   s->img_y = stbi__get16be(s);   if (s->img_y == 0) return stbi__err("no header height", "JPEG format not supported: delayed height"); // Legal, but we don't handle it--but neither does IJG
   s->img_x = stbi__get16be(s);   if (s->img_x == 0) return stbi__err("0 width","Corrupt JPEG"); // JPEG requires
   if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
   if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
   c = stbi__get8(s);
   if (c != 3 && c != 1 && c != 4) return stbi__err("bad component count","Corrupt JPEG");
   s->img_n = c;
   for (i=0; i < c; ++i) {
      z->img_comp[i].data = NULL;
      z->img_comp[i].linebuf = NULL;
   }

   if (Lf != 8+3*s->img_n) return stbi__err("bad SOF len","Corrupt JPEG");

   z->rgb = 0;
   for (i=0; i < s->img_n; ++i) {
      static const unsigned char rgb[3] = { 'R', 'G', 'B' };
      z->img_comp[i].id = stbi__get8(s);
      if (s->img_n == 3 && z->img_comp[i].id == rgb[i])
         ++z->rgb;
      q = stbi__get8(s);
      z->img_comp[i].h = (q >> 4);  if (!z->img_comp[i].h || z->img_comp[i].h > 4) return stbi__err("bad H","Corrupt JPEG");
      z->img_comp[i].v = q & 15;    if (!z->img_comp[i].v || z->img_comp[i].v > 4) return stbi__err("bad V","Corrupt JPEG");
      z->img_comp[i].tq = stbi__get8(s);  if (z->img_comp[i].tq > 3) return stbi__err("bad TQ","Corrupt JPEG");
   }

   if (scan != STBI__SCAN_load) return 1;

   if (!stbi__mad3sizes_valid(s->img_x, s->img_y, s->img_n, 0)) return stbi__err("too large", "Image too large to decode");

   for (i=0; i < s->img_n; ++i) {
      if (z->img_comp[i].h > h_max) h_max = z->img_comp[i].h;
      if (z->img_comp[i].v > v_max) v_max = z->img_comp[i].v;
   }

   // check that plane subsampling factors are integer ratios; our resamplers can't deal with fractional ratios
   // and I've never seen a non-corrupted JPEG file actually use them
   for (i=0; i < s->img_n; ++i) {
      if (h_max % z->img_comp[i].h != 0) return stbi__err("bad H","Corrupt JPEG");
      if (v_max % z->img_comp[i].v != 0) return stbi__err("bad V","Corrupt JPEG");
   }

   // compute interleaved mcu info
   z->img_h_max = h_max;
   z->img_v_max = v_max;
   z->img_mcu_w = h_max * 8;
   z->img_mcu_h = v_max * 8;
   // these sizes can't be more than 17 bits
   z->img_mcu_x = (s->img_x + z->img_mcu_w-1) / z->img_mcu_w;
   z->img_mcu_y = (s->img_y + z->img_mcu_h-1) / z->img_mcu_h;

   for (i=0; i < s->img_n; ++i) {
      // number of effective pixels (e.g. for non-interleaved MCU)
      z->img_comp[i].x = (s->img_x * z->img_comp[i].h + h_max-1) / h_max;
      z->img_comp[i].y = (s->img_y * z->img_comp[i].v + v_max-1) / v_max;
      // to simplify generation, we'll allocate enough memory to decode
      // the bogus oversized data from using interleaved MCUs and their
      // big blocks (e.g. a 16x16 iMCU on an image of width 33); we won't
      // discard the extra data until colorspace conversion
      //
      // img_mcu_x, img_mcu_y: <=17 bits; comp[i].h and .v are <=4 (checked earlier)
      // so these muls can't overflow with 32-bit ints (which we require)
      z->img_comp[i].w2 = z->img_mcu_x * z->img_comp[i].h * 8;
      z->img_comp[i].h2 = z->img_mcu_y * z->img_comp[i].v * 8;
      z->img_comp[i].coeff = 0;
      z->img_comp[i].raw_coeff = 0;
      z->img_comp[i].linebuf = NULL;
      z->img_comp[i].raw_data = stbi__malloc_mad2(z->img_comp[i].w2, z->img_comp[i].h2, 15);
      if (z->img_comp[i].raw_data == NULL)
         return stbi__free_jpeg_components(z, i+1, stbi__err("outofmem", "Out of memory"));
      // align blocks for idct using mmx/sse
      z->img_comp[i].data = (stbi_uc*) (((size_t) z->img_comp[i].raw_data + 15) & ~15);
      if (z->progressive) {
         // w2, h2 are multiples of 8 (see above)
         z->img_comp[i].coeff_w = z->img_comp[i].w2 / 8;
         z->img_comp[i].coeff_h = z->img_comp[i].h2 / 8;
         z->img_comp[i].raw_coeff = stbi__malloc_mad3(z->img_comp[i].w2, z->img_comp[i].h2, sizeof(short), 15);
         if (z->img_comp[i].raw_coeff == NULL)
            return stbi__free_jpeg_components(z, i+1, stbi__err("outofmem", "Out of memory"));
         z->img_comp[i].coeff = (short*) (((size_t) z->img_comp[i].raw_coeff + 15) & ~15);
      }
   }

   return 1;
}

// use comparisons since in some cases we handle more than one case (e.g. SOF)
#define stbi__DNL(x)         ((x) == 0xdc)
#define stbi__SOI(x)         ((x) == 0xd8)
#define stbi__EOI(x)         ((x) == 0xd9)
#define stbi__SOF(x)         ((x) == 0xc0 || (x) == 0xc1 || (x) == 0xc2)
#define stbi__SOS(x)         ((x) == 0xda)

#define stbi__SOF_progressive(x)   ((x) == 0xc2)

static int stbi__decode_jpeg_header(stbi__jpeg *z, int scan)
{
   int m;
   z->jfif = 0;
   z->app14_color_transform = -1; // valid values are 0,1,2
   z->marker = STBI__MARKER_none; // initialize cached marker to empty
   m = stbi__get_marker(z);
   if (!stbi__SOI(m)) return stbi__err("no SOI","Corrupt JPEG");
   if (scan == STBI__SCAN_type) return 1;
   m = stbi__get_marker(z);
   while (!stbi__SOF(m)) {
      if (!stbi__process_marker(z,m)) return 0;
      m = stbi__get_marker(z);
      while (m == STBI__MARKER_none) {
         // some files have extra padding after their blocks, so ok, we'll scan
         if (stbi__at_eof(z->s)) return stbi__err("no SOF", "Corrupt JPEG");
         m = stbi__get_marker(z);
      }
   }
   z->progressive = stbi__SOF_progressive(m);
   if (!stbi__process_frame_header(z, scan)) return 0;
   return 1;
}

static stbi_uc stbi__skip_jpeg_junk_at_end(stbi__jpeg *j)
{
   // some JPEGs have junk at end, skip over it but if we find what looks
   // like a valid marker, resume there
   while (!stbi__at_eof(j->s)) {
      stbi_uc x = stbi__get8(j->s);
      while (x == 0xff) { // might be a marker
         if (stbi__at_eof(j->s)) return STBI__MARKER_none;
         x = stbi__get8(j->s);
         if (x != 0x00 && x != 0xff) {
            // not a stuffed zero or lead-in to another marker, looks
            // like an actual marker, return it
            return x;
         }
         // stuffed zero has x=0 now which ends the loop, meaning we go
         // back to regular scan loop.
         // repeated 0xff keeps trying to read the next byte of the marker.
      }
   }
   return STBI__MARKER_none;
}

// decode image to YCbCr format
static int stbi__decode_jpeg_image(stbi__jpeg *j)
{
   int m;
   for (m = 0; m < 4; m++) {
      j->img_comp[m].raw_data = NULL;
      j->img_comp[m].raw_coeff = NULL;
   }
   j->restart_interval = 0;
   if (!stbi__decode_jpeg_header(j, STBI__SCAN_load)) return 0;
   m = stbi__get_marker(j);
   while (!stbi__EOI(m)) {
      if (stbi__SOS(m)) {
         if (!stbi__process_scan_header(j)) return 0;
         if (!stbi__parse_entropy_coded_data(j)) return 0;
         if (j->marker == STBI__MARKER_none ) {
         j->marker = stbi__skip_jpeg_junk_at_end(j);
            // if we reach eof without hitting a marker, stbi__get_marker() below will fail and we'll eventually return 0
         }
         m = stbi__get_marker(j);
         if (STBI__RESTART(m))
            m = stbi__get_marker(j);
      } else if (stbi__DNL(m)) {
         int Ld = stbi__get16be(j->s);
         stbi__uint32 NL = stbi__get16be(j->s);
         if (Ld != 4) return stbi__err("bad DNL len", "Corrupt JPEG");
         if (NL != j->s->img_y) return stbi__err("bad DNL height", "Corrupt JPEG");
         m = stbi__get_marker(j);
      } else {
         if (!stbi__process_marker(j, m)) return 1;
         m = stbi__get_marker(j);
      }
   }
   if (j->progressive)
      stbi__jpeg_finish(j);
   return 1;
}

// static jfif-centered resampling (across block boundaries)

typedef stbi_uc *(*resample_row_func)(stbi_uc *out, stbi_uc *in0, stbi_uc *in1,
                                    int w, int hs);

#define stbi__div4(x) ((stbi_uc) ((x) >> 2))

static stbi_uc *resample_row_1(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   STBI_NOTUSED(out);
   STBI_NOTUSED(in_far);
   STBI_NOTUSED(w);
   STBI_NOTUSED(hs);
   return in_near;
}

static stbi_uc* stbi__resample_row_v_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate two samples vertically for every one in input
   int i;
   STBI_NOTUSED(hs);
   for (i=0; i < w; ++i)
      out[i] = stbi__div4(3*in_near[i] + in_far[i] + 2);
   return out;
}

static stbi_uc*  stbi__resample_row_h_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate two samples horizontally for every one in input
   int i;
   stbi_uc *input = in_near;

   if (w == 1) {
      // if only one sample, can't do any interpolation
      out[0] = out[1] = input[0];
      return out;
   }

   out[0] = input[0];
   out[1] = stbi__div4(input[0]*3 + input[1] + 2);
   for (i=1; i < w-1; ++i) {
      int n = 3*input[i]+2;
      out[i*2+0] = stbi__div4(n+input[i-1]);
      out[i*2+1] = stbi__div4(n+input[i+1]);
   }
   out[i*2+0] = stbi__div4(input[w-2]*3 + input[w-1] + 2);
   out[i*2+1] = input[w-1];

   STBI_NOTUSED(in_far);
   STBI_NOTUSED(hs);

   return out;
}

#define stbi__div16(x) ((stbi_uc) ((x) >> 4))

static stbi_uc *stbi__resample_row_hv_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate 2x2 samples for every one in input
   int i,t0,t1;
   if (w == 1) {
      out[0] = out[1] = stbi__div4(3*in_near[0] + in_far[0] + 2);
      return out;
   }

   t1 = 3*in_near[0] + in_far[0];
   out[0] = stbi__div4(t1+2);
   for (i=1; i < w; ++i) {
      t0 = t1;
      t1 = 3*in_near[i]+in_far[i];
      out[i*2-1] = stbi__div16(3*t0 + t1 + 8);
      out[i*2  ] = stbi__div16(3*t1 + t0 + 8);
   }
   out[w*2-1] = stbi__div4(t1+2);

   STBI_NOTUSED(hs);

   return out;
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static stbi_uc *stbi__resample_row_hv_2_simd(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate 2x2 samples for every one in input
   int i=0,t0,t1;

   if (w == 1) {
      out[0] = out[1] = stbi__div4(3*in_near[0] + in_far[0] + 2);
      return out;
   }

   t1 = 3*in_near[0] + in_far[0];
   // process groups of 8 pixels for as long as we can.
   // note we can't handle the last pixel in a row in this loop
   // because we need to handle the filter boundary conditions.
   for (; i < ((w-1) & ~7); i += 8) {
#if defined(STBI_SSE2)
      // load and perform the vertical filtering pass
      // this uses 3*x + y = 4*x + (y - x)
      __m128i zero  = _mm_setzero_si128();
      __m128i farb  = _mm_loadl_epi64((__m128i *) (in_far + i));
      __m128i nearb = _mm_loadl_epi64((__m128i *) (in_near + i));
      __m128i farw  = _mm_unpacklo_epi8(farb, zero);
      __m128i nearw = _mm_unpacklo_epi8(nearb, zero);
      __m128i diff  = _mm_sub_epi16(farw, nearw);
      __m128i nears = _mm_slli_epi16(nearw, 2);
      __m128i curr  = _mm_add_epi16(nears, diff); // current row

      // horizontal filter works the same based on shifted vers of current
      // row. "prev" is current row shifted right by 1 pixel; we need to
      // insert the previous pixel value (from t1).
      // "next" is current row shifted left by 1 pixel, with first pixel
      // of next block of 8 pixels added in.
      __m128i prv0 = _mm_slli_si128(curr, 2);
      __m128i nxt0 = _mm_srli_si128(curr, 2);
      __m128i prev = _mm_insert_epi16(prv0, t1, 0);
      __m128i next = _mm_insert_epi16(nxt0, 3*in_near[i+8] + in_far[i+8], 7);

      // horizontal filter, polyphase implementation since it's convenient:
      // even pixels = 3*cur + prev = cur*4 + (prev - cur)
      // odd  pixels = 3*cur + next = cur*4 + (next - cur)
      // note the shared term.
      __m128i bias  = _mm_set1_epi16(8);
      __m128i curs = _mm_slli_epi16(curr, 2);
      __m128i prvd = _mm_sub_epi16(prev, curr);
      __m128i nxtd = _mm_sub_epi16(next, curr);
      __m128i curb = _mm_add_epi16(curs, bias);
      __m128i even = _mm_add_epi16(prvd, curb);
      __m128i odd  = _mm_add_epi16(nxtd, curb);

      // interleave even and odd pixels, then undo scaling.
      __m128i int0 = _mm_unpacklo_epi16(even, odd);
      __m128i int1 = _mm_unpackhi_epi16(even, odd);
      __m128i de0  = _mm_srli_epi16(int0, 4);
      __m128i de1  = _mm_srli_epi16(int1, 4);

      // pack and write output
      __m128i outv = _mm_packus_epi16(de0, de1);
      _mm_storeu_si128((__m128i *) (out + i*2), outv);
#elif defined(STBI_NEON)
      // load and perform the vertical filtering pass
      // this uses 3*x + y = 4*x + (y - x)
      uint8x8_t farb  = vld1_u8(in_far + i);
      uint8x8_t nearb = vld1_u8(in_near + i);
      int16x8_t diff  = vreinterpretq_s16_u16(vsubl_u8(farb, nearb));
      int16x8_t nears = vreinterpretq_s16_u16(vshll_n_u8(nearb, 2));
      int16x8_t curr  = vaddq_s16(nears, diff); // current row

      // horizontal filter works the same based on shifted vers of current
      // row. "prev" is current row shifted right by 1 pixel; we need to
      // insert the previous pixel value (from t1).
      // "next" is current row shifted left by 1 pixel, with first pixel
      // of next block of 8 pixels added in.
      int16x8_t prv0 = vextq_s16(curr, curr, 7);
      int16x8_t nxt0 = vextq_s16(curr, curr, 1);
      int16x8_t prev = vsetq_lane_s16(t1, prv0, 0);
      int16x8_t next = vsetq_lane_s16(3*in_near[i+8] + in_far[i+8], nxt0, 7);

      // horizontal filter, polyphase implementation since it's convenient:
      // even pixels = 3*cur + prev = cur*4 + (prev - cur)
      // odd  pixels = 3*cur + next = cur*4 + (next - cur)
      // note the shared term.
      int16x8_t curs = vshlq_n_s16(curr, 2);
      int16x8_t prvd = vsubq_s16(prev, curr);
      int16x8_t nxtd = vsubq_s16(next, curr);
      int16x8_t even = vaddq_s16(curs, prvd);
      int16x8_t odd  = vaddq_s16(curs, nxtd);

      // undo scaling and round, then store with even/odd phases interleaved
      uint8x8x2_t o;
      o.val[0] = vqrshrun_n_s16(even, 4);
      o.val[1] = vqrshrun_n_s16(odd,  4);
      vst2_u8(out + i*2, o);
#endif

      // "previous" value for next iter
      t1 = 3*in_near[i+7] + in_far[i+7];
   }

   t0 = t1;
   t1 = 3*in_near[i] + in_far[i];
   out[i*2] = stbi__div16(3*t1 + t0 + 8);

   for (++i; i < w; ++i) {
      t0 = t1;
      t1 = 3*in_near[i]+in_far[i];
      out[i*2-1] = stbi__div16(3*t0 + t1 + 8);
      out[i*2  ] = stbi__div16(3*t1 + t0 + 8);
   }
   out[w*2-1] = stbi__div4(t1+2);

   STBI_NOTUSED(hs);

   return out;
}
#endif

static stbi_uc *stbi__resample_row_generic(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // resample with nearest-neighbor
   int i,j;
   STBI_NOTUSED(in_far);
   for (i=0; i < w; ++i)
      for (j=0; j < hs; ++j)
         out[i*hs+j] = in_near[i];
   return out;
}

// this is a reduced-precision calculation of YCbCr-to-RGB introduced
// to make sure the code produces the same results in both SIMD and scalar
#define stbi__float2fixed(x)  (((int) ((x) * 4096.0f + 0.5f)) << 8)
static void stbi__YCbCr_to_RGB_row(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step)
{
   int i;
   for (i=0; i < count; ++i) {
      int y_fixed = (y[i] << 20) + (1<<19); // rounding
      int r,g,b;
      int cr = pcr[i] - 128;
      int cb = pcb[i] - 128;
      r = y_fixed +  cr* stbi__float2fixed(1.40200f);
      g = y_fixed + (cr*-stbi__float2fixed(0.71414f)) + ((cb*-stbi__float2fixed(0.34414f)) & 0xffff0000);
      b = y_fixed                                     +   cb* stbi__float2fixed(1.77200f);
      r >>= 20;
      g >>= 20;
      b >>= 20;
      if ((unsigned) r > 255) { if (r < 0) r = 0; else r = 255; }
      if ((unsigned) g > 255) { if (g < 0) g = 0; else g = 255; }
      if ((unsigned) b > 255) { if (b < 0) b = 0; else b = 255; }
      out[0] = (stbi_uc)r;
      out[1] = (stbi_uc)g;
      out[2] = (stbi_uc)b;
      out[3] = 255;
      out += step;
   }
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static void stbi__YCbCr_to_RGB_simd(stbi_uc *out, stbi_uc const *y, stbi_uc const *pcb, stbi_uc const *pcr, int count, int step)
{
   int i = 0;

#ifdef STBI_SSE2
   // step == 3 is pretty ugly on the final interleave, and i'm not convinced
   // it's useful in practice (you wouldn't use it for textures, for example).
   // so just accelerate step == 4 case.
   if (step == 4) {
      // this is a fairly straightforward implementation and not super-optimized.
      __m128i signflip  = _mm_set1_epi8(-0x80);
      __m128i cr_const0 = _mm_set1_epi16(   (short) ( 1.40200f*4096.0f+0.5f));
      __m128i cr_const1 = _mm_set1_epi16( - (short) ( 0.71414f*4096.0f+0.5f));
      __m128i cb_const0 = _mm_set1_epi16( - (short) ( 0.34414f*4096.0f+0.5f));
      __m128i cb_const1 = _mm_set1_epi16(   (short) ( 1.77200f*4096.0f+0.5f));
      __m128i y_bias = _mm_set1_epi8((char) (unsigned char) 128);
      __m128i xw = _mm_set1_epi16(255); // alpha channel

      for (; i+7 < count; i += 8) {
         // load
         __m128i y_bytes = _mm_loadl_epi64((__m128i *) (y+i));
         __m128i cr_bytes = _mm_loadl_epi64((__m128i *) (pcr+i));
         __m128i cb_bytes = _mm_loadl_epi64((__m128i *) (pcb+i));
         __m128i cr_biased = _mm_xor_si128(cr_bytes, signflip); // -128
         __m128i cb_biased = _mm_xor_si128(cb_bytes, signflip); // -128

         // unpack to short (and left-shift cr, cb by 8)
         __m128i yw  = _mm_unpacklo_epi8(y_bias, y_bytes);
         __m128i crw = _mm_unpacklo_epi8(_mm_setzero_si128(), cr_biased);
         __m128i cbw = _mm_unpacklo_epi8(_mm_setzero_si128(), cb_biased);

         // color transform
         __m128i yws = _mm_srli_epi16(yw, 4);
         __m128i cr0 = _mm_mulhi_epi16(cr_const0, crw);
         __m128i cb0 = _mm_mulhi_epi16(cb_const0, cbw);
         __m128i cb1 = _mm_mulhi_epi16(cbw, cb_const1);
         __m128i cr1 = _mm_mulhi_epi16(crw, cr_const1);
         __m128i rws = _mm_add_epi16(cr0, yws);
         __m128i gwt = _mm_add_epi16(cb0, yws);
         __m128i bws = _mm_add_epi16(yws, cb1);
         __m128i gws = _mm_add_epi16(gwt, cr1);

         // descale
         __m128i rw = _mm_srai_epi16(rws, 4);
         __m128i bw = _mm_srai_epi16(bws, 4);
         __m128i gw = _mm_srai_epi16(gws, 4);

         // back to byte, set up for transpose
         __m128i brb = _mm_packus_epi16(rw, bw);
         __m128i gxb = _mm_packus_epi16(gw, xw);

         // transpose to interleave channels
         __m128i t0 = _mm_unpacklo_epi8(brb, gxb);
         __m128i t1 = _mm_unpackhi_epi8(brb, gxb);
         __m128i o0 = _mm_unpacklo_epi16(t0, t1);
         __m128i o1 = _mm_unpackhi_epi16(t0, t1);

         // store
         _mm_storeu_si128((__m128i *) (out + 0), o0);
         _mm_storeu_si128((__m128i *) (out + 16), o1);
         out += 32;
      }
   }
#endif

#ifdef STBI_NEON
   // in this version, step=3 support would be easy to add. but is there demand?
   if (step == 4) {
      // this is a fairly straightforward implementation and not super-optimized.
      uint8x8_t signflip = vdup_n_u8(0x80);
      int16x8_t cr_const0 = vdupq_n_s16(   (short) ( 1.40200f*4096.0f+0.5f));
      int16x8_t cr_const1 = vdupq_n_s16( - (short) ( 0.71414f*4096.0f+0.5f));
      int16x8_t cb_const0 = vdupq_n_s16( - (short) ( 0.34414f*4096.0f+0.5f));
      int16x8_t cb_const1 = vdupq_n_s16(   (short) ( 1.77200f*4096.0f+0.5f));

      for (; i+7 < count; i += 8) {
         // load
         uint8x8_t y_bytes  = vld1_u8(y + i);
         uint8x8_t cr_bytes = vld1_u8(pcr + i);
         uint8x8_t cb_bytes = vld1_u8(pcb + i);
         int8x8_t cr_biased = vreinterpret_s8_u8(vsub_u8(cr_bytes, signflip));
         int8x8_t cb_biased = vreinterpret_s8_u8(vsub_u8(cb_bytes, signflip));

         // expand to s16
         int16x8_t yws = vreinterpretq_s16_u16(vshll_n_u8(y_bytes, 4));
         int16x8_t crw = vshll_n_s8(cr_biased, 7);
         int16x8_t cbw = vshll_n_s8(cb_biased, 7);

         // color transform
         int16x8_t cr0 = vqdmulhq_s16(crw, cr_const0);
         int16x8_t cb0 = vqdmulhq_s16(cbw, cb_const0);
         int16x8_t cr1 = vqdmulhq_s16(crw, cr_const1);
         int16x8_t cb1 = vqdmulhq_s16(cbw, cb_const1);
         int16x8_t rws = vaddq_s16(yws, cr0);
         int16x8_t gws = vaddq_s16(vaddq_s16(yws, cb0), cr1);
         int16x8_t bws = vaddq_s16(yws, cb1);

         // undo scaling, round, convert to byte
         uint8x8x4_t o;
         o.val[0] = vqrshrun_n_s16(rws, 4);
         o.val[1] = vqrshrun_n_s16(gws, 4);
         o.val[2] = vqrshrun_n_s16(bws, 4);
         o.val[3] = vdup_n_u8(255);

         // store, interleaving r/g/b/a
         vst4_u8(out, o);
         out += 8*4;
      }
   }
#endif

   for (; i < count; ++i) {
      int y_fixed = (y[i] << 20) + (1<<19); // rounding
      int r,g,b;
      int cr = pcr[i] - 128;
      int cb = pcb[i] - 128;
      r = y_fixed + cr* stbi__float2fixed(1.40200f);
      g = y_fixed + cr*-stbi__float2fixed(0.71414f) + ((cb*-stbi__float2fixed(0.34414f)) & 0xffff0000);
      b = y_fixed                                   +   cb* stbi__float2fixed(1.77200f);
      r >>= 20;
      g >>= 20;
      b >>= 20;
      if ((unsigned) r > 255) { if (r < 0) r = 0; else r = 255; }
      if ((unsigned) g > 255) { if (g < 0) g = 0; else g = 255; }
      if ((unsigned) b > 255) { if (b < 0) b = 0; else b = 255; }
      out[0] = (stbi_uc)r;
      out[1] = (stbi_uc)g;
      out[2] = (stbi_uc)b;
      out[3] = 255;
      out += step;
   }
}
#endif

// set up the kernels
static void stbi__setup_jpeg(stbi__jpeg *j)
{
   j->idct_block_kernel = stbi__idct_block;
   j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_row;
   j->resample_row_hv_2_kernel = stbi__resample_row_hv_2;

#ifdef STBI_SSE2
   if (stbi__sse2_available()) {
      j->idct_block_kernel = stbi__idct_simd;
      j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
      j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
   }
#endif

#ifdef STBI_NEON
   j->idct_block_kernel = stbi__idct_simd;
   j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
   j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
#endif
}

// clean up the temporary component buffers
static void stbi__cleanup_jpeg(stbi__jpeg *j)
{
   stbi__free_jpeg_components(j, j->s->img_n, 0);
}

typedef struct
{
   resample_row_func resample;
   stbi_uc *line0,*line1;
   int hs,vs;   // expansion factor in each axis
   int w_lores; // horizontal pixels pre-expansion
   int ystep;   // how far through vertical expansion we are
   int ypos;    // which pre-expansion row we're on
} stbi__resample;

// fast 0..255 * 0..255 => 0..255 rounded multiplication
static stbi_uc stbi__blinn_8x8(stbi_uc x, stbi_uc y)
{
   unsigned int t = x*y + 128;
   return (stbi_uc) ((t + (t >>8)) >> 8);
}

static stbi_uc *load_jpeg_image(stbi__jpeg *z, int *out_x, int *out_y, int *comp, int req_comp)
{
   int n, decode_n, is_rgb;
   z->s->img_n = 0; // make stbi__cleanup_jpeg safe

   // validate req_comp
   if (req_comp < 0 || req_comp > 4) return stbi__errpuc("bad req_comp", "Internal error");

   // load a jpeg image from whichever source, but leave in YCbCr format
   if (!stbi__decode_jpeg_image(z)) { stbi__cleanup_jpeg(z); return NULL; }

   // determine actual number of components to generate
   n = req_comp ? req_comp : z->s->img_n >= 3 ? 3 : 1;

   is_rgb = z->s->img_n == 3 && (z->rgb == 3 || (z->app14_color_transform == 0 && !z->jfif));

   if (z->s->img_n == 3 && n < 3 && !is_rgb)
      decode_n = 1;
   else
      decode_n = z->s->img_n;

   // nothing to do if no components requested; check this now to avoid
   // accessing uninitialized coutput[0] later
   if (decode_n <= 0) { stbi__cleanup_jpeg(z); return NULL; }

   // resample and color-convert
   {
      int k;
      unsigned int i,j;
      stbi_uc *output;
      stbi_uc *coutput[4] = { NULL, NULL, NULL, NULL };

      stbi__resample res_comp[4];

      for (k=0; k < decode_n; ++k) {
         stbi__resample *r = &res_comp[k];

         // allocate line buffer big enough for upsampling off the edges
         // with upsample factor of 4
         z->img_comp[k].linebuf = (stbi_uc *) stbi__malloc(z->s->img_x + 3);
         if (!z->img_comp[k].linebuf) { stbi__cleanup_jpeg(z); return stbi__errpuc("outofmem", "Out of memory"); }

         r->hs      = z->img_h_max / z->img_comp[k].h;
         r->vs      = z->img_v_max / z->img_comp[k].v;
         r->ystep   = r->vs >> 1;
         r->w_lores = (z->s->img_x + r->hs-1) / r->hs;
         r->ypos    = 0;
         r->line0   = r->line1 = z->img_comp[k].data;

         if      (r->hs == 1 && r->vs == 1) r->resample = resample_row_1;
         else if (r->hs == 1 && r->vs == 2) r->resample = stbi__resample_row_v_2;
         else if (r->hs == 2 && r->vs == 1) r->resample = stbi__resample_row_h_2;
         else if (r->hs == 2 && r->vs == 2) r->resample = z->resample_row_hv_2_kernel;
         else                               r->resample = stbi__resample_row_generic;
      }

      // can't error after this so, this is safe
      output = (stbi_uc *) stbi__malloc_mad3(n, z->s->img_x, z->s->img_y, 1);
      if (!output) { stbi__cleanup_jpeg(z); return stbi__errpuc("outofmem", "Out of memory"); }

      // now go ahead and resample
      for (j=0; j < z->s->img_y; ++j) {
         stbi_uc *out = output + n * z->s->img_x * j;
         for (k=0; k < decode_n; ++k) {
            stbi__resample *r = &res_comp[k];
            int y_bot = r->ystep >= (r->vs >> 1);
            coutput[k] = r->resample(z->img_comp[k].linebuf,
                                     y_bot ? r->line1 : r->line0,
                                     y_bot ? r->line0 : r->line1,
                                     r->w_lores, r->hs);
            if (++r->ystep >= r->vs) {
               r->ystep = 0;
               r->line0 = r->line1;
               if (++r->ypos < z->img_comp[k].y)
                  r->line1 += z->img_comp[k].w2;
            }
         }
         if (n >= 3) {
            stbi_uc *y = coutput[0];
            if (z->s->img_n == 3) {
               if (is_rgb) {
                  for (i=0; i < z->s->img_x; ++i) {
                     out[0] = y[i];
                     out[1] = coutput[1][i];
                     out[2] = coutput[2][i];
                     out[3] = 255;
                     out += n;
                  }
               } else {
                  z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
               }
            } else if (z->s->img_n == 4) {
               if (z->app14_color_transform == 0) { // CMYK
                  for (i=0; i < z->s->img_x; ++i) {
                     stbi_uc m = coutput[3][i];
                     out[0] = stbi__blinn_8x8(coutput[0][i], m);
                     out[1] = stbi__blinn_8x8(coutput[1][i], m);
                     out[2] = stbi__blinn_8x8(coutput[2][i], m);
                     out[3] = 255;
                     out += n;
                  }
               } else if (z->app14_color_transform == 2) { // YCCK
                  z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
                  for (i=0; i < z->s->img_x; ++i) {
                     stbi_uc m = coutput[3][i];
                     out[0] = stbi__blinn_8x8(255 - out[0], m);
                     out[1] = stbi__blinn_8x8(255 - out[1], m);
                     out[2] = stbi__blinn_8x8(255 - out[2], m);
                     out += n;
                  }
               } else { // YCbCr + alpha?  Ignore the fourth channel for now
                  z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
               }
            } else
               for (i=0; i < z->s->img_x; ++i) {
                  out[0] = out[1] = out[2] = y[i];
                  out[3] = 255; // not used if n==3
                  out += n;
               }
         } else {
            if (is_rgb) {
               if (n == 1)
                  for (i=0; i < z->s->img_x; ++i)
                     *out++ = stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
               else {
                  for (i=0; i < z->s->img_x; ++i, out += 2) {
                     out[0] = stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
                     out[1] = 255;
                  }
               }
            } else if (z->s->img_n == 4 && z->app14_color_transform == 0) {
               for (i=0; i < z->s->img_x; ++i) {
                  stbi_uc m = coutput[3][i];
                  stbi_uc r = stbi__blinn_8x8(coutput[0][i], m);
                  stbi_uc g = stbi__blinn_8x8(coutput[1][i], m);
                  stbi_uc b = stbi__blinn_8x8(coutput[2][i], m);
                  out[0] = stbi__compute_y(r, g, b);
                  out[1] = 255;
                  out += n;
               }
            } else if (z->s->img_n == 4 && z->app14_color_transform == 2) {
               for (i=0; i < z->s->img_x; ++i) {
                  out[0] = stbi__blinn_8x8(255 - coutput[0][i], coutput[3][i]);
                  out[1] = 255;
                  out += n;
               }
            } else {
               stbi_uc *y = coutput[0];
               if (n == 1)
                  for (i=0; i < z->s->img_x; ++i) out[i] = y[i];
               else
                  for (i=0; i < z->s->img_x; ++i) { *out++ = y[i]; *out++ = 255; }
            }
         }
      }
      stbi__cleanup_jpeg(z);
      *out_x = z->s->img_x;
      *out_y = z->s->img_y;
      if (comp) *comp = z->s->img_n >= 3 ? 3 : 1; // report original components, not output
      return output;
   }
}

static void *stbi__jpeg_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   unsigned char* result;
   stbi__jpeg* j = (stbi__jpeg*) stbi__malloc(sizeof(stbi__jpeg));
   if (!j) return stbi__errpuc("outofmem", "Out of memory");
   memset(j, 0, sizeof(stbi__jpeg));
   STBI_NOTUSED(ri);
   j->s = s;
   stbi__setup_jpeg(j);
   result = load_jpeg_image(j, x,y,comp,req_comp);
   STBI_FREE(j);
   return result;
}

static int stbi__jpeg_test(stbi__context *s)
{
   int r;
   stbi__jpeg* j = (stbi__jpeg*)stbi__malloc(sizeof(stbi__jpeg));
   if (!j) return stbi__err("outofmem", "Out of memory");
   memset(j, 0, sizeof(stbi__jpeg));
   j->s = s;
   stbi__setup_jpeg(j);
   r = stbi__decode_jpeg_header(j, STBI__SCAN_type);
   stbi__rewind(s);
   STBI_FREE(j);
   return r;
}

static int stbi__jpeg_info_raw(stbi__jpeg *j, int *x, int *y, int *comp)
{
   if (!stbi__decode_jpeg_header(j, STBI__SCAN_header)) {
      stbi__rewind( j->s );
      return 0;
   }
   if (x) *x = j->s->img_x;
   if (y) *y = j->s->img_y;
   if (comp) *comp = j->s->img_n >= 3 ? 3 : 1;
   return 1;
}

static int stbi__jpeg_info(stbi__context *s, int *x, int *y, int *comp)
{
   int result;
   stbi__jpeg* j = (stbi__jpeg*) (stbi__malloc(sizeof(stbi__jpeg)));
   if (!j) return stbi__err("outofmem", "Out of memory");
   memset(j, 0, sizeof(stbi__jpeg));
   j->s = s;
   result = stbi__jpeg_info_raw(j, x, y, comp);
   STBI_FREE(j);
   return result;
}
#endif

// public domain zlib decode    v0.2  Sean Barrett 2006-11-18
//    simple implementation
//      - all input must be provided in an upfront buffer
//      - all output is written to a single output buffer (can malloc/realloc)
//    performance
//      - fast huffman

#ifndef STBI_NO_ZLIB

// fast-way is faster to check than jpeg huffman, but slow way is slower
#define STBI__ZFAST_BITS  9 // accelerate all cases in default tables
#define STBI__ZFAST_MASK  ((1 << STBI__ZFAST_BITS) - 1)
#define STBI__ZNSYMS 288 // number of symbols in literal/length alphabet

// zlib-style huffman encoding
// (jpegs packs from left, zlib from right, so can't share code)
typedef struct
{
   stbi__uint16 fast[1 << STBI__ZFAST_BITS];
   stbi__uint16 firstcode[16];
   int maxcode[17];
   stbi__uint16 firstsymbol[16];
   stbi_uc  size[STBI__ZNSYMS];
   stbi__uint16 value[STBI__ZNSYMS];
} stbi__zhuffman;

stbi_inline static int stbi__bitreverse16(int n)
{
  n = ((n & 0xAAAA) >>  1) | ((n & 0x5555) << 1);
  n = ((n & 0xCCCC) >>  2) | ((n & 0x3333) << 2);
  n = ((n & 0xF0F0) >>  4) | ((n & 0x0F0F) << 4);
  n = ((n & 0xFF00) >>  8) | ((n & 0x00FF) << 8);
  return n;
}

stbi_inline static int stbi__bit_reverse(int v, int bits)
{
   STBI_ASSERT(bits <= 16);
   // to bit reverse n bits, reverse 16 and shift
   // e.g. 11 bits, bit reverse and shift away 5
   return stbi__bitreverse16(v) >> (16-bits);
}

static int stbi__zbuild_huffman(stbi__zhuffman *z, const stbi_uc *sizelist, int num)
{
   int i,k=0;
   int code, next_code[16], sizes[17];

   // DEFLATE spec for generating codes
   memset(sizes, 0, sizeof(sizes));
   memset(z->fast, 0, sizeof(z->fast));
   for (i=0; i < num; ++i)
      ++sizes[sizelist[i]];
   sizes[0] = 0;
   for (i=1; i < 16; ++i)
      if (sizes[i] > (1 << i))
         return stbi__err("bad sizes", "Corrupt PNG");
   code = 0;
   for (i=1; i < 16; ++i) {
      next_code[i] = code;
      z->firstcode[i] = (stbi__uint16) code;
      z->firstsymbol[i] = (stbi__uint16) k;
      code = (code + sizes[i]);
      if (sizes[i])
         if (code-1 >= (1 << i)) return stbi__err("bad codelengths","Corrupt PNG");
      z->maxcode[i] = code << (16-i); // preshift for inner loop
      code <<= 1;
      k += sizes[i];
   }
   z->maxcode[16] = 0x10000; // sentinel
   for (i=0; i < num; ++i) {
      int s = sizelist[i];
      if (s) {
         int c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
         stbi__uint16 fastv = (stbi__uint16) ((s << 9) | i);
         z->size [c] = (stbi_uc     ) s;
         z->value[c] = (stbi__uint16) i;
         if (s <= STBI__ZFAST_BITS) {
            int j = stbi__bit_reverse(next_code[s],s);
            while (j < (1 << STBI__ZFAST_BITS)) {
               z->fast[j] = fastv;
               j += (1 << s);
            }
         }
         ++next_code[s];
      }
   }
   return 1;
}

// zlib-from-memory implementation for PNG reading
//    because PNG allows splitting the zlib stream arbitrarily,
//    and it's annoying structurally to have PNG call ZLIB call PNG,
//    we require PNG read all the IDATs and combine them into a single
//    memory buffer

typedef struct
{
   stbi_uc *zbuffer, *zbuffer_end;
   int num_bits;
   int hit_zeof_once;
   stbi__uint32 code_buffer;

   char *zout;
   char *zout_start;
   char *zout_end;
   int   z_expandable;

   stbi__zhuffman z_length, z_distance;
} stbi__zbuf;

stbi_inline static int stbi__zeof(stbi__zbuf *z)
{
   return (z->zbuffer >= z->zbuffer_end);
}

stbi_inline static stbi_uc stbi__zget8(stbi__zbuf *z)
{
   return stbi__zeof(z) ? 0 : *z->zbuffer++;
}

static void stbi__fill_bits(stbi__zbuf *z)
{
   do {
      if (z->code_buffer >= (1U << z->num_bits)) {
        z->zbuffer = z->zbuffer_end;  /* treat this as EOF so we fail. */
        return;
      }
      z->code_buffer |= (unsigned int) stbi__zget8(z) << z->num_bits;
      z->num_bits += 8;
   } while (z->num_bits <= 24);
}

stbi_inline static unsigned int stbi__zreceive(stbi__zbuf *z, int n)
{
   unsigned int k;
   if (z->num_bits < n) stbi__fill_bits(z);
   k = z->code_buffer & ((1 << n) - 1);
   z->code_buffer >>= n;
   z->num_bits -= n;
   return k;
}

static int stbi__zhuffman_decode_slowpath(stbi__zbuf *a, stbi__zhuffman *z)
{
   int b,s,k;
   // not resolved by fast table, so compute it the slow way
   // use jpeg approach, which requires MSbits at top
   k = stbi__bit_reverse(a->code_buffer, 16);
   for (s=STBI__ZFAST_BITS+1; ; ++s)
      if (k < z->maxcode[s])
         break;
   if (s >= 16) return -1; // invalid code!
   // code size is s, so:
   b = (k >> (16-s)) - z->firstcode[s] + z->firstsymbol[s];
   if (b >= STBI__ZNSYMS) return -1; // some data was corrupt somewhere!
   if (z->size[b] != s) return -1;  // was originally an assert, but report failure instead.
   a->code_buffer >>= s;
   a->num_bits -= s;
   return z->value[b];
}

stbi_inline static int stbi__zhuffman_decode(stbi__zbuf *a, stbi__zhuffman *z)
{
   int b,s;
   if (a->num_bits < 16) {
      if (stbi__zeof(a)) {
         if (!a->hit_zeof_once) {
            // This is the first time we hit eof, insert 16 extra padding btis
            // to allow us to keep going; if we actually consume any of them
            // though, that is invalid data. This is caught later.
            a->hit_zeof_once = 1;
            a->num_bits += 16; // add 16 implicit zero bits
         } else {
            // We already inserted our extra 16 padding bits and are again
            // out, this stream is actually prematurely terminated.
            return -1;
         }
      } else {
         stbi__fill_bits(a);
      }
   }
   b = z->fast[a->code_buffer & STBI__ZFAST_MASK];
   if (b) {
      s = b >> 9;
      a->code_buffer >>= s;
      a->num_bits -= s;
      return b & 511;
   }
   return stbi__zhuffman_decode_slowpath(a, z);
}

static int stbi__zexpand(stbi__zbuf *z, char *zout, int n)  // need to make room for n bytes
{
   char *q;
   unsigned int cur, limit, old_limit;
   z->zout = zout;
   if (!z->z_expandable) return stbi__err("output buffer limit","Corrupt PNG");
   cur   = (unsigned int) (z->zout - z->zout_start);
   limit = old_limit = (unsigned) (z->zout_end - z->zout_start);
   if (UINT_MAX - cur < (unsigned) n) return stbi__err("outofmem", "Out of memory");
   while (cur + n > limit) {
      if(limit > UINT_MAX / 2) return stbi__err("outofmem", "Out of memory");
      limit *= 2;
   }
   q = (char *) STBI_REALLOC_SIZED(z->zout_start, old_limit, limit);
   STBI_NOTUSED(old_limit);
   if (q == NULL) return stbi__err("outofmem", "Out of memory");
   z->zout_start = q;
   z->zout       = q + cur;
   z->zout_end   = q + limit;
   return 1;
}

static const int stbi__zlength_base[31] = {
   3,4,5,6,7,8,9,10,11,13,
   15,17,19,23,27,31,35,43,51,59,
   67,83,99,115,131,163,195,227,258,0,0 };

static const int stbi__zlength_extra[31]=
{ 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };

static const int stbi__zdist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};

static const int stbi__zdist_extra[32] =
{ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

static int stbi__parse_huffman_block(stbi__zbuf *a)
{
   char *zout = a->zout;
   for(;;) {
      int z = stbi__zhuffman_decode(a, &a->z_length);
      if (z < 256) {
         if (z < 0) return stbi__err("bad huffman code","Corrupt PNG"); // error in huffman codes
         if (zout >= a->zout_end) {
            if (!stbi__zexpand(a, zout, 1)) return 0;
            zout = a->zout;
         }
         *zout++ = (char) z;
      } else {
         stbi_uc *p;
         int len,dist;
         if (z == 256) {
            a->zout = zout;
            if (a->hit_zeof_once && a->num_bits < 16) {
               // The first time we hit zeof, we inserted 16 extra zero bits into our bit
               // buffer so the decoder can just do its speculative decoding. But if we
               // actually consumed any of those bits (which is the case when num_bits < 16),
               // the stream actually read past the end so it is malformed.
               return stbi__err("unexpected end","Corrupt PNG");
            }
            return 1;
         }
         if (z >= 286) return stbi__err("bad huffman code","Corrupt PNG"); // per DEFLATE, length codes 286 and 287 must not appear in compressed data
         z -= 257;
         len = stbi__zlength_base[z];
         if (stbi__zlength_extra[z]) len += stbi__zreceive(a, stbi__zlength_extra[z]);
         z = stbi__zhuffman_decode(a, &a->z_distance);
         if (z < 0 || z >= 30) return stbi__err("bad huffman code","Corrupt PNG"); // per DEFLATE, distance codes 30 and 31 must not appear in compressed data
         dist = stbi__zdist_base[z];
         if (stbi__zdist_extra[z]) dist += stbi__zreceive(a, stbi__zdist_extra[z]);
         if (zout - a->zout_start < dist) return stbi__err("bad dist","Corrupt PNG");
         if (len > a->zout_end - zout) {
            if (!stbi__zexpand(a, zout, len)) return 0;
            zout = a->zout;
         }
         p = (stbi_uc *) (zout - dist);
         if (dist == 1) { // run of one byte; common in images.
            stbi_uc v = *p;
            if (len) { do *zout++ = v; while (--len); }
         } else {
            if (len) { do *zout++ = *p++; while (--len); }
         }
      }
   }
}

static int stbi__compute_huffman_codes(stbi__zbuf *a)
{
   static const stbi_uc length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
   stbi__zhuffman z_codelength;
   stbi_uc lencodes[286+32+137];//padding for maximum single op
   stbi_uc codelength_sizes[19];
   int i,n;

   int hlit  = stbi__zreceive(a,5) + 257;
   int hdist = stbi__zreceive(a,5) + 1;
   int hclen = stbi__zreceive(a,4) + 4;
   int ntot  = hlit + hdist;

   memset(codelength_sizes, 0, sizeof(codelength_sizes));
   for (i=0; i < hclen; ++i) {
      int s = stbi__zreceive(a,3);
      codelength_sizes[length_dezigzag[i]] = (stbi_uc) s;
   }
   if (!stbi__zbuild_huffman(&z_codelength, codelength_sizes, 19)) return 0;

   n = 0;
   while (n < ntot) {
      int c = stbi__zhuffman_decode(a, &z_codelength);
      if (c < 0 || c >= 19) return stbi__err("bad codelengths", "Corrupt PNG");
      if (c < 16)
         lencodes[n++] = (stbi_uc) c;
      else {
         stbi_uc fill = 0;
         if (c == 16) {
            c = stbi__zreceive(a,2)+3;
            if (n == 0) return stbi__err("bad codelengths", "Corrupt PNG");
            fill = lencodes[n-1];
         } else if (c == 17) {
            c = stbi__zreceive(a,3)+3;
         } else if (c == 18) {
            c = stbi__zreceive(a,7)+11;
         } else {
            return stbi__err("bad codelengths", "Corrupt PNG");
         }
         if (ntot - n < c) return stbi__err("bad codelengths", "Corrupt PNG");
         memset(lencodes+n, fill, c);
         n += c;
      }
   }
   if (n != ntot) return stbi__err("bad codelengths","Corrupt PNG");
   if (!stbi__zbuild_huffman(&a->z_length, lencodes, hlit)) return 0;
   if (!stbi__zbuild_huffman(&a->z_distance, lencodes+hlit, hdist)) return 0;
   return 1;
}

static int stbi__parse_uncompressed_block(stbi__zbuf *a)
{
   stbi_uc header[4];
   int len,nlen,k;
   if (a->num_bits & 7)
      stbi__zreceive(a, a->num_bits & 7); // discard
   // drain the bit-packed data into header
   k = 0;
   while (a->num_bits > 0) {
      header[k++] = (stbi_uc) (a->code_buffer & 255); // suppress MSVC run-time check
      a->code_buffer >>= 8;
      a->num_bits -= 8;
   }
   if (a->num_bits < 0) return stbi__err("zlib corrupt","Corrupt PNG");
   // now fill header the normal way
   while (k < 4)
      header[k++] = stbi__zget8(a);
   len  = header[1] * 256 + header[0];
   nlen = header[3] * 256 + header[2];
   if (nlen != (len ^ 0xffff)) return stbi__err("zlib corrupt","Corrupt PNG");
   if (a->zbuffer + len > a->zbuffer_end) return stbi__err("read past buffer","Corrupt PNG");
   if (a->zout + len > a->zout_end)
      if (!stbi__zexpand(a, a->zout, len)) return 0;
   memcpy(a->zout, a->zbuffer, len);
   a->zbuffer += len;
   a->zout += len;
   return 1;
}

static int stbi__parse_zlib_header(stbi__zbuf *a)
{
   int cmf   = stbi__zget8(a);
   int cm    = cmf & 15;
   /* int cinfo = cmf >> 4; */
   int flg   = stbi__zget8(a);
   if (stbi__zeof(a)) return stbi__err("bad zlib header","Corrupt PNG"); // zlib spec
   if ((cmf*256+flg) % 31 != 0) return stbi__err("bad zlib header","Corrupt PNG"); // zlib spec
   if (flg & 32) return stbi__err("no preset dict","Corrupt PNG"); // preset dictionary not allowed in png
   if (cm != 8) return stbi__err("bad compression","Corrupt PNG"); // DEFLATE required for png
   // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
   return 1;
}

static const stbi_uc stbi__zdefault_length[STBI__ZNSYMS] =
{
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8
};
static const stbi_uc stbi__zdefault_distance[32] =
{
   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
};
/*
Init algorithm:
{
   int i;   // use <= to match clearly with spec
   for (i=0; i <= 143; ++i)     stbi__zdefault_length[i]   = 8;
   for (   ; i <= 255; ++i)     stbi__zdefault_length[i]   = 9;
   for (   ; i <= 279; ++i)     stbi__zdefault_length[i]   = 7;
   for (   ; i <= 287; ++i)     stbi__zdefault_length[i]   = 8;

   for (i=0; i <=  31; ++i)     stbi__zdefault_distance[i] = 5;
}
*/

static int stbi__parse_zlib(stbi__zbuf *a, int parse_header)
{
   int final, type;
   if (parse_header)
      if (!stbi__parse_zlib_header(a)) return 0;
   a->num_bits = 0;
   a->code_buffer = 0;
   a->hit_zeof_once = 0;
   do {
      final = stbi__zreceive(a,1);
      type = stbi__zreceive(a,2);
      if (type == 0) {
         if (!stbi__parse_uncompressed_block(a)) return 0;
      } else if (type == 3) {
         return 0;
      } else {
         if (type == 1) {
            // use fixed code lengths
            if (!stbi__zbuild_huffman(&a->z_length  , stbi__zdefault_length  , STBI__ZNSYMS)) return 0;
            if (!stbi__zbuild_huffman(&a->z_distance, stbi__zdefault_distance,  32)) return 0;
         } else {
            if (!stbi__compute_huffman_codes(a)) return 0;
         }
         if (!stbi__parse_huffman_block(a)) return 0;
      }
   } while (!final);
   return 1;
}

static int stbi__do_zlib(stbi__zbuf *a, char *obuf, int olen, int exp, int parse_header)
{
   a->zout_start = obuf;
   a->zout       = obuf;
   a->zout_end   = obuf + olen;
   a->z_expandable = exp;

   return stbi__parse_zlib(a, parse_header);
}

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen)
{
   stbi__zbuf a;
   char *p = (char *) stbi__malloc(initial_size);
   if (p == NULL) return NULL;
   a.zbuffer = (stbi_uc *) buffer;
   a.zbuffer_end = (stbi_uc *) buffer + len;
   if (stbi__do_zlib(&a, p, initial_size, 1, 1)) {
      if (outlen) *outlen = (int) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      STBI_FREE(a.zout_start);
      return NULL;
   }
}

STBIDEF char *stbi_zlib_decode_malloc(char const *buffer, int len, int *outlen)
{
   return stbi_zlib_decode_malloc_guesssize(buffer, len, 16384, outlen);
}

STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header)
{
   stbi__zbuf a;
   char *p = (char *) stbi__malloc(initial_size);
   if (p == NULL) return NULL;
   a.zbuffer = (stbi_uc *) buffer;
   a.zbuffer_end = (stbi_uc *) buffer + len;
   if (stbi__do_zlib(&a, p, initial_size, 1, parse_header)) {
      if (outlen) *outlen = (int) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      STBI_FREE(a.zout_start);
      return NULL;
   }
}

STBIDEF int stbi_zlib_decode_buffer(char *obuffer, int olen, char const *ibuffer, int ilen)
{
   stbi__zbuf a;
   a.zbuffer = (stbi_uc *) ibuffer;
   a.zbuffer_end = (stbi_uc *) ibuffer + ilen;
   if (stbi__do_zlib(&a, obuffer, olen, 0, 1))
      return (int) (a.zout - a.zout_start);
   else
      return -1;
}

STBIDEF char *stbi_zlib_decode_noheader_malloc(char const *buffer, int len, int *outlen)
{
   stbi__zbuf a;
   char *p = (char *) stbi__malloc(16384);
   if (p == NULL) return NULL;
   a.zbuffer = (stbi_uc *) buffer;
   a.zbuffer_end = (stbi_uc *) buffer+len;
   if (stbi__do_zlib(&a, p, 16384, 1, 0)) {
      if (outlen) *outlen = (int) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      STBI_FREE(a.zout_start);
      return NULL;
   }
}

STBIDEF int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen)
{
   stbi__zbuf a;
   a.zbuffer = (stbi_uc *) ibuffer;
   a.zbuffer_end = (stbi_uc *) ibuffer + ilen;
   if (stbi__do_zlib(&a, obuffer, olen, 0, 0))
      return (int) (a.zout - a.zout_start);
   else
      return -1;
}
#endif

// public domain "baseline" PNG decoder   v0.10  Sean Barrett 2006-11-18
//    simple implementation
//      - only 8-bit samples
//      - no CRC checking
//      - allocates lots of intermediate memory
//        - avoids problem of streaming data between subsystems
//        - avoids explicit window management
//    performance
//      - uses stb_zlib, a PD zlib implementation with fast huffman decoding

#ifndef STBI_NO_PNG
typedef struct
{
   stbi__uint32 length;
   stbi__uint32 type;
} stbi__pngchunk;

static stbi__pngchunk stbi__get_chunk_header(stbi__context *s)
{
   stbi__pngchunk c;
   c.length = stbi__get32be(s);
   c.type   = stbi__get32be(s);
   return c;
}

static int stbi__check_png_header(stbi__context *s)
{
   static const stbi_uc png_sig[8] = { 137,80,78,71,13,10,26,10 };
   int i;
   for (i=0; i < 8; ++i)
      if (stbi__get8(s) != png_sig[i]) return stbi__err("bad png sig","Not a PNG");
   return 1;
}

typedef struct
{
   stbi__context *s;
   stbi_uc *idata, *expanded, *out;
   int depth;
} stbi__png;


enum {
   STBI__F_none=0,
   STBI__F_sub=1,
   STBI__F_up=2,
   STBI__F_avg=3,
   STBI__F_paeth=4,
   // synthetic filter used for first scanline to avoid needing a dummy row of 0s
   STBI__F_avg_first
};

static stbi_uc first_row_filter[5] =
{
   STBI__F_none,
   STBI__F_sub,
   STBI__F_none,
   STBI__F_avg_first,
   STBI__F_sub // Paeth with b=c=0 turns out to be equivalent to sub
};

static int stbi__paeth(int a, int b, int c)
{
   // This formulation looks very different from the reference in the PNG spec, but is
   // actually equivalent and has favorable data dependencies and admits straightforward
   // generation of branch-free code, which helps performance significantly.
   int thresh = c*3 - (a + b);
   int lo = a < b ? a : b;
   int hi = a < b ? b : a;
   int t0 = (hi <= thresh) ? lo : c;
   int t1 = (thresh <= lo) ? hi : t0;
   return t1;
}

static const stbi_uc stbi__depth_scale_table[9] = { 0, 0xff, 0x55, 0, 0x11, 0,0,0, 0x01 };

// adds an extra all-255 alpha channel
// dest == src is legal
// img_n must be 1 or 3
static void stbi__create_png_alpha_expand8(stbi_uc *dest, stbi_uc *src, stbi__uint32 x, int img_n)
{
   int i;
   // must process data backwards since we allow dest==src
   if (img_n == 1) {
      for (i=x-1; i >= 0; --i) {
         dest[i*2+1] = 255;
         dest[i*2+0] = src[i];
      }
   } else {
      STBI_ASSERT(img_n == 3);
      for (i=x-1; i >= 0; --i) {
         dest[i*4+3] = 255;
         dest[i*4+2] = src[i*3+2];
         dest[i*4+1] = src[i*3+1];
         dest[i*4+0] = src[i*3+0];
      }
   }
}

// create the png data from post-deflated data
static int stbi__create_png_image_raw(stbi__png *a, stbi_uc *raw, stbi__uint32 raw_len, int out_n, stbi__uint32 x, stbi__uint32 y, int depth, int color)
{
   int bytes = (depth == 16 ? 2 : 1);
   stbi__context *s = a->s;
   stbi__uint32 i,j,stride = x*out_n*bytes;
   stbi__uint32 img_len, img_width_bytes;
   stbi_uc *filter_buf;
   int all_ok = 1;
   int k;
   int img_n = s->img_n; // copy it into a local for later

   int output_bytes = out_n*bytes;
   int filter_bytes = img_n*bytes;
   int width = x;

   STBI_ASSERT(out_n == s->img_n || out_n == s->img_n+1);
   a->out = (stbi_uc *) stbi__malloc_mad3(x, y, output_bytes, 0); // extra bytes to write off the end into
   if (!a->out) return stbi__err("outofmem", "Out of memory");

   // note: error exits here don't need to clean up a->out individually,
   // stbi__do_png always does on error.
   if (!stbi__mad3sizes_valid(img_n, x, depth, 7)) return stbi__err("too large", "Corrupt PNG");
   img_width_bytes = (((img_n * x * depth) + 7) >> 3);
   if (!stbi__mad2sizes_valid(img_width_bytes, y, img_width_bytes)) return stbi__err("too large", "Corrupt PNG");
   img_len = (img_width_bytes + 1) * y;

   // we used to check for exact match between raw_len and img_len on non-interlaced PNGs,
   // but issue #276 reported a PNG in the wild that had extra data at the end (all zeros),
   // so just check for raw_len < img_len always.
   if (raw_len < img_len) return stbi__err("not enough pixels","Corrupt PNG");

   // Allocate two scan lines worth of filter workspace buffer.
   filter_buf = (stbi_uc *) stbi__malloc_mad2(img_width_bytes, 2, 0);
   if (!filter_buf) return stbi__err("outofmem", "Out of memory");

   // Filtering for low-bit-depth images
   if (depth < 8) {
      filter_bytes = 1;
      width = img_width_bytes;
   }

   for (j=0; j < y; ++j) {
      // cur/prior filter buffers alternate
      stbi_uc *cur = filter_buf + (j & 1)*img_width_bytes;
      stbi_uc *prior = filter_buf + (~j & 1)*img_width_bytes;
      stbi_uc *dest = a->out + stride*j;
      int nk = width * filter_bytes;
      int filter = *raw++;

      // check filter type
      if (filter > 4) {
         all_ok = stbi__err("invalid filter","Corrupt PNG");
         break;
      }

      // if first row, use special filter that doesn't sample previous row
      if (j == 0) filter = first_row_filter[filter];

      // perform actual filtering
      switch (filter) {
      case STBI__F_none:
         memcpy(cur, raw, nk);
         break;
      case STBI__F_sub:
         memcpy(cur, raw, filter_bytes);
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + cur[k-filter_bytes]);
         break;
      case STBI__F_up:
         for (k = 0; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + prior[k]);
         break;
      case STBI__F_avg:
         for (k = 0; k < filter_bytes; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + (prior[k]>>1));
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + ((prior[k] + cur[k-filter_bytes])>>1));
         break;
      case STBI__F_paeth:
         for (k = 0; k < filter_bytes; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + prior[k]); // prior[k] == stbi__paeth(0,prior[k],0)
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k-filter_bytes], prior[k], prior[k-filter_bytes]));
         break;
      case STBI__F_avg_first:
         memcpy(cur, raw, filter_bytes);
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + (cur[k-filter_bytes] >> 1));
         break;
      }

      raw += nk;

      // expand decoded bits in cur to dest, also adding an extra alpha channel if desired
      if (depth < 8) {
         stbi_uc scale = (color == 0) ? stbi__depth_scale_table[depth] : 1; // scale grayscale values to 0..255 range
         stbi_uc *in = cur;
         stbi_uc *out = dest;
         stbi_uc inb = 0;
         stbi__uint32 nsmp = x*img_n;

         // expand bits to bytes first
         if (depth == 4) {
            for (i=0; i < nsmp; ++i) {
               if ((i & 1) == 0) inb = *in++;
               *out++ = scale * (inb >> 4);
               inb <<= 4;
            }
         } else if (depth == 2) {
            for (i=0; i < nsmp; ++i) {
               if ((i & 3) == 0) inb = *in++;
               *out++ = scale * (inb >> 6);
               inb <<= 2;
            }
         } else {
            STBI_ASSERT(depth == 1);
            for (i=0; i < nsmp; ++i) {
               if ((i & 7) == 0) inb = *in++;
               *out++ = scale * (inb >> 7);
               inb <<= 1;
            }
         }

         // insert alpha=255 values if desired
         if (img_n != out_n)
            stbi__create_png_alpha_expand8(dest, dest, x, img_n);
      } else if (depth == 8) {
         if (img_n == out_n)
            memcpy(dest, cur, x*img_n);
         else
            stbi__create_png_alpha_expand8(dest, cur, x, img_n);
      } else if (depth == 16) {
         // convert the image data from big-endian to platform-native
         stbi__uint16 *dest16 = (stbi__uint16*)dest;
         stbi__uint32 nsmp = x*img_n;

         if (img_n == out_n) {
            for (i = 0; i < nsmp; ++i, ++dest16, cur += 2)
               *dest16 = (cur[0] << 8) | cur[1];
         } else {
            STBI_ASSERT(img_n+1 == out_n);
            if (img_n == 1) {
               for (i = 0; i < x; ++i, dest16 += 2, cur += 2) {
                  dest16[0] = (cur[0] << 8) | cur[1];
                  dest16[1] = 0xffff;
               }
            } else {
               STBI_ASSERT(img_n == 3);
               for (i = 0; i < x; ++i, dest16 += 4, cur += 6) {
                  dest16[0] = (cur[0] << 8) | cur[1];
                  dest16[1] = (cur[2] << 8) | cur[3];
                  dest16[2] = (cur[4] << 8) | cur[5];
                  dest16[3] = 0xffff;
               }
            }
         }
      }
   }

   STBI_FREE(filter_buf);
   if (!all_ok) return 0;

   return 1;
}

static int stbi__create_png_image(stbi__png *a, stbi_uc *image_data, stbi__uint32 image_data_len, int out_n, int depth, int color, int interlaced)
{
   int bytes = (depth == 16 ? 2 : 1);
   int out_bytes = out_n * bytes;
   stbi_uc *final;
   int p;
   if (!interlaced)
      return stbi__create_png_image_raw(a, image_data, image_data_len, out_n, a->s->img_x, a->s->img_y, depth, color);

   // de-interlacing
   final = (stbi_uc *) stbi__malloc_mad3(a->s->img_x, a->s->img_y, out_bytes, 0);
   if (!final) return stbi__err("outofmem", "Out of memory");
   for (p=0; p < 7; ++p) {
      int xorig[] = { 0,4,0,2,0,1,0 };
      int yorig[] = { 0,0,4,0,2,0,1 };
      int xspc[]  = { 8,8,4,4,2,2,1 };
      int yspc[]  = { 8,8,8,4,4,2,2 };
      int i,j,x,y;
      // pass1_x[4] = 0, pass1_x[5] = 1, pass1_x[12] = 1
      x = (a->s->img_x - xorig[p] + xspc[p]-1) / xspc[p];
      y = (a->s->img_y - yorig[p] + yspc[p]-1) / yspc[p];
      if (x && y) {
         stbi__uint32 img_len = ((((a->s->img_n * x * depth) + 7) >> 3) + 1) * y;
         if (!stbi__create_png_image_raw(a, image_data, image_data_len, out_n, x, y, depth, color)) {
            STBI_FREE(final);
            return 0;
         }
         for (j=0; j < y; ++j) {
            for (i=0; i < x; ++i) {
               int out_y = j*yspc[p]+yorig[p];
               int out_x = i*xspc[p]+xorig[p];
               memcpy(final + out_y*a->s->img_x*out_bytes + out_x*out_bytes,
                      a->out + (j*x+i)*out_bytes, out_bytes);
            }
         }
         STBI_FREE(a->out);
         image_data += img_len;
         image_data_len -= img_len;
      }
   }
   a->out = final;

   return 1;
}

static int stbi__compute_transparency(stbi__png *z, stbi_uc tc[3], int out_n)
{
   stbi__context *s = z->s;
   stbi__uint32 i, pixel_count = s->img_x * s->img_y;
   stbi_uc *p = z->out;

   // compute color-based transparency, assuming we've
   // already got 255 as the alpha value in the output
   STBI_ASSERT(out_n == 2 || out_n == 4);

   if (out_n == 2) {
      for (i=0; i < pixel_count; ++i) {
         p[1] = (p[0] == tc[0] ? 0 : 255);
         p += 2;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
            p[3] = 0;
         p += 4;
      }
   }
   return 1;
}

static int stbi__compute_transparency16(stbi__png *z, stbi__uint16 tc[3], int out_n)
{
   stbi__context *s = z->s;
   stbi__uint32 i, pixel_count = s->img_x * s->img_y;
   stbi__uint16 *p = (stbi__uint16*) z->out;

   // compute color-based transparency, assuming we've
   // already got 65535 as the alpha value in the output
   STBI_ASSERT(out_n == 2 || out_n == 4);

   if (out_n == 2) {
      for (i = 0; i < pixel_count; ++i) {
         p[1] = (p[0] == tc[0] ? 0 : 65535);
         p += 2;
      }
   } else {
      for (i = 0; i < pixel_count; ++i) {
         if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
            p[3] = 0;
         p += 4;
      }
   }
   return 1;
}

static int stbi__expand_png_palette(stbi__png *a, stbi_uc *palette, int len, int pal_img_n)
{
   stbi__uint32 i, pixel_count = a->s->img_x * a->s->img_y;
   stbi_uc *p, *temp_out, *orig = a->out;

   p = (stbi_uc *) stbi__malloc_mad2(pixel_count, pal_img_n, 0);
   if (p == NULL) return stbi__err("outofmem", "Out of memory");

   // between here and free(out) below, exitting would leak
   temp_out = p;

   if (pal_img_n == 3) {
      for (i=0; i < pixel_count; ++i) {
         int n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p += 3;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         int n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p[3] = palette[n+3];
         p += 4;
      }
   }
   STBI_FREE(a->out);
   a->out = temp_out;

   STBI_NOTUSED(len);

   return 1;
}

static int stbi__unpremultiply_on_load_global = 0;
static int stbi__de_iphone_flag_global = 0;

STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply)
{
   stbi__unpremultiply_on_load_global = flag_true_if_should_unpremultiply;
}

STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert)
{
   stbi__de_iphone_flag_global = flag_true_if_should_convert;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__unpremultiply_on_load  stbi__unpremultiply_on_load_global
#define stbi__de_iphone_flag  stbi__de_iphone_flag_global
#else
static STBI_THREAD_LOCAL int stbi__unpremultiply_on_load_local, stbi__unpremultiply_on_load_set;
static STBI_THREAD_LOCAL int stbi__de_iphone_flag_local, stbi__de_iphone_flag_set;

STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply)
{
   stbi__unpremultiply_on_load_local = flag_true_if_should_unpremultiply;
   stbi__unpremultiply_on_load_set = 1;
}

STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert)
{
   stbi__de_iphone_flag_local = flag_true_if_should_convert;
   stbi__de_iphone_flag_set = 1;
}

#define stbi__unpremultiply_on_load  (stbi__unpremultiply_on_load_set           \
                                       ? stbi__unpremultiply_on_load_local      \
                                       : stbi__unpremultiply_on_load_global)
#define stbi__de_iphone_flag  (stbi__de_iphone_flag_set                         \
                                ? stbi__de_iphone_flag_local                    \
                                : stbi__de_iphone_flag_global)
#endif // STBI_THREAD_LOCAL

static void stbi__de_iphone(stbi__png *z)
{
   stbi__context *s = z->s;
   stbi__uint32 i, pixel_count = s->img_x * s->img_y;
   stbi_uc *p = z->out;

   if (s->img_out_n == 3) {  // convert bgr to rgb
      for (i=0; i < pixel_count; ++i) {
         stbi_uc t = p[0];
         p[0] = p[2];
         p[2] = t;
         p += 3;
      }
   } else {
      STBI_ASSERT(s->img_out_n == 4);
      if (stbi__unpremultiply_on_load) {
         // convert bgr to rgb and unpremultiply
         for (i=0; i < pixel_count; ++i) {
            stbi_uc a = p[3];
            stbi_uc t = p[0];
            if (a) {
               stbi_uc half = a / 2;
               p[0] = (p[2] * 255 + half) / a;
               p[1] = (p[1] * 255 + half) / a;
               p[2] = ( t   * 255 + half) / a;
            } else {
               p[0] = p[2];
               p[2] = t;
            }
            p += 4;
         }
      } else {
         // convert bgr to rgb
         for (i=0; i < pixel_count; ++i) {
            stbi_uc t = p[0];
            p[0] = p[2];
            p[2] = t;
            p += 4;
         }
      }
   }
}

#define STBI__PNG_TYPE(a,b,c,d)  (((unsigned) (a) << 24) + ((unsigned) (b) << 16) + ((unsigned) (c) << 8) + (unsigned) (d))

static int stbi__parse_png_file(stbi__png *z, int scan, int req_comp)
{
   stbi_uc palette[1024], pal_img_n=0;
   stbi_uc has_trans=0, tc[3]={0};
   stbi__uint16 tc16[3];
   stbi__uint32 ioff=0, idata_limit=0, i, pal_len=0;
   int first=1,k,interlace=0, color=0, is_iphone=0;
   stbi__context *s = z->s;

   z->expanded = NULL;
   z->idata = NULL;
   z->out = NULL;

   if (!stbi__check_png_header(s)) return 0;

   if (scan == STBI__SCAN_type) return 1;

   for (;;) {
      stbi__pngchunk c = stbi__get_chunk_header(s);
      switch (c.type) {
         case STBI__PNG_TYPE('C','g','B','I'):
            is_iphone = 1;
            stbi__skip(s, c.length);
            break;
         case STBI__PNG_TYPE('I','H','D','R'): {
            int comp,filter;
            if (!first) return stbi__err("multiple IHDR","Corrupt PNG");
            first = 0;
            if (c.length != 13) return stbi__err("bad IHDR len","Corrupt PNG");
            s->img_x = stbi__get32be(s);
            s->img_y = stbi__get32be(s);
            if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
            if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
            z->depth = stbi__get8(s);  if (z->depth != 1 && z->depth != 2 && z->depth != 4 && z->depth != 8 && z->depth != 16)  return stbi__err("1/2/4/8/16-bit only","PNG not supported: 1/2/4/8/16-bit only");
            color = stbi__get8(s);  if (color > 6)         return stbi__err("bad ctype","Corrupt PNG");
            if (color == 3 && z->depth == 16)                  return stbi__err("bad ctype","Corrupt PNG");
            if (color == 3) pal_img_n = 3; else if (color & 1) return stbi__err("bad ctype","Corrupt PNG");
            comp  = stbi__get8(s);  if (comp) return stbi__err("bad comp method","Corrupt PNG");
            filter= stbi__get8(s);  if (filter) return stbi__err("bad filter method","Corrupt PNG");
            interlace = stbi__get8(s); if (interlace>1) return stbi__err("bad interlace method","Corrupt PNG");
            if (!s->img_x || !s->img_y) return stbi__err("0-pixel image","Corrupt PNG");
            if (!pal_img_n) {
               s->img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
               if ((1 << 30) / s->img_x / s->img_n < s->img_y) return stbi__err("too large", "Image too large to decode");
            } else {
               // if paletted, then pal_n is our final components, and
               // img_n is # components to decompress/filter.
               s->img_n = 1;
               if ((1 << 30) / s->img_x / 4 < s->img_y) return stbi__err("too large","Corrupt PNG");
            }
            // even with SCAN_header, have to scan to see if we have a tRNS
            break;
         }

         case STBI__PNG_TYPE('P','L','T','E'):  {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (c.length > 256*3) return stbi__err("invalid PLTE","Corrupt PNG");
            pal_len = c.length / 3;
            if (pal_len * 3 != c.length) return stbi__err("invalid PLTE","Corrupt PNG");
            for (i=0; i < pal_len; ++i) {
               palette[i*4+0] = stbi__get8(s);
               palette[i*4+1] = stbi__get8(s);
               palette[i*4+2] = stbi__get8(s);
               palette[i*4+3] = 255;
            }
            break;
         }

         case STBI__PNG_TYPE('t','R','N','S'): {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (z->idata) return stbi__err("tRNS after IDAT","Corrupt PNG");
            if (pal_img_n) {
               if (scan == STBI__SCAN_header) { s->img_n = 4; return 1; }
               if (pal_len == 0) return stbi__err("tRNS before PLTE","Corrupt PNG");
               if (c.length > pal_len) return stbi__err("bad tRNS len","Corrupt PNG");
               pal_img_n = 4;
               for (i=0; i < c.length; ++i)
                  palette[i*4+3] = stbi__get8(s);
            } else {
               if (!(s->img_n & 1)) return stbi__err("tRNS with alpha","Corrupt PNG");
               if (c.length != (stbi__uint32) s->img_n*2) return stbi__err("bad tRNS len","Corrupt PNG");
               has_trans = 1;
               // non-paletted with tRNS = constant alpha. if header-scanning, we can stop now.
               if (scan == STBI__SCAN_header) { ++s->img_n; return 1; }
               if (z->depth == 16) {
                  for (k = 0; k < s->img_n; ++k) tc16[k] = (stbi__uint16)stbi__get16be(s); // copy the values as-is
               } else {
                  for (k = 0; k < s->img_n; ++k) tc[k] = (stbi_uc)(stbi__get16be(s) & 255) * stbi__depth_scale_table[z->depth]; // non 8-bit images will be larger
               }
            }
            break;
         }

         case STBI__PNG_TYPE('I','D','A','T'): {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (pal_img_n && !pal_len) return stbi__err("no PLTE","Corrupt PNG");
            if (scan == STBI__SCAN_header) {
               // header scan definitely stops at first IDAT
               if (pal_img_n)
                  s->img_n = pal_img_n;
               return 1;
            }
            if (c.length > (1u << 30)) return stbi__err("IDAT size limit", "IDAT section larger than 2^30 bytes");
            if ((int)(ioff + c.length) < (int)ioff) return 0;
            if (ioff + c.length > idata_limit) {
               stbi__uint32 idata_limit_old = idata_limit;
               stbi_uc *p;
               if (idata_limit == 0) idata_limit = c.length > 4096 ? c.length : 4096;
               while (ioff + c.length > idata_limit)
                  idata_limit *= 2;
               STBI_NOTUSED(idata_limit_old);
               p = (stbi_uc *) STBI_REALLOC_SIZED(z->idata, idata_limit_old, idata_limit); if (p == NULL) return stbi__err("outofmem", "Out of memory");
               z->idata = p;
            }
            if (!stbi__getn(s, z->idata+ioff,c.length)) return stbi__err("outofdata","Corrupt PNG");
            ioff += c.length;
            break;
         }

         case STBI__PNG_TYPE('I','E','N','D'): {
            stbi__uint32 raw_len, bpl;
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (scan != STBI__SCAN_load) return 1;
            if (z->idata == NULL) return stbi__err("no IDAT","Corrupt PNG");
            // initial guess for decoded data size to avoid unnecessary reallocs
            bpl = (s->img_x * z->depth + 7) / 8; // bytes per line, per component
            raw_len = bpl * s->img_y * s->img_n /* pixels */ + s->img_y /* filter mode per row */;
            z->expanded = (stbi_uc *) stbi_zlib_decode_malloc_guesssize_headerflag((char *) z->idata, ioff, raw_len, (int *) &raw_len, !is_iphone);
            if (z->expanded == NULL) return 0; // zlib should set error
            STBI_FREE(z->idata); z->idata = NULL;
            if ((req_comp == s->img_n+1 && req_comp != 3 && !pal_img_n) || has_trans)
               s->img_out_n = s->img_n+1;
            else
               s->img_out_n = s->img_n;
            if (!stbi__create_png_image(z, z->expanded, raw_len, s->img_out_n, z->depth, color, interlace)) return 0;
            if (has_trans) {
               if (z->depth == 16) {
                  if (!stbi__compute_transparency16(z, tc16, s->img_out_n)) return 0;
               } else {
                  if (!stbi__compute_transparency(z, tc, s->img_out_n)) return 0;
               }
            }
            if (is_iphone && stbi__de_iphone_flag && s->img_out_n > 2)
               stbi__de_iphone(z);
            if (pal_img_n) {
               // pal_img_n == 3 or 4
               s->img_n = pal_img_n; // record the actual colors we had
               s->img_out_n = pal_img_n;
               if (req_comp >= 3) s->img_out_n = req_comp;
               if (!stbi__expand_png_palette(z, palette, pal_len, s->img_out_n))
                  return 0;
            } else if (has_trans) {
               // non-paletted image with tRNS -> source image has (constant) alpha
               ++s->img_n;
            }
            STBI_FREE(z->expanded); z->expanded = NULL;
            // end of PNG chunk, read and skip CRC
            stbi__get32be(s);
            return 1;
         }

         default:
            // if critical, fail
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if ((c.type & (1 << 29)) == 0) {
               #ifndef STBI_NO_FAILURE_STRINGS
               // not threadsafe
               static char invalid_chunk[] = "XXXX PNG chunk not known";
               invalid_chunk[0] = STBI__BYTECAST(c.type >> 24);
               invalid_chunk[1] = STBI__BYTECAST(c.type >> 16);
               invalid_chunk[2] = STBI__BYTECAST(c.type >>  8);
               invalid_chunk[3] = STBI__BYTECAST(c.type >>  0);
               #endif
               return stbi__err(invalid_chunk, "PNG not supported: unknown PNG chunk type");
            }
            stbi__skip(s, c.length);
            break;
      }
      // end of PNG chunk, read and skip CRC
      stbi__get32be(s);
   }
}

static void *stbi__do_png(stbi__png *p, int *x, int *y, int *n, int req_comp, stbi__result_info *ri)
{
   void *result=NULL;
   if (req_comp < 0 || req_comp > 4) return stbi__errpuc("bad req_comp", "Internal error");
   if (stbi__parse_png_file(p, STBI__SCAN_load, req_comp)) {
      if (p->depth <= 8)
         ri->bits_per_channel = 8;
      else if (p->depth == 16)
         ri->bits_per_channel = 16;
      else
         return stbi__errpuc("bad bits_per_channel", "PNG not supported: unsupported color depth");
      result = p->out;
      p->out = NULL;
      if (req_comp && req_comp != p->s->img_out_n) {
         if (ri->bits_per_channel == 8)
            result = stbi__convert_format((unsigned char *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
         else
            result = stbi__convert_format16((stbi__uint16 *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
         p->s->img_out_n = req_comp;
         if (result == NULL) return result;
      }
      *x = p->s->img_x;
      *y = p->s->img_y;
      if (n) *n = p->s->img_n;
   }
   STBI_FREE(p->out);      p->out      = NULL;
   STBI_FREE(p->expanded); p->expanded = NULL;
   STBI_FREE(p->idata);    p->idata    = NULL;

   return result;
}

static void *stbi__png_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi__png p;
   p.s = s;
   return stbi__do_png(&p, x,y,comp,req_comp, ri);
}

static int stbi__png_test(stbi__context *s)
{
   int r;
   r = stbi__check_png_header(s);
   stbi__rewind(s);
   return r;
}

static int stbi__png_info_raw(stbi__png *p, int *x, int *y, int *comp)
{
   if (!stbi__parse_png_file(p, STBI__SCAN_header, 0)) {
      stbi__rewind( p->s );
      return 0;
   }
   if (x) *x = p->s->img_x;
   if (y) *y = p->s->img_y;
   if (comp) *comp = p->s->img_n;
   return 1;
}

static int stbi__png_info(stbi__context *s, int *x, int *y, int *comp)
{
   stbi__png p;
   p.s = s;
   return stbi__png_info_raw(&p, x, y, comp);
}

static int stbi__png_is16(stbi__context *s)
{
   stbi__png p;
   p.s = s;
   if (!stbi__png_info_raw(&p, NULL, NULL, NULL))
	   return 0;
   if (p.depth != 16) {
      stbi__rewind(p.s);
      return 0;
   }
   return 1;
}
#endif

// Microsoft/Windows BMP image

#ifndef STBI_NO_BMP
static int stbi__bmp_test_raw(stbi__context *s)
{
   int r;
   int sz;
   if (stbi__get8(s) != 'B') return 0;
   if (stbi__get8(s) != 'M') return 0;
   stbi__get32le(s); // discard filesize
   stbi__get16le(s); // discard reserved
   stbi__get16le(s); // discard reserved
   stbi__get32le(s); // discard data offset
   sz = stbi__get32le(s);
   r = (sz == 12 || sz == 40 || sz == 56 || sz == 108 || sz == 124);
   return r;
}

static int stbi__bmp_test(stbi__context *s)
{
   int r = stbi__bmp_test_raw(s);
   stbi__rewind(s);
   return r;
}


// returns 0..31 for the highest set bit
static int stbi__high_bit(unsigned int z)
{
   int n=0;
   if (z == 0) return -1;
   if (z >= 0x10000) { n += 16; z >>= 16; }
   if (z >= 0x00100) { n +=  8; z >>=  8; }
   if (z >= 0x00010) { n +=  4; z >>=  4; }
   if (z >= 0x00004) { n +=  2; z >>=  2; }
   if (z >= 0x00002) { n +=  1;/* >>=  1;*/ }
   return n;
}

static int stbi__bitcount(unsigned int a)
{
   a = (a & 0x55555555) + ((a >>  1) & 0x55555555); // max 2
   a = (a & 0x33333333) + ((a >>  2) & 0x33333333); // max 4
   a = (a + (a >> 4)) & 0x0f0f0f0f; // max 8 per 4, now 8 bits
   a = (a + (a >> 8)); // max 16 per 8 bits
   a = (a + (a >> 16)); // max 32 per 8 bits
   return a & 0xff;
}

// extract an arbitrarily-aligned N-bit value (N=bits)
// from v, and then make it 8-bits long and fractionally
// extend it to full full range.
static int stbi__shiftsigned(unsigned int v, int shift, int bits)
{
   static unsigned int mul_table[9] = {
      0,
      0xff/*0b11111111*/, 0x55/*0b01010101*/, 0x49/*0b01001001*/, 0x11/*0b00010001*/,
      0x21/*0b00100001*/, 0x41/*0b01000001*/, 0x81/*0b10000001*/, 0x01/*0b00000001*/,
   };
   static unsigned int shift_table[9] = {
      0, 0,0,1,0,2,4,6,0,
   };
   if (shift < 0)
      v <<= -shift;
   else
      v >>= shift;
   STBI_ASSERT(v < 256);
   v >>= (8-bits);
   STBI_ASSERT(bits >= 0 && bits <= 8);
   return (int) ((unsigned) v * mul_table[bits]) >> shift_table[bits];
}

typedef struct
{
   int bpp, offset, hsz;
   unsigned int mr,mg,mb,ma, all_a;
   int extra_read;
} stbi__bmp_data;

static int stbi__bmp_set_mask_defaults(stbi__bmp_data *info, int compress)
{
   // BI_BITFIELDS specifies masks explicitly, don't override
   if (compress == 3)
      return 1;

   if (compress == 0) {
      if (info->bpp == 16) {
         info->mr = 31u << 10;
         info->mg = 31u <<  5;
         info->mb = 31u <<  0;
      } else if (info->bpp == 32) {
         info->mr = 0xffu << 16;
         info->mg = 0xffu <<  8;
         info->mb = 0xffu <<  0;
         info->ma = 0xffu << 24;
         info->all_a = 0; // if all_a is 0 at end, then we loaded alpha channel but it was all 0
      } else {
         // otherwise, use defaults, which is all-0
         info->mr = info->mg = info->mb = info->ma = 0;
      }
      return 1;
   }
   return 0; // error
}

static void *stbi__bmp_parse_header(stbi__context *s, stbi__bmp_data *info)
{
   int hsz;
   if (stbi__get8(s) != 'B' || stbi__get8(s) != 'M') return stbi__errpuc("not BMP", "Corrupt BMP");
   stbi__get32le(s); // discard filesize
   stbi__get16le(s); // discard reserved
   stbi__get16le(s); // discard reserved
   info->offset = stbi__get32le(s);
   info->hsz = hsz = stbi__get32le(s);
   info->mr = info->mg = info->mb = info->ma = 0;
   info->extra_read = 14;

   if (info->offset < 0) return stbi__errpuc("bad BMP", "bad BMP");

   if (hsz != 12 && hsz != 40 && hsz != 56 && hsz != 108 && hsz != 124) return stbi__errpuc("unknown BMP", "BMP type not supported: unknown");
   if (hsz == 12) {
      s->img_x = stbi__get16le(s);
      s->img_y = stbi__get16le(s);
   } else {
      s->img_x = stbi__get32le(s);
      s->img_y = stbi__get32le(s);
   }
   if (stbi__get16le(s) != 1) return stbi__errpuc("bad BMP", "bad BMP");
   info->bpp = stbi__get16le(s);
   if (hsz != 12) {
      int compress = stbi__get32le(s);
      if (compress == 1 || compress == 2) return stbi__errpuc("BMP RLE", "BMP type not supported: RLE");
      if (compress >= 4) return stbi__errpuc("BMP JPEG/PNG", "BMP type not supported: unsupported compression"); // this includes PNG/JPEG modes
      if (compress == 3 && info->bpp != 16 && info->bpp != 32) return stbi__errpuc("bad BMP", "bad BMP"); // bitfields requires 16 or 32 bits/pixel
      stbi__get32le(s); // discard sizeof
      stbi__get32le(s); // discard hres
      stbi__get32le(s); // discard vres
      stbi__get32le(s); // discard colorsused
      stbi__get32le(s); // discard max important
      if (hsz == 40 || hsz == 56) {
         if (hsz == 56) {
            stbi__get32le(s);
            stbi__get32le(s);
            stbi__get32le(s);
            stbi__get32le(s);
         }
         if (info->bpp == 16 || info->bpp == 32) {
            if (compress == 0) {
               stbi__bmp_set_mask_defaults(info, compress);
            } else if (compress == 3) {
               info->mr = stbi__get32le(s);
               info->mg = stbi__get32le(s);
               info->mb = stbi__get32le(s);
               info->extra_read += 12;
               // not documented, but generated by photoshop and handled by mspaint
               if (info->mr == info->mg && info->mg == info->mb) {
                  // ?!?!?
                  return stbi__errpuc("bad BMP", "bad BMP");
               }
            } else
               return stbi__errpuc("bad BMP", "bad BMP");
         }
      } else {
         // V4/V5 header
         int i;
         if (hsz != 108 && hsz != 124)
            return stbi__errpuc("bad BMP", "bad BMP");
         info->mr = stbi__get32le(s);
         info->mg = stbi__get32le(s);
         info->mb = stbi__get32le(s);
         info->ma = stbi__get32le(s);
         if (compress != 3) // override mr/mg/mb unless in BI_BITFIELDS mode, as per docs
            stbi__bmp_set_mask_defaults(info, compress);
         stbi__get32le(s); // discard color space
         for (i=0; i < 12; ++i)
            stbi__get32le(s); // discard color space parameters
         if (hsz == 124) {
            stbi__get32le(s); // discard rendering intent
            stbi__get32le(s); // discard offset of profile data
            stbi__get32le(s); // discard size of profile data
            stbi__get32le(s); // discard reserved
         }
      }
   }
   return (void *) 1;
}


static void *stbi__bmp_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi_uc *out;
   unsigned int mr=0,mg=0,mb=0,ma=0, all_a;
   stbi_uc pal[256][4];
   int psize=0,i,j,width;
   int flip_vertically, pad, target;
   stbi__bmp_data info;
   STBI_NOTUSED(ri);

   info.all_a = 255;
   if (stbi__bmp_parse_header(s, &info) == NULL)
      return NULL; // error code already set

   flip_vertically = ((int) s->img_y) > 0;
   s->img_y = abs((int) s->img_y);

   if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   mr = info.mr;
   mg = info.mg;
   mb = info.mb;
   ma = info.ma;
   all_a = info.all_a;

   if (info.hsz == 12) {
      if (info.bpp < 24)
         psize = (info.offset - info.extra_read - 24) / 3;
   } else {
      if (info.bpp < 16)
         psize = (info.offset - info.extra_read - info.hsz) >> 2;
   }
   if (psize == 0) {
      // accept some number of extra bytes after the header, but if the offset points either to before
      // the header ends or implies a large amount of extra data, reject the file as malformed
      int bytes_read_so_far = s->callback_already_read + (int)(s->img_buffer - s->img_buffer_original);
      int header_limit = 1024; // max we actually read is below 256 bytes currently.
      int extra_data_limit = 256*4; // what ordinarily goes here is a palette; 256 entries*4 bytes is its max size.
      if (bytes_read_so_far <= 0 || bytes_read_so_far > header_limit) {
         return stbi__errpuc("bad header", "Corrupt BMP");
      }
      // we established that bytes_read_so_far is positive and sensible.
      // the first half of this test rejects offsets that are either too small positives, or
      // negative, and guarantees that info.offset >= bytes_read_so_far > 0. this in turn
      // ensures the number computed in the second half of the test can't overflow.
      if (info.offset < bytes_read_so_far || info.offset - bytes_read_so_far > extra_data_limit) {
         return stbi__errpuc("bad offset", "Corrupt BMP");
      } else {
         stbi__skip(s, info.offset - bytes_read_so_far);
      }
   }

   if (info.bpp == 24 && ma == 0xff000000)
      s->img_n = 3;
   else
      s->img_n = ma ? 4 : 3;
   if (req_comp && req_comp >= 3) // we can directly decode 3 or 4
      target = req_comp;
   else
      target = s->img_n; // if they want monochrome, we'll post-convert

   // sanity-check size
   if (!stbi__mad3sizes_valid(target, s->img_x, s->img_y, 0))
      return stbi__errpuc("too large", "Corrupt BMP");

   out = (stbi_uc *) stbi__malloc_mad3(target, s->img_x, s->img_y, 0);
   if (!out) return stbi__errpuc("outofmem", "Out of memory");
   if (info.bpp < 16) {
      int z=0;
      if (psize == 0 || psize > 256) { STBI_FREE(out); return stbi__errpuc("invalid", "Corrupt BMP"); }
      for (i=0; i < psize; ++i) {
         pal[i][2] = stbi__get8(s);
         pal[i][1] = stbi__get8(s);
         pal[i][0] = stbi__get8(s);
         if (info.hsz != 12) stbi__get8(s);
         pal[i][3] = 255;
      }
      stbi__skip(s, info.offset - info.extra_read - info.hsz - psize * (info.hsz == 12 ? 3 : 4));
      if (info.bpp == 1) width = (s->img_x + 7) >> 3;
      else if (info.bpp == 4) width = (s->img_x + 1) >> 1;
      else if (info.bpp == 8) width = s->img_x;
      else { STBI_FREE(out); return stbi__errpuc("bad bpp", "Corrupt BMP"); }
      pad = (-width)&3;
      if (info.bpp == 1) {
         for (j=0; j < (int) s->img_y; ++j) {
            int bit_offset = 7, v = stbi__get8(s);
            for (i=0; i < (int) s->img_x; ++i) {
               int color = (v>>bit_offset)&0x1;
               out[z++] = pal[color][0];
               out[z++] = pal[color][1];
               out[z++] = pal[color][2];
               if (target == 4) out[z++] = 255;
               if (i+1 == (int) s->img_x) break;
               if((--bit_offset) < 0) {
                  bit_offset = 7;
                  v = stbi__get8(s);
               }
            }
            stbi__skip(s, pad);
         }
      } else {
         for (j=0; j < (int) s->img_y; ++j) {
            for (i=0; i < (int) s->img_x; i += 2) {
               int v=stbi__get8(s),v2=0;
               if (info.bpp == 4) {
                  v2 = v & 15;
                  v >>= 4;
               }
               out[z++] = pal[v][0];
               out[z++] = pal[v][1];
               out[z++] = pal[v][2];
               if (target == 4) out[z++] = 255;
               if (i+1 == (int) s->img_x) break;
               v = (info.bpp == 8) ? stbi__get8(s) : v2;
               out[z++] = pal[v][0];
               out[z++] = pal[v][1];
               out[z++] = pal[v][2];
               if (target == 4) out[z++] = 255;
            }
            stbi__skip(s, pad);
         }
      }
   } else {
      int rshift=0,gshift=0,bshift=0,ashift=0,rcount=0,gcount=0,bcount=0,acount=0;
      int z = 0;
      int easy=0;
      stbi__skip(s, info.offset - info.extra_read - info.hsz);
      if (info.bpp == 24) width = 3 * s->img_x;
      else if (info.bpp == 16) width = 2*s->img_x;
      else /* bpp = 32 and pad = 0 */ width=0;
      pad = (-width) & 3;
      if (info.bpp == 24) {
         easy = 1;
      } else if (info.bpp == 32) {
         if (mb == 0xff && mg == 0xff00 && mr == 0x00ff0000 && ma == 0xff000000)
            easy = 2;
      }
      if (!easy) {
         if (!mr || !mg || !mb) { STBI_FREE(out); return stbi__errpuc("bad masks", "Corrupt BMP"); }
         // right shift amt to put high bit in position #7
         rshift = stbi__high_bit(mr)-7; rcount = stbi__bitcount(mr);
         gshift = stbi__high_bit(mg)-7; gcount = stbi__bitcount(mg);
         bshift = stbi__high_bit(mb)-7; bcount = stbi__bitcount(mb);
         ashift = stbi__high_bit(ma)-7; acount = stbi__bitcount(ma);
         if (rcount > 8 || gcount > 8 || bcount > 8 || acount > 8) { STBI_FREE(out); return stbi__errpuc("bad masks", "Corrupt BMP"); }
      }
      for (j=0; j < (int) s->img_y; ++j) {
         if (easy) {
            for (i=0; i < (int) s->img_x; ++i) {
               unsigned char a;
               out[z+2] = stbi__get8(s);
               out[z+1] = stbi__get8(s);
               out[z+0] = stbi__get8(s);
               z += 3;
               a = (easy == 2 ? stbi__get8(s) : 255);
               all_a |= a;
               if (target == 4) out[z++] = a;
            }
         } else {
            int bpp = info.bpp;
            for (i=0; i < (int) s->img_x; ++i) {
               stbi__uint32 v = (bpp == 16 ? (stbi__uint32) stbi__get16le(s) : stbi__get32le(s));
               unsigned int a;
               out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mr, rshift, rcount));
               out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mg, gshift, gcount));
               out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mb, bshift, bcount));
               a = (ma ? stbi__shiftsigned(v & ma, ashift, acount) : 255);
               all_a |= a;
               if (target == 4) out[z++] = STBI__BYTECAST(a);
            }
         }
         stbi__skip(s, pad);
      }
   }

   // if alpha channel is all 0s, replace with all 255s
   if (target == 4 && all_a == 0)
      for (i=4*s->img_x*s->img_y-1; i >= 0; i -= 4)
         out[i] = 255;

   if (flip_vertically) {
      stbi_uc t;
      for (j=0; j < (int) s->img_y>>1; ++j) {
         stbi_uc *p1 = out +      j     *s->img_x*target;
         stbi_uc *p2 = out + (s->img_y-1-j)*s->img_x*target;
         for (i=0; i < (int) s->img_x*target; ++i) {
            t = p1[i]; p1[i] = p2[i]; p2[i] = t;
         }
      }
   }

   if (req_comp && req_comp != target) {
      out = stbi__convert_format(out, target, req_comp, s->img_x, s->img_y);
      if (out == NULL) return out; // stbi__convert_format frees input on failure
   }

   *x = s->img_x;
   *y = s->img_y;
   if (comp) *comp = s->img_n;
   return out;
}
#endif

// Targa Truevision - TGA
// by Jonathan Dummer
#ifndef STBI_NO_TGA
// returns STBI_rgb or whatever, 0 on error
static int stbi__tga_get_comp(int bits_per_pixel, int is_grey, int* is_rgb16)
{
   // only RGB or RGBA (incl. 16bit) or grey allowed
   if (is_rgb16) *is_rgb16 = 0;
   switch(bits_per_pixel) {
      case 8:  return STBI_grey;
      case 16: if(is_grey) return STBI_grey_alpha;
               // fallthrough
      case 15: if(is_rgb16) *is_rgb16 = 1;
               return STBI_rgb;
      case 24: // fallthrough
      case 32: return bits_per_pixel/8;
      default: return 0;
   }
}

static int stbi__tga_info(stbi__context *s, int *x, int *y, int *comp)
{
    int tga_w, tga_h, tga_comp, tga_image_type, tga_bits_per_pixel, tga_colormap_bpp;
    int sz, tga_colormap_type;
    stbi__get8(s);                   // discard Offset
    tga_colormap_type = stbi__get8(s); // colormap type
    if( tga_colormap_type > 1 ) {
        stbi__rewind(s);
        return 0;      // only RGB or indexed allowed
    }
    tga_image_type = stbi__get8(s); // image type
    if ( tga_colormap_type == 1 ) { // colormapped (paletted) image
        if (tga_image_type != 1 && tga_image_type != 9) {
            stbi__rewind(s);
            return 0;
        }
        stbi__skip(s,4);       // skip index of first colormap entry and number of entries
        sz = stbi__get8(s);    //   check bits per palette color entry
        if ( (sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32) ) {
            stbi__rewind(s);
            return 0;
        }
        stbi__skip(s,4);       // skip image x and y origin
        tga_colormap_bpp = sz;
    } else { // "normal" image w/o colormap - only RGB or grey allowed, +/- RLE
        if ( (tga_image_type != 2) && (tga_image_type != 3) && (tga_image_type != 10) && (tga_image_type != 11) ) {
            stbi__rewind(s);
            return 0; // only RGB or grey allowed, +/- RLE
        }
        stbi__skip(s,9); // skip colormap specification and image x/y origin
        tga_colormap_bpp = 0;
    }
    tga_w = stbi__get16le(s);
    if( tga_w < 1 ) {
        stbi__rewind(s);
        return 0;   // test width
    }
    tga_h = stbi__get16le(s);
    if( tga_h < 1 ) {
        stbi__rewind(s);
        return 0;   // test height
    }
    tga_bits_per_pixel = stbi__get8(s); // bits per pixel
    stbi__get8(s); // ignore alpha bits
    if (tga_colormap_bpp != 0) {
        if((tga_bits_per_pixel != 8) && (tga_bits_per_pixel != 16)) {
            // when using a colormap, tga_bits_per_pixel is the size of the indexes
            // I don't think anything but 8 or 16bit indexes makes sense
            stbi__rewind(s);
            return 0;
        }
        tga_comp = stbi__tga_get_comp(tga_colormap_bpp, 0, NULL);
    } else {
        tga_comp = stbi__tga_get_comp(tga_bits_per_pixel, (tga_image_type == 3) || (tga_image_type == 11), NULL);
    }
    if(!tga_comp) {
      stbi__rewind(s);
      return 0;
    }
    if (x) *x = tga_w;
    if (y) *y = tga_h;
    if (comp) *comp = tga_comp;
    return 1;                   // seems to have passed everything
}

static int stbi__tga_test(stbi__context *s)
{
   int res = 0;
   int sz, tga_color_type;
   stbi__get8(s);      //   discard Offset
   tga_color_type = stbi__get8(s);   //   color type
   if ( tga_color_type > 1 ) goto errorEnd;   //   only RGB or indexed allowed
   sz = stbi__get8(s);   //   image type
   if ( tga_color_type == 1 ) { // colormapped (paletted) image
      if (sz != 1 && sz != 9) goto errorEnd; // colortype 1 demands image type 1 or 9
      stbi__skip(s,4);       // skip index of first colormap entry and number of entries
      sz = stbi__get8(s);    //   check bits per palette color entry
      if ( (sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32) ) goto errorEnd;
      stbi__skip(s,4);       // skip image x and y origin
   } else { // "normal" image w/o colormap
      if ( (sz != 2) && (sz != 3) && (sz != 10) && (sz != 11) ) goto errorEnd; // only RGB or grey allowed, +/- RLE
      stbi__skip(s,9); // skip colormap specification and image x/y origin
   }
   if ( stbi__get16le(s) < 1 ) goto errorEnd;      //   test width
   if ( stbi__get16le(s) < 1 ) goto errorEnd;      //   test height
   sz = stbi__get8(s);   //   bits per pixel
   if ( (tga_color_type == 1) && (sz != 8) && (sz != 16) ) goto errorEnd; // for colormapped images, bpp is size of an index
   if ( (sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32) ) goto errorEnd;

   res = 1; // if we got this far, everything's good and we can return 1 instead of 0

errorEnd:
   stbi__rewind(s);
   return res;
}

// read 16bit value and convert to 24bit RGB
static void stbi__tga_read_rgb16(stbi__context *s, stbi_uc* out)
{
   stbi__uint16 px = (stbi__uint16)stbi__get16le(s);
   stbi__uint16 fiveBitMask = 31;
   // we have 3 channels with 5bits each
   int r = (px >> 10) & fiveBitMask;
   int g = (px >> 5) & fiveBitMask;
   int b = px & fiveBitMask;
   // Note that this saves the data in RGB(A) order, so it doesn't need to be swapped later
   out[0] = (stbi_uc)((r * 255)/31);
   out[1] = (stbi_uc)((g * 255)/31);
   out[2] = (stbi_uc)((b * 255)/31);

   // some people claim that the most significant bit might be used for alpha
   // (possibly if an alpha-bit is set in the "image descriptor byte")
   // but that only made 16bit test images completely translucent..
   // so let's treat all 15 and 16bit TGAs as RGB with no alpha.
}

static void *stbi__tga_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   //   read in the TGA header stuff
   int tga_offset = stbi__get8(s);
   int tga_indexed = stbi__get8(s);
   int tga_image_type = stbi__get8(s);
   int tga_is_RLE = 0;
   int tga_palette_start = stbi__get16le(s);
   int tga_palette_len = stbi__get16le(s);
   int tga_palette_bits = stbi__get8(s);
   int tga_x_origin = stbi__get16le(s);
   int tga_y_origin = stbi__get16le(s);
   int tga_width = stbi__get16le(s);
   int tga_height = stbi__get16le(s);
   int tga_bits_per_pixel = stbi__get8(s);
   int tga_comp, tga_rgb16=0;
   int tga_inverted = stbi__get8(s);
   // int tga_alpha_bits = tga_inverted & 15; // the 4 lowest bits - unused (useless?)
   //   image data
   unsigned char *tga_data;
   unsigned char *tga_palette = NULL;
   int i, j;
   unsigned char raw_data[4] = {0};
   int RLE_count = 0;
   int RLE_repeating = 0;
   int read_next_pixel = 1;
   STBI_NOTUSED(ri);
   STBI_NOTUSED(tga_x_origin); // @TODO
   STBI_NOTUSED(tga_y_origin); // @TODO

   if (tga_height > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (tga_width > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   //   do a tiny bit of precessing
   if ( tga_image_type >= 8 )
   {
      tga_image_type -= 8;
      tga_is_RLE = 1;
   }
   tga_inverted = 1 - ((tga_inverted >> 5) & 1);

   //   If I'm paletted, then I'll use the number of bits from the palette
   if ( tga_indexed ) tga_comp = stbi__tga_get_comp(tga_palette_bits, 0, &tga_rgb16);
   else tga_comp = stbi__tga_get_comp(tga_bits_per_pixel, (tga_image_type == 3), &tga_rgb16);

   if(!tga_comp) // shouldn't really happen, stbi__tga_test() should have ensured basic consistency
      return stbi__errpuc("bad format", "Can't find out TGA pixelformat");

   //   tga info
   *x = tga_width;
   *y = tga_height;
   if (comp) *comp = tga_comp;

   if (!stbi__mad3sizes_valid(tga_width, tga_height, tga_comp, 0))
      return stbi__errpuc("too large", "Corrupt TGA");

   tga_data = (unsigned char*)stbi__malloc_mad3(tga_width, tga_height, tga_comp, 0);
   if (!tga_data) return stbi__errpuc("outofmem", "Out of memory");

   // skip to the data's starting position (offset usually = 0)
   stbi__skip(s, tga_offset );

   if ( !tga_indexed && !tga_is_RLE && !tga_rgb16 ) {
      for (i=0; i < tga_height; ++i) {
         int row = tga_inverted ? tga_height -i - 1 : i;
         stbi_uc *tga_row = tga_data + row*tga_width*tga_comp;
         stbi__getn(s, tga_row, tga_width * tga_comp);
      }
   } else  {
      //   do I need to load a palette?
      if ( tga_indexed)
      {
         if (tga_palette_len == 0) {  /* you have to have at least one entry! */
            STBI_FREE(tga_data);
            return stbi__errpuc("bad palette", "Corrupt TGA");
         }

         //   any data to skip? (offset usually = 0)
         stbi__skip(s, tga_palette_start );
         //   load the palette
         tga_palette = (unsigned char*)stbi__malloc_mad2(tga_palette_len, tga_comp, 0);
         if (!tga_palette) {
            STBI_FREE(tga_data);
            return stbi__errpuc("outofmem", "Out of memory");
         }
         if (tga_rgb16) {
            stbi_uc *pal_entry = tga_palette;
            STBI_ASSERT(tga_comp == STBI_rgb);
            for (i=0; i < tga_palette_len; ++i) {
               stbi__tga_read_rgb16(s, pal_entry);
               pal_entry += tga_comp;
            }
         } else if (!stbi__getn(s, tga_palette, tga_palette_len * tga_comp)) {
               STBI_FREE(tga_data);
               STBI_FREE(tga_palette);
               return stbi__errpuc("bad palette", "Corrupt TGA");
         }
      }
      //   load the data
      for (i=0; i < tga_width * tga_height; ++i)
      {
         //   if I'm in RLE mode, do I need to get a RLE stbi__pngchunk?
         if ( tga_is_RLE )
         {
            if ( RLE_count == 0 )
            {
               //   yep, get the next byte as a RLE command
               int RLE_cmd = stbi__get8(s);
               RLE_count = 1 + (RLE_cmd & 127);
               RLE_repeating = RLE_cmd >> 7;
               read_next_pixel = 1;
            } else if ( !RLE_repeating )
            {
               read_next_pixel = 1;
            }
         } else
         {
            read_next_pixel = 1;
         }
         //   OK, if I need to read a pixel, do it now
         if ( read_next_pixel )
         {
            //   load however much data we did have
            if ( tga_indexed )
            {
               // read in index, then perform the lookup
               int pal_idx = (tga_bits_per_pixel == 8) ? stbi__get8(s) : stbi__get16le(s);
               if ( pal_idx >= tga_palette_len ) {
                  // invalid index
                  pal_idx = 0;
               }
               pal_idx *= tga_comp;
               for (j = 0; j < tga_comp; ++j) {
                  raw_data[j] = tga_palette[pal_idx+j];
               }
            } else if(tga_rgb16) {
               STBI_ASSERT(tga_comp == STBI_rgb);
               stbi__tga_read_rgb16(s, raw_data);
            } else {
               //   read in the data raw
               for (j = 0; j < tga_comp; ++j) {
                  raw_data[j] = stbi__get8(s);
               }
            }
            //   clear the reading flag for the next pixel
            read_next_pixel = 0;
         } // end of reading a pixel

         // copy data
         for (j = 0; j < tga_comp; ++j)
           tga_data[i*tga_comp+j] = raw_data[j];

         //   in case we're in RLE mode, keep counting down
         --RLE_count;
      }
      //   do I need to invert the image?
      if ( tga_inverted )
      {
         for (j = 0; j*2 < tga_height; ++j)
         {
            int index1 = j * tga_width * tga_comp;
            int index2 = (tga_height - 1 - j) * tga_width * tga_comp;
            for (i = tga_width * tga_comp; i > 0; --i)
            {
               unsigned char temp = tga_data[index1];
               tga_data[index1] = tga_data[index2];
               tga_data[index2] = temp;
               ++index1;
               ++index2;
            }
         }
      }
      //   clear my palette, if I had one
      if ( tga_palette != NULL )
      {
         STBI_FREE( tga_palette );
      }
   }

   // swap RGB - if the source data was RGB16, it already is in the right order
   if (tga_comp >= 3 && !tga_rgb16)
   {
      unsigned char* tga_pixel = tga_data;
      for (i=0; i < tga_width * tga_height; ++i)
      {
         unsigned char temp = tga_pixel[0];
         tga_pixel[0] = tga_pixel[2];
         tga_pixel[2] = temp;
         tga_pixel += tga_comp;
      }
   }

   // convert to target component count
   if (req_comp && req_comp != tga_comp)
      tga_data = stbi__convert_format(tga_data, tga_comp, req_comp, tga_width, tga_height);

   //   the things I do to get rid of an error message, and yet keep
   //   Microsoft's C compilers happy... [8^(
   tga_palette_start = tga_palette_len = tga_palette_bits =
         tga_x_origin = tga_y_origin = 0;
   STBI_NOTUSED(tga_palette_start);
   //   OK, done
   return tga_data;
}
#endif

// *************************************************************************************************
// Photoshop PSD loader -- PD by Thatcher Ulrich, integration by Nicolas Schulz, tweaked by STB

#ifndef STBI_NO_PSD
static int stbi__psd_test(stbi__context *s)
{
   int r = (stbi__get32be(s) == 0x38425053);
   stbi__rewind(s);
   return r;
}

static int stbi__psd_decode_rle(stbi__context *s, stbi_uc *p, int pixelCount)
{
   int count, nleft, len;

   count = 0;
   while ((nleft = pixelCount - count) > 0) {
      len = stbi__get8(s);
      if (len == 128) {
         // No-op.
      } else if (len < 128) {
         // Copy next len+1 bytes literally.
         len++;
         if (len > nleft) return 0; // corrupt data
         count += len;
         while (len) {
            *p = stbi__get8(s);
            p += 4;
            len--;
         }
      } else if (len > 128) {
         stbi_uc   val;
         // Next -len+1 bytes in the dest are replicated from next source byte.
         // (Interpret len as a negative 8-bit int.)
         len = 257 - len;
         if (len > nleft) return 0; // corrupt data
         val = stbi__get8(s);
         count += len;
         while (len) {
            *p = val;
            p += 4;
            len--;
         }
      }
   }

   return 1;
}

static void *stbi__psd_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri, int bpc)
{
   int pixelCount;
   int channelCount, compression;
   int channel, i;
   int bitdepth;
   int w,h;
   stbi_uc *out;
   STBI_NOTUSED(ri);

   // Check identifier
   if (stbi__get32be(s) != 0x38425053)   // "8BPS"
      return stbi__errpuc("not PSD", "Corrupt PSD image");

   // Check file type version.
   if (stbi__get16be(s) != 1)
      return stbi__errpuc("wrong version", "Unsupported version of PSD image");

   // Skip 6 reserved bytes.
   stbi__skip(s, 6 );

   // Read the number of channels (R, G, B, A, etc).
   channelCount = stbi__get16be(s);
   if (channelCount < 0 || channelCount > 16)
      return stbi__errpuc("wrong channel count", "Unsupported number of channels in PSD image");

   // Read the rows and columns of the image.
   h = stbi__get32be(s);
   w = stbi__get32be(s);

   if (h > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (w > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   // Make sure the depth is 8 bits.
   bitdepth = stbi__get16be(s);
   if (bitdepth != 8 && bitdepth != 16)
      return stbi__errpuc("unsupported bit depth", "PSD bit depth is not 8 or 16 bit");

   // Make sure the color mode is RGB.
   // Valid options are:
   //   0: Bitmap
   //   1: Grayscale
   //   2: Indexed color
   //   3: RGB color
   //   4: CMYK color
   //   7: Multichannel
   //   8: Duotone
   //   9: Lab color
   if (stbi__get16be(s) != 3)
      return stbi__errpuc("wrong color format", "PSD is not in RGB color format");

   // Skip the Mode Data.  (It's the palette for indexed color; other info for other modes.)
   stbi__skip(s,stbi__get32be(s) );

   // Skip the image resources.  (resolution, pen tool paths, etc)
   stbi__skip(s, stbi__get32be(s) );

   // Skip the reserved data.
   stbi__skip(s, stbi__get32be(s) );

   // Find out if the data is compressed.
   // Known values:
   //   0: no compression
   //   1: RLE compressed
   compression = stbi__get16be(s);
   if (compression > 1)
      return stbi__errpuc("bad compression", "PSD has an unknown compression format");

   // Check size
   if (!stbi__mad3sizes_valid(4, w, h, 0))
      return stbi__errpuc("too large", "Corrupt PSD");

   // Create the destination image.

   if (!compression && bitdepth == 16 && bpc == 16) {
      out = (stbi_uc *) stbi__malloc_mad3(8, w, h, 0);
      ri->bits_per_channel = 16;
   } else
      out = (stbi_uc *) stbi__malloc(4 * w*h);

   if (!out) return stbi__errpuc("outofmem", "Out of memory");
   pixelCount = w*h;

   // Initialize the data to zero.
   //memset( out, 0, pixelCount * 4 );

   // Finally, the image data.
   if (compression) {
      // RLE as used by .PSD and .TIFF
      // Loop until you get the number of unpacked bytes you are expecting:
      //     Read the next source byte into n.
      //     If n is between 0 and 127 inclusive, copy the next n+1 bytes literally.
      //     Else if n is between -127 and -1 inclusive, copy the next byte -n+1 times.
      //     Else if n is 128, noop.
      // Endloop

      // The RLE-compressed data is preceded by a 2-byte data count for each row in the data,
      // which we're going to just skip.
      stbi__skip(s, h * channelCount * 2 );

      // Read the RLE data by channel.
      for (channel = 0; channel < 4; channel++) {
         stbi_uc *p;

         p = out+channel;
         if (channel >= channelCount) {
            // Fill this channel with default data.
            for (i = 0; i < pixelCount; i++, p += 4)
               *p = (channel == 3 ? 255 : 0);
         } else {
            // Read the RLE data.
            if (!stbi__psd_decode_rle(s, p, pixelCount)) {
               STBI_FREE(out);
               return stbi__errpuc("corrupt", "bad RLE data");
            }
         }
      }

   } else {
      // We're at the raw image data.  It's each channel in order (Red, Green, Blue, Alpha, ...)
      // where each channel consists of an 8-bit (or 16-bit) value for each pixel in the image.

      // Read the data by channel.
      for (channel = 0; channel < 4; channel++) {
         if (channel >= channelCount) {
            // Fill this channel with default data.
            if (bitdepth == 16 && bpc == 16) {
               stbi__uint16 *q = ((stbi__uint16 *) out) + channel;
               stbi__uint16 val = channel == 3 ? 65535 : 0;
               for (i = 0; i < pixelCount; i++, q += 4)
                  *q = val;
            } else {
               stbi_uc *p = out+channel;
               stbi_uc val = channel == 3 ? 255 : 0;
               for (i = 0; i < pixelCount; i++, p += 4)
                  *p = val;
            }
         } else {
            if (ri->bits_per_channel == 16) {    // output bpc
               stbi__uint16 *q = ((stbi__uint16 *) out) + channel;
               for (i = 0; i < pixelCount; i++, q += 4)
                  *q = (stbi__uint16) stbi__get16be(s);
            } else {
               stbi_uc *p = out+channel;
               if (bitdepth == 16) {  // input bpc
                  for (i = 0; i < pixelCount; i++, p += 4)
                     *p = (stbi_uc) (stbi__get16be(s) >> 8);
               } else {
                  for (i = 0; i < pixelCount; i++, p += 4)
                     *p = stbi__get8(s);
               }
            }
         }
      }
   }

   // remove weird white matte from PSD
   if (channelCount >= 4) {
      if (ri->bits_per_channel == 16) {
         for (i=0; i < w*h; ++i) {
            stbi__uint16 *pixel = (stbi__uint16 *) out + 4*i;
            if (pixel[3] != 0 && pixel[3] != 65535) {
               float a = pixel[3] / 65535.0f;
               float ra = 1.0f / a;
               float inv_a = 65535.0f * (1 - ra);
               pixel[0] = (stbi__uint16) (pixel[0]*ra + inv_a);
               pixel[1] = (stbi__uint16) (pixel[1]*ra + inv_a);
               pixel[2] = (stbi__uint16) (pixel[2]*ra + inv_a);
            }
         }
      } else {
         for (i=0; i < w*h; ++i) {
            unsigned char *pixel = out + 4*i;
            if (pixel[3] != 0 && pixel[3] != 255) {
               float a = pixel[3] / 255.0f;
               float ra = 1.0f / a;
               float inv_a = 255.0f * (1 - ra);
               pixel[0] = (unsigned char) (pixel[0]*ra + inv_a);
               pixel[1] = (unsigned char) (pixel[1]*ra + inv_a);
               pixel[2] = (unsigned char) (pixel[2]*ra + inv_a);
            }
         }
      }
   }

   // convert to desired output format
   if (req_comp && req_comp != 4) {
      if (ri->bits_per_channel == 16)
         out = (stbi_uc *) stbi__convert_format16((stbi__uint16 *) out, 4, req_comp, w, h);
      else
         out = stbi__convert_format(out, 4, req_comp, w, h);
      if (out == NULL) return out; // stbi__convert_format frees input on failure
   }

   if (comp) *comp = 4;
   *y = h;
   *x = w;

   return out;
}
#endif

// *************************************************************************************************
// Softimage PIC loader
// by Tom Seddon
//
// See http://softimage.wiki.softimage.com/index.php/INFO:_PIC_file_format
// See http://ozviz.wasp.uwa.edu.au/~pbourke/dataformats/softimagepic/

#ifndef STBI_NO_PIC
static int stbi__pic_is4(stbi__context *s,const char *str)
{
   int i;
   for (i=0; i<4; ++i)
      if (stbi__get8(s) != (stbi_uc)str[i])
         return 0;

   return 1;
}

static int stbi__pic_test_core(stbi__context *s)
{
   int i;

   if (!stbi__pic_is4(s,"\x53\x80\xF6\x34"))
      return 0;

   for(i=0;i<84;++i)
      stbi__get8(s);

   if (!stbi__pic_is4(s,"PICT"))
      return 0;

   return 1;
}

typedef struct
{
   stbi_uc size,type,channel;
} stbi__pic_packet;

static stbi_uc *stbi__readval(stbi__context *s, int channel, stbi_uc *dest)
{
   int mask=0x80, i;

   for (i=0; i<4; ++i, mask>>=1) {
      if (channel & mask) {
         if (stbi__at_eof(s)) return stbi__errpuc("bad file","PIC file too short");
         dest[i]=stbi__get8(s);
      }
   }

   return dest;
}

static void stbi__copyval(int channel,stbi_uc *dest,const stbi_uc *src)
{
   int mask=0x80,i;

   for (i=0;i<4; ++i, mask>>=1)
      if (channel&mask)
         dest[i]=src[i];
}

static stbi_uc *stbi__pic_load_core(stbi__context *s,int width,int height,int *comp, stbi_uc *result)
{
   int act_comp=0,num_packets=0,y,chained;
   stbi__pic_packet packets[10];

   // this will (should...) cater for even some bizarre stuff like having data
    // for the same channel in multiple packets.
   do {
      stbi__pic_packet *packet;

      if (num_packets==sizeof(packets)/sizeof(packets[0]))
         return stbi__errpuc("bad format","too many packets");

      packet = &packets[num_packets++];

      chained = stbi__get8(s);
      packet->size    = stbi__get8(s);
      packet->type    = stbi__get8(s);
      packet->channel = stbi__get8(s);

      act_comp |= packet->channel;

      if (stbi__at_eof(s))          return stbi__errpuc("bad file","file too short (reading packets)");
      if (packet->size != 8)  return stbi__errpuc("bad format","packet isn't 8bpp");
   } while (chained);

   *comp = (act_comp & 0x10 ? 4 : 3); // has alpha channel?

   for(y=0; y<height; ++y) {
      int packet_idx;

      for(packet_idx=0; packet_idx < num_packets; ++packet_idx) {
         stbi__pic_packet *packet = &packets[packet_idx];
         stbi_uc *dest = result+y*width*4;

         switch (packet->type) {
            default:
               return stbi__errpuc("bad format","packet has bad compression type");

            case 0: {//uncompressed
               int x;

               for(x=0;x<width;++x, dest+=4)
                  if (!stbi__readval(s,packet->channel,dest))
                     return 0;
               break;
            }

            case 1://Pure RLE
               {
                  int left=width, i;

                  while (left>0) {
                     stbi_uc count,value[4];

                     count=stbi__get8(s);
                     if (stbi__at_eof(s))   return stbi__errpuc("bad file","file too short (pure read count)");

                     if (count > left)
                        count = (stbi_uc) left;

                     if (!stbi__readval(s,packet->channel,value))  return 0;

                     for(i=0; i<count; ++i,dest+=4)
                        stbi__copyval(packet->channel,dest,value);
                     left -= count;
                  }
               }
               break;

            case 2: {//Mixed RLE
               int left=width;
               while (left>0) {
                  int count = stbi__get8(s), i;
                  if (stbi__at_eof(s))  return stbi__errpuc("bad file","file too short (mixed read count)");

                  if (count >= 128) { // Repeated
                     stbi_uc value[4];

                     if (count==128)
                        count = stbi__get16be(s);
                     else
                        count -= 127;
                     if (count > left)
                        return stbi__errpuc("bad file","scanline overrun");

                     if (!stbi__readval(s,packet->channel,value))
                        return 0;

                     for(i=0;i<count;++i, dest += 4)
                        stbi__copyval(packet->channel,dest,value);
                  } else { // Raw
                     ++count;
                     if (count>left) return stbi__errpuc("bad file","scanline overrun");

                     for(i=0;i<count;++i, dest+=4)
                        if (!stbi__readval(s,packet->channel,dest))
                           return 0;
                  }
                  left-=count;
               }
               break;
            }
         }
      }
   }

   return result;
}

static void *stbi__pic_load(stbi__context *s,int *px,int *py,int *comp,int req_comp, stbi__result_info *ri)
{
   stbi_uc *result;
   int i, x,y, internal_comp;
   STBI_NOTUSED(ri);

   if (!comp) comp = &internal_comp;

   for (i=0; i<92; ++i)
      stbi__get8(s);

   x = stbi__get16be(s);
   y = stbi__get16be(s);

   if (y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   if (stbi__at_eof(s))  return stbi__errpuc("bad file","file too short (pic header)");
   if (!stbi__mad3sizes_valid(x, y, 4, 0)) return stbi__errpuc("too large", "PIC image too large to decode");

   stbi__get32be(s); //skip `ratio'
   stbi__get16be(s); //skip `fields'
   stbi__get16be(s); //skip `pad'

   // intermediate buffer is RGBA
   result = (stbi_uc *) stbi__malloc_mad3(x, y, 4, 0);
   if (!result) return stbi__errpuc("outofmem", "Out of memory");
   memset(result, 0xff, x*y*4);

   if (!stbi__pic_load_core(s,x,y,comp, result)) {
      STBI_FREE(result);
      result=0;
   }
   *px = x;
   *py = y;
   if (req_comp == 0) req_comp = *comp;
   result=stbi__convert_format(result,4,req_comp,x,y);

   return result;
}

static int stbi__pic_test(stbi__context *s)
{
   int r = stbi__pic_test_core(s);
   stbi__rewind(s);
   return r;
}
#endif

// *************************************************************************************************
// GIF loader -- public domain by Jean-Marc Lienher -- simplified/shrunk by stb

#ifndef STBI_NO_GIF
typedef struct
{
   stbi__int16 prefix;
   stbi_uc first;
   stbi_uc suffix;
} stbi__gif_lzw;

typedef struct
{
   int w,h;
   stbi_uc *out;                 // output buffer (always 4 components)
   stbi_uc *background;          // The current "background" as far as a gif is concerned
   stbi_uc *history;
   int flags, bgindex, ratio, transparent, eflags;
   stbi_uc  pal[256][4];
   stbi_uc lpal[256][4];
   stbi__gif_lzw codes[8192];
   stbi_uc *color_table;
   int parse, step;
   int lflags;
   int start_x, start_y;
   int max_x, max_y;
   int cur_x, cur_y;
   int line_size;
   int delay;
} stbi__gif;

static int stbi__gif_test_raw(stbi__context *s)
{
   int sz;
   if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8') return 0;
   sz = stbi__get8(s);
   if (sz != '9' && sz != '7') return 0;
   if (stbi__get8(s) != 'a') return 0;
   return 1;
}

static int stbi__gif_test(stbi__context *s)
{
   int r = stbi__gif_test_raw(s);
   stbi__rewind(s);
   return r;
}

static void stbi__gif_parse_colortable(stbi__context *s, stbi_uc pal[256][4], int num_entries, int transp)
{
   int i;
   for (i=0; i < num_entries; ++i) {
      pal[i][2] = stbi__get8(s);
      pal[i][1] = stbi__get8(s);
      pal[i][0] = stbi__get8(s);
      pal[i][3] = transp == i ? 0 : 255;
   }
}

static int stbi__gif_header(stbi__context *s, stbi__gif *g, int *comp, int is_info)
{
   stbi_uc version;
   if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8')
      return stbi__err("not GIF", "Corrupt GIF");

   version = stbi__get8(s);
   if (version != '7' && version != '9')    return stbi__err("not GIF", "Corrupt GIF");
   if (stbi__get8(s) != 'a')                return stbi__err("not GIF", "Corrupt GIF");

   stbi__g_failure_reason = "";
   g->w = stbi__get16le(s);
   g->h = stbi__get16le(s);
   g->flags = stbi__get8(s);
   g->bgindex = stbi__get8(s);
   g->ratio = stbi__get8(s);
   g->transparent = -1;

   if (g->w > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
   if (g->h > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");

   if (comp != 0) *comp = 4;  // can't actually tell whether it's 3 or 4 until we parse the comments

   if (is_info) return 1;

   if (g->flags & 0x80)
      stbi__gif_parse_colortable(s,g->pal, 2 << (g->flags & 7), -1);

   return 1;
}

static int stbi__gif_info_raw(stbi__context *s, int *x, int *y, int *comp)
{
   stbi__gif* g = (stbi__gif*) stbi__malloc(sizeof(stbi__gif));
   if (!g) return stbi__err("outofmem", "Out of memory");
   if (!stbi__gif_header(s, g, comp, 1)) {
      STBI_FREE(g);
      stbi__rewind( s );
      return 0;
   }
   if (x) *x = g->w;
   if (y) *y = g->h;
   STBI_FREE(g);
   return 1;
}

static void stbi__out_gif_code(stbi__gif *g, stbi__uint16 code)
{
   stbi_uc *p, *c;
   int idx;

   // recurse to decode the prefixes, since the linked-list is backwards,
   // and working backwards through an interleaved image would be nasty
   if (g->codes[code].prefix >= 0)
      stbi__out_gif_code(g, g->codes[code].prefix);

   if (g->cur_y >= g->max_y) return;

   idx = g->cur_x + g->cur_y;
   p = &g->out[idx];
   g->history[idx / 4] = 1;

   c = &g->color_table[g->codes[code].suffix * 4];
   if (c[3] > 128) { // don't render transparent pixels;
      p[0] = c[2];
      p[1] = c[1];
      p[2] = c[0];
      p[3] = c[3];
   }
   g->cur_x += 4;

   if (g->cur_x >= g->max_x) {
      g->cur_x = g->start_x;
      g->cur_y += g->step;

      while (g->cur_y >= g->max_y && g->parse > 0) {
         g->step = (1 << g->parse) * g->line_size;
         g->cur_y = g->start_y + (g->step >> 1);
         --g->parse;
      }
   }
}

static stbi_uc *stbi__process_gif_raster(stbi__context *s, stbi__gif *g)
{
   stbi_uc lzw_cs;
   stbi__int32 len, init_code;
   stbi__uint32 first;
   stbi__int32 codesize, codemask, avail, oldcode, bits, valid_bits, clear;
   stbi__gif_lzw *p;

   lzw_cs = stbi__get8(s);
   if (lzw_cs > 12) return NULL;
   clear = 1 << lzw_cs;
   first = 1;
   codesize = lzw_cs + 1;
   codemask = (1 << codesize) - 1;
   bits = 0;
   valid_bits = 0;
   for (init_code = 0; init_code < clear; init_code++) {
      g->codes[init_code].prefix = -1;
      g->codes[init_code].first = (stbi_uc) init_code;
      g->codes[init_code].suffix = (stbi_uc) init_code;
   }

   // support no starting clear code
   avail = clear+2;
   oldcode = -1;

   len = 0;
   for(;;) {
      if (valid_bits < codesize) {
         if (len == 0) {
            len = stbi__get8(s); // start new block
            if (len == 0)
               return g->out;
         }
         --len;
         bits |= (stbi__int32) stbi__get8(s) << valid_bits;
         valid_bits += 8;
      } else {
         stbi__int32 code = bits & codemask;
         bits >>= codesize;
         valid_bits -= codesize;
         // @OPTIMIZE: is there some way we can accelerate the non-clear path?
         if (code == clear) {  // clear code
            codesize = lzw_cs + 1;
            codemask = (1 << codesize) - 1;
            avail = clear + 2;
            oldcode = -1;
            first = 0;
         } else if (code == clear + 1) { // end of stream code
            stbi__skip(s, len);
            while ((len = stbi__get8(s)) > 0)
               stbi__skip(s,len);
            return g->out;
         } else if (code <= avail) {
            if (first) {
               return stbi__errpuc("no clear code", "Corrupt GIF");
            }

            if (oldcode >= 0) {
               p = &g->codes[avail++];
               if (avail > 8192) {
                  return stbi__errpuc("too many codes", "Corrupt GIF");
               }

               p->prefix = (stbi__int16) oldcode;
               p->first = g->codes[oldcode].first;
               p->suffix = (code == avail) ? p->first : g->codes[code].first;
            } else if (code == avail)
               return stbi__errpuc("illegal code in raster", "Corrupt GIF");

            stbi__out_gif_code(g, (stbi__uint16) code);

            if ((avail & codemask) == 0 && avail <= 0x0FFF) {
               codesize++;
               codemask = (1 << codesize) - 1;
            }

            oldcode = code;
         } else {
            return stbi__errpuc("illegal code in raster", "Corrupt GIF");
         }
      }
   }
}

// this function is designed to support animated gifs, although stb_image doesn't support it
// two back is the image from two frames ago, used for a very specific disposal format
static stbi_uc *stbi__gif_load_next(stbi__context *s, stbi__gif *g, int *comp, int req_comp, stbi_uc *two_back)
{
   int dispose;
   int first_frame;
   int pi;
   int pcount;
   STBI_NOTUSED(req_comp);

   // on first frame, any non-written pixels get the background colour (non-transparent)
   first_frame = 0;
   if (g->out == 0) {
      if (!stbi__gif_header(s, g, comp,0)) return 0; // stbi__g_failure_reason set by stbi__gif_header
      if (!stbi__mad3sizes_valid(4, g->w, g->h, 0))
         return stbi__errpuc("too large", "GIF image is too large");
      pcount = g->w * g->h;
      g->out = (stbi_uc *) stbi__malloc(4 * pcount);
      g->background = (stbi_uc *) stbi__malloc(4 * pcount);
      g->history = (stbi_uc *) stbi__malloc(pcount);
      if (!g->out || !g->background || !g->history)
         return stbi__errpuc("outofmem", "Out of memory");

      // image is treated as "transparent" at the start - ie, nothing overwrites the current background;
      // background colour is only used for pixels that are not rendered first frame, after that "background"
      // color refers to the color that was there the previous frame.
      memset(g->out, 0x00, 4 * pcount);
      memset(g->background, 0x00, 4 * pcount); // state of the background (starts transparent)
      memset(g->history, 0x00, pcount);        // pixels that were affected previous frame
      first_frame = 1;
   } else {
      // second frame - how do we dispose of the previous one?
      dispose = (g->eflags & 0x1C) >> 2;
      pcount = g->w * g->h;

      if ((dispose == 3) && (two_back == 0)) {
         dispose = 2; // if I don't have an image to revert back to, default to the old background
      }

      if (dispose == 3) { // use previous graphic
         for (pi = 0; pi < pcount; ++pi) {
            if (g->history[pi]) {
               memcpy( &g->out[pi * 4], &two_back[pi * 4], 4 );
            }
         }
      } else if (dispose == 2) {
         // restore what was changed last frame to background before that frame;
         for (pi = 0; pi < pcount; ++pi) {
            if (g->history[pi]) {
               memcpy( &g->out[pi * 4], &g->background[pi * 4], 4 );
            }
         }
      } else {
         // This is a non-disposal case eithe way, so just
         // leave the pixels as is, and they will become the new background
         // 1: do not dispose
         // 0:  not specified.
      }

      // background is what out is after the undoing of the previou frame;
      memcpy( g->background, g->out, 4 * g->w * g->h );
   }

   // clear my history;
   memset( g->history, 0x00, g->w * g->h );        // pixels that were affected previous frame

   for (;;) {
      int tag = stbi__get8(s);
      switch (tag) {
         case 0x2C: /* Image Descriptor */
         {
            stbi__int32 x, y, w, h;
            stbi_uc *o;

            x = stbi__get16le(s);
            y = stbi__get16le(s);
            w = stbi__get16le(s);
            h = stbi__get16le(s);
            if (((x + w) > (g->w)) || ((y + h) > (g->h)))
               return stbi__errpuc("bad Image Descriptor", "Corrupt GIF");

            g->line_size = g->w * 4;
            g->start_x = x * 4;
            g->start_y = y * g->line_size;
            g->max_x   = g->start_x + w * 4;
            g->max_y   = g->start_y + h * g->line_size;
            g->cur_x   = g->start_x;
            g->cur_y   = g->start_y;

            // if the width of the specified rectangle is 0, that means
            // we may not see *any* pixels or the image is malformed;
            // to make sure this is caught, move the current y down to
            // max_y (which is what out_gif_code checks).
            if (w == 0)
               g->cur_y = g->max_y;

            g->lflags = stbi__get8(s);

            if (g->lflags & 0x40) {
               g->step = 8 * g->line_size; // first interlaced spacing
               g->parse = 3;
            } else {
               g->step = g->line_size;
               g->parse = 0;
            }

            if (g->lflags & 0x80) {
               stbi__gif_parse_colortable(s,g->lpal, 2 << (g->lflags & 7), g->eflags & 0x01 ? g->transparent : -1);
               g->color_table = (stbi_uc *) g->lpal;
            } else if (g->flags & 0x80) {
               g->color_table = (stbi_uc *) g->pal;
            } else
               return stbi__errpuc("missing color table", "Corrupt GIF");

            o = stbi__process_gif_raster(s, g);
            if (!o) return NULL;

            // if this was the first frame,
            pcount = g->w * g->h;
            if (first_frame && (g->bgindex > 0)) {
               // if first frame, any pixel not drawn to gets the background color
               for (pi = 0; pi < pcount; ++pi) {
                  if (g->history[pi] == 0) {
                     g->pal[g->bgindex][3] = 255; // just in case it was made transparent, undo that; It will be reset next frame if need be;
                     memcpy( &g->out[pi * 4], &g->pal[g->bgindex], 4 );
                  }
               }
            }

            return o;
         }

         case 0x21: // Comment Extension.
         {
            int len;
            int ext = stbi__get8(s);
            if (ext == 0xF9) { // Graphic Control Extension.
               len = stbi__get8(s);
               if (len == 4) {
                  g->eflags = stbi__get8(s);
                  g->delay = 10 * stbi__get16le(s); // delay - 1/100th of a second, saving as 1/1000ths.

                  // unset old transparent
                  if (g->transparent >= 0) {
                     g->pal[g->transparent][3] = 255;
                  }
                  if (g->eflags & 0x01) {
                     g->transparent = stbi__get8(s);
                     if (g->transparent >= 0) {
                        g->pal[g->transparent][3] = 0;
                     }
                  } else {
                     // don't need transparent
                     stbi__skip(s, 1);
                     g->transparent = -1;
                  }
               } else {
                  stbi__skip(s, len);
                  break;
               }
            }
            while ((len = stbi__get8(s)) != 0) {
               stbi__skip(s, len);
            }
            break;
         }

         case 0x3B: // gif stream termination code
            return (stbi_uc *) s; // using '1' causes warning on some compilers

         default:
            return stbi__errpuc("unknown code", "Corrupt GIF");
      }
   }
}

static void *stbi__load_gif_main_outofmem(stbi__gif *g, stbi_uc *out, int **delays)
{
   STBI_FREE(g->out);
   STBI_FREE(g->history);
   STBI_FREE(g->background);

   if (out) STBI_FREE(out);
   if (delays && *delays) STBI_FREE(*delays);
   return stbi__errpuc("outofmem", "Out of memory");
}

static void *stbi__load_gif_main(stbi__context *s, int **delays, int *x, int *y, int *z, int *comp, int req_comp)
{
   if (stbi__gif_test(s)) {
      int layers = 0;
      stbi_uc *u = 0;
      stbi_uc *out = 0;
      stbi_uc *two_back = 0;
      stbi__gif g;
      int stride;
      int out_size = 0;
      int delays_size = 0;

      STBI_NOTUSED(out_size);
      STBI_NOTUSED(delays_size);

      memset(&g, 0, sizeof(g));
      if (delays) {
         *delays = 0;
      }

      do {
         u = stbi__gif_load_next(s, &g, comp, req_comp, two_back);
         if (u == (stbi_uc *) s) u = 0;  // end of animated gif marker

         if (u) {
            *x = g.w;
            *y = g.h;
            ++layers;
            stride = g.w * g.h * 4;

            if (out) {
               void *tmp = (stbi_uc*) STBI_REALLOC_SIZED( out, out_size, layers * stride );
               if (!tmp)
                  return stbi__load_gif_main_outofmem(&g, out, delays);
               else {
                   out = (stbi_uc*) tmp;
                   out_size = layers * stride;
               }

               if (delays) {
                  int *new_delays = (int*) STBI_REALLOC_SIZED( *delays, delays_size, sizeof(int) * layers );
                  if (!new_delays)
                     return stbi__load_gif_main_outofmem(&g, out, delays);
                  *delays = new_delays;
                  delays_size = layers * sizeof(int);
               }
            } else {
               out = (stbi_uc*)stbi__malloc( layers * stride );
               if (!out)
                  return stbi__load_gif_main_outofmem(&g, out, delays);
               out_size = layers * stride;
               if (delays) {
                  *delays = (int*) stbi__malloc( layers * sizeof(int) );
                  if (!*delays)
                     return stbi__load_gif_main_outofmem(&g, out, delays);
                  delays_size = layers * sizeof(int);
               }
            }
            memcpy( out + ((layers - 1) * stride), u, stride );
            if (layers >= 2) {
               two_back = out - 2 * stride;
            }

            if (delays) {
               (*delays)[layers - 1U] = g.delay;
            }
         }
      } while (u != 0);

      // free temp buffer;
      STBI_FREE(g.out);
      STBI_FREE(g.history);
      STBI_FREE(g.background);

      // do the final conversion after loading everything;
      if (req_comp && req_comp != 4)
         out = stbi__convert_format(out, 4, req_comp, layers * g.w, g.h);

      *z = layers;
      return out;
   } else {
      return stbi__errpuc("not GIF", "Image was not as a gif type.");
   }
}

static void *stbi__gif_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi_uc *u = 0;
   stbi__gif g;
   memset(&g, 0, sizeof(g));
   STBI_NOTUSED(ri);

   u = stbi__gif_load_next(s, &g, comp, req_comp, 0);
   if (u == (stbi_uc *) s) u = 0;  // end of animated gif marker
   if (u) {
      *x = g.w;
      *y = g.h;

      // moved conversion to after successful load so that the same
      // can be done for multiple frames.
      if (req_comp && req_comp != 4)
         u = stbi__convert_format(u, 4, req_comp, g.w, g.h);
   } else if (g.out) {
      // if there was an error and we allocated an image buffer, free it!
      STBI_FREE(g.out);
   }

   // free buffers needed for multiple frame loading;
   STBI_FREE(g.history);
   STBI_FREE(g.background);

   return u;
}

static int stbi__gif_info(stbi__context *s, int *x, int *y, int *comp)
{
   return stbi__gif_info_raw(s,x,y,comp);
}
#endif

// *************************************************************************************************
// Radiance RGBE HDR loader
// originally by Nicolas Schulz
#ifndef STBI_NO_HDR
static int stbi__hdr_test_core(stbi__context *s, const char *signature)
{
   int i;
   for (i=0; signature[i]; ++i)
      if (stbi__get8(s) != signature[i])
          return 0;
   stbi__rewind(s);
   return 1;
}

static int stbi__hdr_test(stbi__context* s)
{
   int r = stbi__hdr_test_core(s, "#?RADIANCE\n");
   stbi__rewind(s);
   if(!r) {
       r = stbi__hdr_test_core(s, "#?RGBE\n");
       stbi__rewind(s);
   }
   return r;
}

#define STBI__HDR_BUFLEN  1024
static char *stbi__hdr_gettoken(stbi__context *z, char *buffer)
{
   int len=0;
   char c = '\0';

   c = (char) stbi__get8(z);

   while (!stbi__at_eof(z) && c != '\n') {
      buffer[len++] = c;
      if (len == STBI__HDR_BUFLEN-1) {
         // flush to end of line
         while (!stbi__at_eof(z) && stbi__get8(z) != '\n')
            ;
         break;
      }
      c = (char) stbi__get8(z);
   }

   buffer[len] = 0;
   return buffer;
}

static void stbi__hdr_convert(float *output, stbi_uc *input, int req_comp)
{
   if ( input[3] != 0 ) {
      float f1;
      // Exponent
      f1 = (float) ldexp(1.0f, input[3] - (int)(128 + 8));
      if (req_comp <= 2)
         output[0] = (input[0] + input[1] + input[2]) * f1 / 3;
      else {
         output[0] = input[0] * f1;
         output[1] = input[1] * f1;
         output[2] = input[2] * f1;
      }
      if (req_comp == 2) output[1] = 1;
      if (req_comp == 4) output[3] = 1;
   } else {
      switch (req_comp) {
         case 4: output[3] = 1; /* fallthrough */
         case 3: output[0] = output[1] = output[2] = 0;
                 break;
         case 2: output[1] = 1; /* fallthrough */
         case 1: output[0] = 0;
                 break;
      }
   }
}

static float *stbi__hdr_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   char buffer[STBI__HDR_BUFLEN];
   char *token;
   int valid = 0;
   int width, height;
   stbi_uc *scanline;
   float *hdr_data;
   int len;
   unsigned char count, value;
   int i, j, k, c1,c2, z;
   const char *headerToken;
   STBI_NOTUSED(ri);

   // Check identifier
   headerToken = stbi__hdr_gettoken(s,buffer);
   if (strcmp(headerToken, "#?RADIANCE") != 0 && strcmp(headerToken, "#?RGBE") != 0)
      return stbi__errpf("not HDR", "Corrupt HDR image");

   // Parse header
   for(;;) {
      token = stbi__hdr_gettoken(s,buffer);
      if (token[0] == 0) break;
      if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
   }

   if (!valid)    return stbi__errpf("unsupported format", "Unsupported HDR format");

   // Parse width and height
   // can't use sscanf() if we're not using stdio!
   token = stbi__hdr_gettoken(s,buffer);
   if (strncmp(token, "-Y ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
   token += 3;
   height = (int) strtol(token, &token, 10);
   while (*token == ' ') ++token;
   if (strncmp(token, "+X ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
   token += 3;
   width = (int) strtol(token, NULL, 10);

   if (height > STBI_MAX_DIMENSIONS) return stbi__errpf("too large","Very large image (corrupt?)");
   if (width > STBI_MAX_DIMENSIONS) return stbi__errpf("too large","Very large image (corrupt?)");

   *x = width;
   *y = height;

   if (comp) *comp = 3;
   if (req_comp == 0) req_comp = 3;

   if (!stbi__mad4sizes_valid(width, height, req_comp, sizeof(float), 0))
      return stbi__errpf("too large", "HDR image is too large");

   // Read data
   hdr_data = (float *) stbi__malloc_mad4(width, height, req_comp, sizeof(float), 0);
   if (!hdr_data)
      return stbi__errpf("outofmem", "Out of memory");

   // Load image data
   // image data is stored as some number of sca
   if ( width < 8 || width >= 32768) {
      // Read flat data
      for (j=0; j < height; ++j) {
         for (i=0; i < width; ++i) {
            stbi_uc rgbe[4];
           main_decode_loop:
            stbi__getn(s, rgbe, 4);
            stbi__hdr_convert(hdr_data + j * width * req_comp + i * req_comp, rgbe, req_comp);
         }
      }
   } else {
      // Read RLE-encoded data
      scanline = NULL;

      for (j = 0; j < height; ++j) {
         c1 = stbi__get8(s);
         c2 = stbi__get8(s);
         len = stbi__get8(s);
         if (c1 != 2 || c2 != 2 || (len & 0x80)) {
            // not run-length encoded, so we have to actually use THIS data as a decoded
            // pixel (note this can't be a valid pixel--one of RGB must be >= 128)
            stbi_uc rgbe[4];
            rgbe[0] = (stbi_uc) c1;
            rgbe[1] = (stbi_uc) c2;
            rgbe[2] = (stbi_uc) len;
            rgbe[3] = (stbi_uc) stbi__get8(s);
            stbi__hdr_convert(hdr_data, rgbe, req_comp);
            i = 1;
            j = 0;
            STBI_FREE(scanline);
            goto main_decode_loop; // yes, this makes no sense
         }
         len <<= 8;
         len |= stbi__get8(s);
         if (len != width) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("invalid decoded scanline length", "corrupt HDR"); }
         if (scanline == NULL) {
            scanline = (stbi_uc *) stbi__malloc_mad2(width, 4, 0);
            if (!scanline) {
               STBI_FREE(hdr_data);
               return stbi__errpf("outofmem", "Out of memory");
            }
         }

         for (k = 0; k < 4; ++k) {
            int nleft;
            i = 0;
            while ((nleft = width - i) > 0) {
               count = stbi__get8(s);
               if (count > 128) {
                  // Run
                  value = stbi__get8(s);
                  count -= 128;
                  if ((count == 0) || (count > nleft)) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("corrupt", "bad RLE data in HDR"); }
                  for (z = 0; z < count; ++z)
                     scanline[i++ * 4 + k] = value;
               } else {
                  // Dump
                  if ((count == 0) || (count > nleft)) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("corrupt", "bad RLE data in HDR"); }
                  for (z = 0; z < count; ++z)
                     scanline[i++ * 4 + k] = stbi__get8(s);
               }
            }
         }
         for (i=0; i < width; ++i)
            stbi__hdr_convert(hdr_data+(j*width + i)*req_comp, scanline + i*4, req_comp);
      }
      if (scanline)
         STBI_FREE(scanline);
   }

   return hdr_data;
}

static int stbi__hdr_info(stbi__context *s, int *x, int *y, int *comp)
{
   char buffer[STBI__HDR_BUFLEN];
   char *token;
   int valid = 0;
   int dummy;

   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;

   if (stbi__hdr_test(s) == 0) {
       stbi__rewind( s );
       return 0;
   }

   for(;;) {
      token = stbi__hdr_gettoken(s,buffer);
      if (token[0] == 0) break;
      if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
   }

   if (!valid) {
       stbi__rewind( s );
       return 0;
   }
   token = stbi__hdr_gettoken(s,buffer);
   if (strncmp(token, "-Y ", 3)) {
       stbi__rewind( s );
       return 0;
   }
   token += 3;
   *y = (int) strtol(token, &token, 10);
   while (*token == ' ') ++token;
   if (strncmp(token, "+X ", 3)) {
       stbi__rewind( s );
       return 0;
   }
   token += 3;
   *x = (int) strtol(token, NULL, 10);
   *comp = 3;
   return 1;
}
#endif // STBI_NO_HDR

#ifndef STBI_NO_BMP
static int stbi__bmp_info(stbi__context *s, int *x, int *y, int *comp)
{
   void *p;
   stbi__bmp_data info;

   info.all_a = 255;
   p = stbi__bmp_parse_header(s, &info);
   if (p == NULL) {
      stbi__rewind( s );
      return 0;
   }
   if (x) *x = s->img_x;
   if (y) *y = s->img_y;
   if (comp) {
      if (info.bpp == 24 && info.ma == 0xff000000)
         *comp = 3;
      else
         *comp = info.ma ? 4 : 3;
   }
   return 1;
}
#endif

#ifndef STBI_NO_PSD
static int stbi__psd_info(stbi__context *s, int *x, int *y, int *comp)
{
   int channelCount, dummy, depth;
   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;
   if (stbi__get32be(s) != 0x38425053) {
       stbi__rewind( s );
       return 0;
   }
   if (stbi__get16be(s) != 1) {
       stbi__rewind( s );
       return 0;
   }
   stbi__skip(s, 6);
   channelCount = stbi__get16be(s);
   if (channelCount < 0 || channelCount > 16) {
       stbi__rewind( s );
       return 0;
   }
   *y = stbi__get32be(s);
   *x = stbi__get32be(s);
   depth = stbi__get16be(s);
   if (depth != 8 && depth != 16) {
       stbi__rewind( s );
       return 0;
   }
   if (stbi__get16be(s) != 3) {
       stbi__rewind( s );
       return 0;
   }
   *comp = 4;
   return 1;
}

static int stbi__psd_is16(stbi__context *s)
{
   int channelCount, depth;
   if (stbi__get32be(s) != 0x38425053) {
       stbi__rewind( s );
       return 0;
   }
   if (stbi__get16be(s) != 1) {
       stbi__rewind( s );
       return 0;
   }
   stbi__skip(s, 6);
   channelCount = stbi__get16be(s);
   if (channelCount < 0 || channelCount > 16) {
       stbi__rewind( s );
       return 0;
   }
   STBI_NOTUSED(stbi__get32be(s));
   STBI_NOTUSED(stbi__get32be(s));
   depth = stbi__get16be(s);
   if (depth != 16) {
       stbi__rewind( s );
       return 0;
   }
   return 1;
}
#endif

#ifndef STBI_NO_PIC
static int stbi__pic_info(stbi__context *s, int *x, int *y, int *comp)
{
   int act_comp=0,num_packets=0,chained,dummy;
   stbi__pic_packet packets[10];

   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;

   if (!stbi__pic_is4(s,"\x53\x80\xF6\x34")) {
      stbi__rewind(s);
      return 0;
   }

   stbi__skip(s, 88);

   *x = stbi__get16be(s);
   *y = stbi__get16be(s);
   if (stbi__at_eof(s)) {
      stbi__rewind( s);
      return 0;
   }
   if ( (*x) != 0 && (1 << 28) / (*x) < (*y)) {
      stbi__rewind( s );
      return 0;
   }

   stbi__skip(s, 8);

   do {
      stbi__pic_packet *packet;

      if (num_packets==sizeof(packets)/sizeof(packets[0]))
         return 0;

      packet = &packets[num_packets++];
      chained = stbi__get8(s);
      packet->size    = stbi__get8(s);
      packet->type    = stbi__get8(s);
      packet->channel = stbi__get8(s);
      act_comp |= packet->channel;

      if (stbi__at_eof(s)) {
          stbi__rewind( s );
          return 0;
      }
      if (packet->size != 8) {
          stbi__rewind( s );
          return 0;
      }
   } while (chained);

   *comp = (act_comp & 0x10 ? 4 : 3);

   return 1;
}
#endif

// *************************************************************************************************
// Portable Gray Map and Portable Pixel Map loader
// by Ken Miller
//
// PGM: http://netpbm.sourceforge.net/doc/pgm.html
// PPM: http://netpbm.sourceforge.net/doc/ppm.html
//
// Known limitations:
//    Does not support comments in the header section
//    Does not support ASCII image data (formats P2 and P3)

#ifndef STBI_NO_PNM

static int      stbi__pnm_test(stbi__context *s)
{
   char p, t;
   p = (char) stbi__get8(s);
   t = (char) stbi__get8(s);
   if (p != 'P' || (t != '5' && t != '6')) {
       stbi__rewind( s );
       return 0;
   }
   return 1;
}

static void *stbi__pnm_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi_uc *out;
   STBI_NOTUSED(ri);

   ri->bits_per_channel = stbi__pnm_info(s, (int *)&s->img_x, (int *)&s->img_y, (int *)&s->img_n);
   if (ri->bits_per_channel == 0)
      return 0;

   if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   *x = s->img_x;
   *y = s->img_y;
   if (comp) *comp = s->img_n;

   if (!stbi__mad4sizes_valid(s->img_n, s->img_x, s->img_y, ri->bits_per_channel / 8, 0))
      return stbi__errpuc("too large", "PNM too large");

   out = (stbi_uc *) stbi__malloc_mad4(s->img_n, s->img_x, s->img_y, ri->bits_per_channel / 8, 0);
   if (!out) return stbi__errpuc("outofmem", "Out of memory");
   if (!stbi__getn(s, out, s->img_n * s->img_x * s->img_y * (ri->bits_per_channel / 8))) {
      STBI_FREE(out);
      return stbi__errpuc("bad PNM", "PNM file truncated");
   }

   if (req_comp && req_comp != s->img_n) {
      if (ri->bits_per_channel == 16) {
         out = (stbi_uc *) stbi__convert_format16((stbi__uint16 *) out, s->img_n, req_comp, s->img_x, s->img_y);
      } else {
         out = stbi__convert_format(out, s->img_n, req_comp, s->img_x, s->img_y);
      }
      if (out == NULL) return out; // stbi__convert_format frees input on failure
   }
   return out;
}

static int      stbi__pnm_isspace(char c)
{
   return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

static void     stbi__pnm_skip_whitespace(stbi__context *s, char *c)
{
   for (;;) {
      while (!stbi__at_eof(s) && stbi__pnm_isspace(*c))
         *c = (char) stbi__get8(s);

      if (stbi__at_eof(s) || *c != '#')
         break;

      while (!stbi__at_eof(s) && *c != '\n' && *c != '\r' )
         *c = (char) stbi__get8(s);
   }
}

static int      stbi__pnm_isdigit(char c)
{
   return c >= '0' && c <= '9';
}

static int      stbi__pnm_getinteger(stbi__context *s, char *c)
{
   int value = 0;

   while (!stbi__at_eof(s) && stbi__pnm_isdigit(*c)) {
      value = value*10 + (*c - '0');
      *c = (char) stbi__get8(s);
      if((value > 214748364) || (value == 214748364 && *c > '7'))
          return stbi__err("integer parse overflow", "Parsing an integer in the PPM header overflowed a 32-bit int");
   }

   return value;
}

static int      stbi__pnm_info(stbi__context *s, int *x, int *y, int *comp)
{
   int maxv, dummy;
   char c, p, t;

   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;

   stbi__rewind(s);

   // Get identifier
   p = (char) stbi__get8(s);
   t = (char) stbi__get8(s);
   if (p != 'P' || (t != '5' && t != '6')) {
       stbi__rewind(s);
       return 0;
   }

   *comp = (t == '6') ? 3 : 1;  // '5' is 1-component .pgm; '6' is 3-component .ppm

   c = (char) stbi__get8(s);
   stbi__pnm_skip_whitespace(s, &c);

   *x = stbi__pnm_getinteger(s, &c); // read width
   if(*x == 0)
       return stbi__err("invalid width", "PPM image header had zero or overflowing width");
   stbi__pnm_skip_whitespace(s, &c);

   *y = stbi__pnm_getinteger(s, &c); // read height
   if (*y == 0)
       return stbi__err("invalid width", "PPM image header had zero or overflowing width");
   stbi__pnm_skip_whitespace(s, &c);

   maxv = stbi__pnm_getinteger(s, &c);  // read max value
   if (maxv > 65535)
      return stbi__err("max value > 65535", "PPM image supports only 8-bit and 16-bit images");
   else if (maxv > 255)
      return 16;
   else
      return 8;
}

static int stbi__pnm_is16(stbi__context *s)
{
   if (stbi__pnm_info(s, NULL, NULL, NULL) == 16)
	   return 1;
   return 0;
}
#endif

static int stbi__info_main(stbi__context *s, int *x, int *y, int *comp)
{
   #ifndef STBI_NO_JPEG
   if (stbi__jpeg_info(s, x, y, comp)) return 1;
   #endif

   #ifndef STBI_NO_PNG
   if (stbi__png_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_GIF
   if (stbi__gif_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_BMP
   if (stbi__bmp_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_PSD
   if (stbi__psd_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_PIC
   if (stbi__pic_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_PNM
   if (stbi__pnm_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_HDR
   if (stbi__hdr_info(s, x, y, comp))  return 1;
   #endif

   // test tga last because it's a crappy test!
   #ifndef STBI_NO_TGA
   if (stbi__tga_info(s, x, y, comp))
       return 1;
   #endif
   return stbi__err("unknown image type", "Image not of any known type, or corrupt");
}

static int stbi__is_16_main(stbi__context *s)
{
   #ifndef STBI_NO_PNG
   if (stbi__png_is16(s))  return 1;
   #endif

   #ifndef STBI_NO_PSD
   if (stbi__psd_is16(s))  return 1;
   #endif

   #ifndef STBI_NO_PNM
   if (stbi__pnm_is16(s))  return 1;
   #endif
   return 0;
}

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const *filename, int *x, int *y, int *comp)
{
    FILE *f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_info_from_file(f, x, y, comp);
    fclose(f);
    return result;
}

STBIDEF int stbi_info_from_file(FILE *f, int *x, int *y, int *comp)
{
   int r;
   stbi__context s;
   long pos = ftell(f);
   stbi__start_file(&s, f);
   r = stbi__info_main(&s,x,y,comp);
   fseek(f,pos,SEEK_SET);
   return r;
}

STBIDEF int stbi_is_16_bit(char const *filename)
{
    FILE *f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_is_16_bit_from_file(f);
    fclose(f);
    return result;
}

STBIDEF int stbi_is_16_bit_from_file(FILE *f)
{
   int r;
   stbi__context s;
   long pos = ftell(f);
   stbi__start_file(&s, f);
   r = stbi__is_16_main(&s);
   fseek(f,pos,SEEK_SET);
   return r;
}
#endif // !STBI_NO_STDIO

STBIDEF int stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__info_main(&s,x,y,comp);
}

STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const *c, void *user, int *x, int *y, int *comp)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) c, user);
   return stbi__info_main(&s,x,y,comp);
}

STBIDEF int stbi_is_16_bit_from_memory(stbi_uc const *buffer, int len)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__is_16_main(&s);
}

STBIDEF int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const *c, void *user)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) c, user);
   return stbi__is_16_main(&s);
}

#endif // STB_IMAGE_IMPLEMENTATION

/*
   revision history:
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings and platform ifdefs
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) change sbti__shiftsigned to avoid clang -O2 bug
                         1-bit BMP
                         *_is_16_bit api
                         avoid warnings
      2.16  (2017-07-23) all functions have 16-bit variants;
                         STBI_NO_STDIO works again;
                         compilation fixes;
                         fix rounding in unpremultiply;
                         optimize vertical flip;
                         disable raw_len validation;
                         documentation fixes
      2.15  (2017-03-18) fix png-1,2,4 bug; now all Imagenet JPGs decode;
                         warning fixes; disable run-time SSE detection on gcc;
                         uniform handling of optional "return" values;
                         thread-safe initialization of zlib tables
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-11-29) add 16-bit API, only supported for PNG right now
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) allocate large structures on the stack
                         remove white matting for transparent PSD
                         fix reported channel count for PNG & BMP
                         re-enable SSE2 in non-gcc 64-bit
                         support RGB-formatted JPEG
                         read 16-bit PNGs (only as 8-bit)
      2.10  (2016-01-22) avoid warning introduced in 2.09 by STBI_REALLOC_SIZED
      2.09  (2016-01-16) allow comments in PNM files
                         16-bit-per-pixel TGA (not bit-per-component)
                         info() for TGA could break due to .hdr handling
                         info() for BMP to shares code instead of sloppy parse
                         can use STBI_REALLOC_SIZED if allocator doesn't support realloc
                         code cleanup
      2.08  (2015-09-13) fix to 2.07 cleanup, reading RGB PSD as RGBA
      2.07  (2015-09-13) fix compiler warnings
                         partial animated GIF support
                         limited 16-bpc PSD support
                         #ifdef unused functions
                         bug with < 92 byte PIC,PNM,HDR,TGA
      2.06  (2015-04-19) fix bug where PSD returns wrong '*comp' value
      2.05  (2015-04-19) fix bug in progressive JPEG handling, fix warning
      2.04  (2015-04-15) try to re-enable SIMD on MinGW 64-bit
      2.03  (2015-04-12) extra corruption checking (mmozeiko)
                         stbi_set_flip_vertically_on_load (nguillemot)
                         fix NEON support; fix mingw support
      2.02  (2015-01-19) fix incorrect assert, fix warning
      2.01  (2015-01-17) fix various warnings; suppress SIMD on gcc 32-bit without -msse2
      2.00b (2014-12-25) fix STBI_MALLOC in progressive JPEG
      2.00  (2014-12-25) optimize JPG, including x86 SSE2 & NEON SIMD (ryg)
                         progressive JPEG (stb)
                         PGM/PPM support (Ken Miller)
                         STBI_MALLOC,STBI_REALLOC,STBI_FREE
                         GIF bugfix -- seemingly never worked
                         STBI_NO_*, STBI_ONLY_*
      1.48  (2014-12-14) fix incorrectly-named assert()
      1.47  (2014-12-14) 1/2/4-bit PNG support, both direct and paletted (Omar Cornut & stb)
                         optimize PNG (ryg)
                         fix bug in interlaced PNG with user-specified channel count (stb)
      1.46  (2014-08-26)
              fix broken tRNS chunk (colorkey-style transparency) in non-paletted PNG
      1.45  (2014-08-16)
              fix MSVC-ARM internal compiler error by wrapping malloc
      1.44  (2014-08-07)
              various warning fixes from Ronny Chevalier
      1.43  (2014-07-15)
              fix MSVC-only compiler problem in code changed in 1.42
      1.42  (2014-07-09)
              don't define _CRT_SECURE_NO_WARNINGS (affects user code)
              fixes to stbi__cleanup_jpeg path
              added STBI_ASSERT to avoid requiring assert.h
      1.41  (2014-06-25)
              fix search&replace from 1.36 that messed up comments/error messages
      1.40  (2014-06-22)
              fix gcc struct-initialization warning
      1.39  (2014-06-15)
              fix to TGA optimization when req_comp != number of components in TGA;
              fix to GIF loading because BMP wasn't rewinding (whoops, no GIFs in my test suite)
              add support for BMP version 5 (more ignored fields)
      1.38  (2014-06-06)
              suppress MSVC warnings on integer casts truncating values
              fix accidental rename of 'skip' field of I/O
      1.37  (2014-06-04)
              remove duplicate typedef
      1.36  (2014-06-03)
              convert to header file single-file library
              if de-iphone isn't set, load iphone images color-swapped instead of returning NULL
      1.35  (2014-05-27)
              various warnings
              fix broken STBI_SIMD path
              fix bug where stbi_load_from_file no longer left file pointer in correct place
              fix broken non-easy path for 32-bit BMP (possibly never used)
              TGA optimization by Arseny Kapoulkine
      1.34  (unknown)
              use STBI_NOTUSED in stbi__resample_row_generic(), fix one more leak in tga failure case
      1.33  (2011-07-14)
              make stbi_is_hdr work in STBI_NO_HDR (as specified), minor compiler-friendly improvements
      1.32  (2011-07-13)
              support for "info" function for all supported filetypes (SpartanJ)
      1.31  (2011-06-20)
              a few more leak fixes, bug in PNG handling (SpartanJ)
      1.30  (2011-06-11)
              added ability to load files via callbacks to accomidate custom input streams (Ben Wenger)
              removed deprecated format-specific test/load functions
              removed support for installable file formats (stbi_loader) -- would have been broken for IO callbacks anyway
              error cases in bmp and tga give messages and don't leak (Raymond Barbiero, grisha)
              fix inefficiency in decoding 32-bit BMP (David Woo)
      1.29  (2010-08-16)
              various warning fixes from Aurelien Pocheville
      1.28  (2010-08-01)
              fix bug in GIF palette transparency (SpartanJ)
      1.27  (2010-08-01)
              cast-to-stbi_uc to fix warnings
      1.26  (2010-07-24)
              fix bug in file buffering for PNG reported by SpartanJ
      1.25  (2010-07-17)
              refix trans_data warning (Won Chun)
      1.24  (2010-07-12)
              perf improvements reading from files on platforms with lock-heavy fgetc()
              minor perf improvements for jpeg
              deprecated type-specific functions so we'll get feedback if they're needed
              attempt to fix trans_data warning (Won Chun)
      1.23    fixed bug in iPhone support
      1.22  (2010-07-10)
              removed image *writing* support
              stbi_info support from Jetro Lauha
              GIF support from Jean-Marc Lienher
              iPhone PNG-extensions from James Brown
              warning-fixes from Nicolas Schulz and Janez Zemva (i.stbi__err. Janez (U+017D)emva)
      1.21    fix use of 'stbi_uc' in header (reported by jon blow)
      1.20    added support for Softimage PIC, by Tom Seddon
      1.19    bug in interlaced PNG corruption check (found by ryg)
      1.18  (2008-08-02)
              fix a threading bug (local mutable static)
      1.17    support interlaced PNG
      1.16    major bugfix - stbi__convert_format converted one too many pixels
      1.15    initialize some fields for thread safety
      1.14    fix threadsafe conversion bug
              header-file-only version (#define STBI_HEADER_FILE_ONLY before including)
      1.13    threadsafe
      1.12    const qualifiers in the API
      1.11    Support installable IDCT, colorspace conversion routines
      1.10    Fixes for 64-bit (don't use "unsigned long")
              optimized upsampling by Fabian "ryg" Giesen
      1.09    Fix format-conversion for PSD code (bad global variables!)
      1.08    Thatcher Ulrich's PSD code integrated by Nicolas Schulz
      1.07    attempt to fix C++ warning/errors again
      1.06    attempt to fix C++ warning/errors again
      1.05    fix TGA loading to return correct *comp and use good luminance calc
      1.04    default float alpha is 1, not 255; use 'void *' for stbi_image_free
      1.03    bugfixes to STBI_NO_STDIO, STBI_NO_HDR
      1.02    support for (subset of) HDR files, float interface for preferred access to them
      1.01    fix bug: possible bug in handling right-side up bmps... not sure
              fix bug: the stbi__bmp_load() and stbi__tga_load() functions didn't work at all
      1.00    interface to zlib that skips zlib header
      0.99    correct handling of alpha in palette
      0.98    TGA loader by lonesock; dynamically add loaders (untested)
      0.97    jpeg errors on too large a file; also catch another malloc failure
      0.96    fix detection of invalid v value - particleman@mollyrocket forum
      0.95    during header scan, seek to markers in case of padding
      0.94    STBI_NO_STDIO to disable stdio usage; rename all #defines the same
      0.93    handle jpegtran output; verbose errors
      0.92    read 4,8,16,24,32-bit BMP files of several formats
      0.91    output 24-bit Windows 3.0 BMP files
      0.90    fix a few more warnings; bump version number to approach 1.0
      0.61    bugfixes due to Marc LeBlanc, Christopher Lloyd
      0.60    fix compiling as c++
      0.59    fix warnings: merge Dave Moore's -Wall fixes
      0.58    fix bug: zlib uncompressed mode len/nlen was wrong endian
      0.57    fix bug: jpg last huffman symbol before marker was >9 bits but less than 16 available
      0.56    fix bug: zlib uncompressed mode len vs. nlen
      0.55    fix bug: restart_interval not initialized to 0
      0.54    allow NULL for 'int *comp'
      0.53    fix bug in png 3->4; speedup png decoding
      0.52    png handles req_comp=3,4 directly; minor cleanup; jpeg comments
      0.51    obey req_comp requests, 1-component jpegs return as 1-component,
              on 'test' only check type, not whether we support this variant
      0.50  (2006-11-19)
              first released version
*/


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp continued                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/portable_file_dialogs.h included by src/immvision/internal/image.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Portable File Dialogs
//
//  Copyright © 2018–2022 Sam Hocevar <sam@hocevar.net>
//
//  This library is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//


#if _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#endif
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h> // IFileDialog
#include <shellapi.h>
#include <strsafe.h>
#include <future>     // std::async
#include <userenv.h>  // GetUserProfileDirectory()

#elif __EMSCRIPTEN__
#include <emscripten.h>

#else
#ifndef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 2 // for popen()
#endif
#ifdef __APPLE__
#   ifndef _DARWIN_C_SOURCE
#       define _DARWIN_C_SOURCE
#   endif
#endif
#include <cstdio>     // popen()
#include <cstdlib>    // std::getenv()
#include <fcntl.h>    // fcntl()
#include <unistd.h>   // read(), pipe(), dup2(), getuid()
#include <csignal>    // ::kill, std::signal
#include <sys/stat.h> // stat()
#include <sys/wait.h> // waitpid()
#include <pwd.h>      // getpwnam()
#endif

#include <string>   // std::string
#include <memory>   // std::shared_ptr
#include <iostream> // std::ostream
#include <map>      // std::map
#include <set>      // std::set
#include <regex>    // std::regex
#include <thread>   // std::mutex, std::this_thread
#include <chrono>   // std::chrono

// Versions of mingw64 g++ up to 9.3.0 do not have a complete IFileDialog
#ifndef PFD_HAS_IFILEDIALOG
#   define PFD_HAS_IFILEDIALOG 1
#   if (defined __MINGW64__ || defined __MINGW32__) && defined __GXX_ABI_VERSION
#       if __GXX_ABI_VERSION <= 1013
#           undef PFD_HAS_IFILEDIALOG
#           define PFD_HAS_IFILEDIALOG 0
#       endif
#   endif
#endif

namespace pfd
{

    enum class button
    {
        cancel = -1,
        ok,
        yes,
        no,
        abort,
        retry,
        ignore,
    };

    enum class choice
    {
        ok = 0,
        ok_cancel,
        yes_no,
        yes_no_cancel,
        retry_cancel,
        abort_retry_ignore,
    };

    enum class icon
    {
        info = 0,
        warning,
        error,
        question,
    };

    // Additional option flags for various dialog constructors
    enum class opt : uint8_t
    {
        none = 0,
        // For file open, allow multiselect.
        multiselect = 0x1,
        // For file save, force overwrite and disable the confirmation dialog.
        force_overwrite = 0x2,
        // For folder select, force path to be the provided argument instead
        // of the last opened directory, which is the Microsoft-recommended,
        // user-friendly behaviour.
        force_path = 0x4,
    };

    inline opt operator |(opt a, opt b) { return opt(uint8_t(a) | uint8_t(b)); }
    inline bool operator &(opt a, opt b) { return bool(uint8_t(a) & uint8_t(b)); }

    // The settings class, only exposing to the user a way to set verbose mode
    // and to force a rescan of installed desktop helpers (zenity, kdialog…).
    class settings
    {
    public:
        static bool available();

        static void verbose(bool value);
        static void rescan();

    protected:
        explicit settings(bool resync = false);

        bool check_program(std::string const& program);

        inline bool is_osascript() const;
        inline bool is_zenity() const;
        inline bool is_kdialog() const;

        enum class flag
        {
            is_scanned = 0,
            is_verbose,

            has_zenity,
            has_matedialog,
            has_qarma,
            has_kdialog,
            is_vista,

            max_flag,
        };

        // Static array of flags for internal state
        bool const& flags(flag in_flag) const;

        // Non-const getter for the static array of flags
        bool& flags(flag in_flag);
    };

    // Internal classes, not to be used by client applications
    namespace internal
    {

        // Process wait timeout, in milliseconds
        static int const default_wait_timeout = 20;

        class executor
        {
            friend class dialog;

        public:
            // High level function to get the result of a command
            std::string result(int* exit_code = nullptr);

            // High level function to abort
            bool kill();

#if _WIN32
            void start_func(std::function<std::string(int*)> const& fun);
            static BOOL CALLBACK enum_windows_callback(HWND hwnd, LPARAM lParam);
#elif __EMSCRIPTEN__
            void start(int exit_code);
#else
            void start_process(std::vector<std::string> const& command);
#endif

            ~executor();

        protected:
            bool ready(int timeout = default_wait_timeout);
            void stop();

        private:
            bool m_running = false;
            std::string m_stdout;
            int m_exit_code = -1;
#if _WIN32
            std::future<std::string> m_future;
            std::set<HWND> m_windows;
            std::condition_variable m_cond;
            std::mutex m_mutex;
            DWORD m_tid;
#elif __EMSCRIPTEN__ || __NX__
            // FIXME: do something
#else
            pid_t m_pid = 0;
            int m_fd = -1;
#endif
        };

        class platform
        {
        protected:
#if _WIN32
            // Helper class around LoadLibraryA() and GetProcAddress() with some safety
            class dll
            {
            public:
                dll(std::string const& name);
                ~dll();

                template<typename T> class proc
                {
                public:
                    proc(dll const& lib, std::string const& sym)
                        : m_proc(reinterpret_cast<T*>((void*)::GetProcAddress(lib.handle, sym.c_str())))
                    {}

                    operator bool() const { return m_proc != nullptr; }
                    operator T* () const { return m_proc; }

                private:
                    T* m_proc;
                };

            private:
                HMODULE handle;
            };

            // Helper class around CoInitialize() and CoUnInitialize()
            class ole32_dll : public dll
            {
            public:
                ole32_dll();
                ~ole32_dll();
                bool is_initialized();

            private:
                HRESULT m_state;
            };

            // Helper class around CreateActCtx() and ActivateActCtx()
            class new_style_context
            {
            public:
                new_style_context();
                ~new_style_context();

            private:
                HANDLE create();
                ULONG_PTR m_cookie = 0;
            };
#endif
        };

        class dialog : protected settings, protected platform
        {
        public:
            bool ready(int timeout = default_wait_timeout) const;
            bool kill() const;
            virtual ~dialog() = default;

        protected:
            explicit dialog();

            std::vector<std::string> desktop_helper() const;
            static std::string buttons_to_name(choice _choice);
            static std::string get_icon_name(icon _icon);

            std::string powershell_quote(std::string const& str) const;
            std::string osascript_quote(std::string const& str) const;
            std::string shell_quote(std::string const& str) const;

            // Keep handle to executing command
            std::shared_ptr<executor> m_async;
        };

        class file_dialog : public dialog
        {
        protected:
            enum type
            {
                open,
                save,
                folder,
            };

            file_dialog(type in_type,
                std::string const& title,
                std::string const& default_path = "",
                std::vector<std::string> const& filters = {},
                opt options = opt::none);

        protected:
            std::string string_result();
            std::vector<std::string> vector_result();

#if _WIN32
            static int CALLBACK bffcallback(HWND hwnd, UINT uMsg, LPARAM, LPARAM pData);
#if PFD_HAS_IFILEDIALOG
            std::string select_folder_vista(IFileDialog* ifd, bool force_path);
#endif

            std::wstring m_wtitle;
            std::wstring m_wdefault_path;

            std::vector<std::string> m_vector_result;
#endif
        };

    } // namespace internal

    //
    // The path class provides some platform-specific path constants
    //

    class path : protected internal::platform
    {
    public:
        static std::string home();
        static std::string separator();
    };

    //
    // The notify widget
    //

    class notify : public internal::dialog
    {
    public:
        notify(std::string const& title,
            std::string const& message,
            icon _icon = icon::info);
    };

    //
    // The message widget
    //

    class message : public internal::dialog
    {
    public:
        message(std::string const& title,
            std::string const& text,
            choice _choice = choice::ok_cancel,
            icon _icon = icon::info);

        button result();

    private:
        // Some extra logic to map the exit code to button number
        std::map<int, button> m_mappings;
    };

    //
    // The open_file, save_file, and open_folder widgets
    //

    class open_file : public internal::file_dialog
    {
    public:
        open_file(std::string const& title,
            std::string const& default_path = "",
            std::vector<std::string> const& filters = { "All Files", "*" },
            opt options = opt::none);

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
        // Backwards compatibility
        [[deprecated("Use pfd::opt::multiselect instead of allow_multiselect")]]
#endif
#endif
        open_file(std::string const& title,
            std::string const& default_path,
            std::vector<std::string> const& filters,
            bool allow_multiselect);

        std::vector<std::string> result();
    };

    class save_file : public internal::file_dialog
    {
    public:
        save_file(std::string const& title,
            std::string const& default_path = "",
            std::vector<std::string> const& filters = { "All Files", "*" },
            opt options = opt::none);

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
        // Backwards compatibility
        [[deprecated("Use pfd::opt::force_overwrite instead of confirm_overwrite")]]
#endif
#endif
        save_file(std::string const& title,
            std::string const& default_path,
            std::vector<std::string> const& filters,
            bool confirm_overwrite);

        std::string result();
    };

    class select_folder : public internal::file_dialog
    {
    public:
        select_folder(std::string const& title,
            std::string const& default_path = "",
            opt options = opt::none);

        std::string result();
    };

    //
    // Below this are all the method implementations. You may choose to define the
    // macro PFD_SKIP_IMPLEMENTATION everywhere before including this header except
    // in one place. This may reduce compilation times.
    //

#if !defined PFD_SKIP_IMPLEMENTATION

// internal free functions implementations

    namespace internal
    {

#if _WIN32
        static inline std::wstring str2wstr(std::string const& str)
        {
            int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
            std::wstring ret(len, '\0');
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), (LPWSTR)ret.data(), (int)ret.size());
            return ret;
        }

        static inline std::string wstr2str(std::wstring const& str)
        {
            int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0, nullptr, nullptr);
            std::string ret(len, '\0');
            WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), (LPSTR)ret.data(), (int)ret.size(), nullptr, nullptr);
            return ret;
        }

        static inline bool is_vista()
        {
            OSVERSIONINFOEXW osvi;
            memset(&osvi, 0, sizeof(osvi));
            DWORDLONG const mask = VerSetConditionMask(
                VerSetConditionMask(
                    VerSetConditionMask(
                        0, VER_MAJORVERSION, VER_GREATER_EQUAL),
                    VER_MINORVERSION, VER_GREATER_EQUAL),
                VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
            osvi.dwOSVersionInfoSize = sizeof(osvi);
            osvi.dwMajorVersion = HIBYTE(_WIN32_WINNT_VISTA);
            osvi.dwMinorVersion = LOBYTE(_WIN32_WINNT_VISTA);
            osvi.wServicePackMajor = 0;

            return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, mask) != FALSE;
        }
#endif

        // This is necessary until C++20 which will have std::string::ends_with() etc.

        static inline bool ends_with(std::string const& str, std::string const& suffix)
        {
            return suffix.size() <= str.size() &&
                str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
        }

        static inline bool starts_with(std::string const& str, std::string const& prefix)
        {
            return prefix.size() <= str.size() &&
                str.compare(0, prefix.size(), prefix) == 0;
        }

        // This is necessary until C++17 which will have std::filesystem::is_directory

        static inline bool is_directory(std::string const& path)
        {
#if _WIN32
            auto attr = GetFileAttributesA(path.c_str());
            return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
#elif __EMSCRIPTEN__
            // TODO
            return false;
#else
            struct stat s;
            return stat(path.c_str(), &s) == 0 && S_ISDIR(s.st_mode);
#endif
        }

        // This is necessary because getenv is not thread-safe

        static inline std::string getenv(std::string const& str)
        {
#if _MSC_VER
            char* buf = nullptr;
            size_t size = 0;
            if (_dupenv_s(&buf, &size, str.c_str()) == 0 && buf)
            {
                std::string ret(buf);
                free(buf);
                return ret;
            }
            return "";
#else
            auto buf = std::getenv(str.c_str());
            return buf ? buf : "";
#endif
        }

    } // namespace internal

    // settings implementation

    inline settings::settings(bool resync)
    {
        flags(flag::is_scanned) &= !resync;

        if (flags(flag::is_scanned))
            return;

        auto pfd_verbose = internal::getenv("PFD_VERBOSE");
        auto match_no = std::regex("(|0|no|false)", std::regex_constants::icase);
        if (!std::regex_match(pfd_verbose, match_no))
            flags(flag::is_verbose) = true;

#if _WIN32
        flags(flag::is_vista) = internal::is_vista();
#elif !__APPLE__
        flags(flag::has_zenity) = check_program("zenity");
        flags(flag::has_matedialog) = check_program("matedialog");
        flags(flag::has_qarma) = check_program("qarma");
        flags(flag::has_kdialog) = check_program("kdialog");

        // If multiple helpers are available, try to default to the best one
        if (flags(flag::has_zenity) && flags(flag::has_kdialog))
        {
            auto desktop_name = internal::getenv("XDG_SESSION_DESKTOP");
            if (desktop_name == std::string("gnome"))
                flags(flag::has_kdialog) = false;
            else if (desktop_name == std::string("KDE"))
                flags(flag::has_zenity) = false;
        }
#endif

        flags(flag::is_scanned) = true;
    }

    inline bool settings::available()
    {
#if _WIN32
        return true;
#elif __APPLE__
        return true;
#elif __EMSCRIPTEN__
        // FIXME: Return true after implementation is complete.
        return false;
#else
        settings tmp;
        return tmp.flags(flag::has_zenity) ||
            tmp.flags(flag::has_matedialog) ||
            tmp.flags(flag::has_qarma) ||
            tmp.flags(flag::has_kdialog);
#endif
    }

    inline void settings::verbose(bool value)
    {
        settings().flags(flag::is_verbose) = value;
    }

    inline void settings::rescan()
    {
        settings(/* resync = */ true);
    }

    // Check whether a program is present using “which”.
    inline bool settings::check_program(std::string const& program)
    {
#if _WIN32
        (void)program;
        return false;
#elif __EMSCRIPTEN__
        (void)program;
        return false;
#else
        int exit_code = -1;
        internal::executor async;
        async.start_process({ "/bin/sh", "-c", "which " + program });
        async.result(&exit_code);
        return exit_code == 0;
#endif
    }

    inline bool settings::is_osascript() const
    {
#if __APPLE__
        return true;
#else
        return false;
#endif
    }

    inline bool settings::is_zenity() const
    {
        return flags(flag::has_zenity) ||
            flags(flag::has_matedialog) ||
            flags(flag::has_qarma);
    }

    inline bool settings::is_kdialog() const
    {
        return flags(flag::has_kdialog);
    }

    inline bool const& settings::flags(flag in_flag) const
    {
        static bool flags[size_t(flag::max_flag)];
        return flags[size_t(in_flag)];
    }

    inline bool& settings::flags(flag in_flag)
    {
        return const_cast<bool&>(static_cast<settings const*>(this)->flags(in_flag));
    }

    // path implementation
    inline std::string path::home()
    {
#if _WIN32
        // First try the USERPROFILE environment variable
        auto user_profile = internal::getenv("USERPROFILE");
        if (user_profile.size() > 0)
            return user_profile;
        // Otherwise, try GetUserProfileDirectory()
        HANDLE token = nullptr;
        DWORD len = MAX_PATH;
        char buf[MAX_PATH] = { '\0' };
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        {
            dll userenv("userenv.dll");
            dll::proc<BOOL WINAPI(HANDLE, LPSTR, LPDWORD)> get_user_profile_directory(userenv, "GetUserProfileDirectoryA");
            get_user_profile_directory(token, buf, &len);
            CloseHandle(token);
            if (*buf)
                return buf;
        }
#elif __EMSCRIPTEN__
        return "/";
#else
        // First try the HOME environment variable
        auto home = internal::getenv("HOME");
        if (home.size() > 0)
            return home;
        // Otherwise, try getpwuid_r()
        size_t len = 4096;
#if defined(_SC_GETPW_R_SIZE_MAX)
        auto size_max = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (size_max != -1)
            len = size_t(size_max);
#endif
        std::vector<char> buf(len);
        struct passwd pwd, * result;
        if (getpwuid_r(getuid(), &pwd, buf.data(), buf.size(), &result) == 0)
            return result->pw_dir;
#endif
        return "/";
    }

    inline std::string path::separator()
    {
#if _WIN32
        return "\\";
#else
        return "/";
#endif
    }

    // executor implementation

    inline std::string internal::executor::result(int* exit_code /* = nullptr */)
    {
        stop();
        if (exit_code)
            *exit_code = m_exit_code;
        return m_stdout;
    }

    inline bool internal::executor::kill()
    {
#if _WIN32
        if (m_future.valid())
        {
            // Close all windows that weren’t open when we started the future
            auto previous_windows = m_windows;
            EnumWindows(&enum_windows_callback, (LPARAM)this);
            for (auto hwnd : m_windows)
                if (previous_windows.find(hwnd) == previous_windows.end())
                {
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    // Also send IDNO in case of a Yes/No or Abort/Retry/Ignore messagebox
                    SendMessage(hwnd, WM_COMMAND, IDNO, 0);
                }
        }
#elif __EMSCRIPTEN__ || __NX__
        // FIXME: do something
        return false; // cannot kill
#else
        ::kill(m_pid, SIGKILL);
#endif
        stop();
        return true;
    }

#if _WIN32
    inline BOOL CALLBACK internal::executor::enum_windows_callback(HWND hwnd, LPARAM lParam)
    {
        auto that = (executor*)lParam;

        DWORD pid;
        auto tid = GetWindowThreadProcessId(hwnd, &pid);
        if (tid == that->m_tid)
            that->m_windows.insert(hwnd);
        return TRUE;
    }
#endif

#if _WIN32
    inline void internal::executor::start_func(std::function<std::string(int*)> const& fun)
    {
        stop();

        auto trampoline = [fun, this]()
        {
            // Save our thread id so that the caller can cancel us
            m_tid = GetCurrentThreadId();
            EnumWindows(&enum_windows_callback, (LPARAM)this);
            m_cond.notify_all();
            return fun(&m_exit_code);
        };

        std::unique_lock<std::mutex> lock(m_mutex);
        m_future = std::async(std::launch::async, trampoline);
        m_cond.wait(lock);
        m_running = true;
    }

#elif __EMSCRIPTEN__
    inline void internal::executor::start(int exit_code)
    {
        m_exit_code = exit_code;
    }

#else
    inline void internal::executor::start_process(std::vector<std::string> const& command)
    {
        stop();
        m_stdout.clear();
        m_exit_code = -1;

        int in[2], out[2];
        if (pipe(in) != 0 || pipe(out) != 0)
            return;

        m_pid = fork();
        if (m_pid < 0)
            return;

        close(in[m_pid ? 0 : 1]);
        close(out[m_pid ? 1 : 0]);

        if (m_pid == 0)
        {
            dup2(in[0], STDIN_FILENO);
            dup2(out[1], STDOUT_FILENO);

            // Ignore stderr so that it doesn’t pollute the console (e.g. GTK+ errors from zenity)
            int fd = open("/dev/null", O_WRONLY);
            dup2(fd, STDERR_FILENO);
            close(fd);

            std::vector<char*> args;
            std::transform(command.cbegin(), command.cend(), std::back_inserter(args),
                [](std::string const& s) { return const_cast<char*>(s.c_str()); });
            args.push_back(nullptr); // null-terminate argv[]

            execvp(args[0], args.data());
            exit(1);
        }

        close(in[1]);
        m_fd = out[0];
        auto flags = fcntl(m_fd, F_GETFL);
        fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);

        m_running = true;
    }
#endif

    inline internal::executor::~executor()
    {
        stop();
    }

    inline bool internal::executor::ready(int timeout /* = default_wait_timeout */)
    {
        if (!m_running)
            return true;

#if _WIN32
        if (m_future.valid())
        {
            auto status = m_future.wait_for(std::chrono::milliseconds(timeout));
            if (status != std::future_status::ready)
            {
                // On Windows, we need to run the message pump. If the async
                // thread uses a Windows API dialog, it may be attached to the
                // main thread and waiting for messages that only we can dispatch.
                MSG msg;
                while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                return false;
            }

            m_stdout = m_future.get();
        }
#elif __EMSCRIPTEN__ || __NX__
        // FIXME: do something
        (void)timeout;
#else
        char buf[BUFSIZ];
        ssize_t received = read(m_fd, buf, BUFSIZ); // Flawfinder: ignore
        if (received > 0)
        {
            m_stdout += std::string(buf, received);
            return false;
        }

        // Reap child process if it is dead. It is possible that the system has already reaped it
        // (this happens when the calling application handles or ignores SIG_CHLD) and results in
        // waitpid() failing with ECHILD. Otherwise we assume the child is running and we sleep for
        // a little while.
        int status;
        pid_t child = waitpid(m_pid, &status, WNOHANG);
        if (child != m_pid && (child >= 0 || errno != ECHILD))
        {
            // FIXME: this happens almost always at first iteration
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
            return false;
        }

        close(m_fd);
        m_exit_code = WEXITSTATUS(status);
#endif

        m_running = false;
        return true;
    }

    inline void internal::executor::stop()
    {
        // Loop until the user closes the dialog
        while (!ready())
            ;
    }

    // dll implementation

#if _WIN32
    inline internal::platform::dll::dll(std::string const& name)
        : handle(::LoadLibraryA(name.c_str()))
    {}

    inline internal::platform::dll::~dll()
    {
        if (handle)
            ::FreeLibrary(handle);
    }
#endif // _WIN32

    // ole32_dll implementation

#if _WIN32
    inline internal::platform::ole32_dll::ole32_dll()
        : dll("ole32.dll")
    {
        // Use COINIT_MULTITHREADED because COINIT_APARTMENTTHREADED causes crashes.
        // See https://github.com/samhocevar/portable-file-dialogs/issues/51
        auto coinit = proc<HRESULT WINAPI(LPVOID, DWORD)>(*this, "CoInitializeEx");
        m_state = coinit(nullptr, COINIT_MULTITHREADED);
    }

    inline internal::platform::ole32_dll::~ole32_dll()
    {
        if (is_initialized())
            proc<void WINAPI()>(*this, "CoUninitialize")();
    }

    inline bool internal::platform::ole32_dll::is_initialized()
    {
        return m_state == S_OK || m_state == S_FALSE;
    }
#endif

    // new_style_context implementation

#if _WIN32
    inline internal::platform::new_style_context::new_style_context()
    {
        // Only create one activation context for the whole app lifetime.
        static HANDLE hctx = create();

        if (hctx != INVALID_HANDLE_VALUE)
            ActivateActCtx(hctx, &m_cookie);
    }

    inline internal::platform::new_style_context::~new_style_context()
    {
        DeactivateActCtx(0, m_cookie);
    }

    inline HANDLE internal::platform::new_style_context::create()
    {
        // This “hack” seems to be necessary for this code to work on windows XP.
        // Without it, dialogs do not show and close immediately. GetError()
        // returns 0 so I don’t know what causes this. I was not able to reproduce
        // this behavior on Windows 7 and 10 but just in case, let it be here for
        // those versions too.
        // This hack is not required if other dialogs are used (they load comdlg32
        // automatically), only if message boxes are used.
        dll comdlg32("comdlg32.dll");

        // Using approach as shown here: https://stackoverflow.com/a/10444161
        UINT len = ::GetSystemDirectoryA(nullptr, 0);
        std::string sys_dir(len, '\0');
        ::GetSystemDirectoryA(&sys_dir[0], len);

        ACTCTXA act_ctx =
        {
            // Do not set flag ACTCTX_FLAG_SET_PROCESS_DEFAULT, since it causes a
            // crash with error “default context is already set”.
            sizeof(act_ctx),
            ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID,
            "shell32.dll", 0, 0, sys_dir.c_str(), (LPCSTR)124, nullptr, 0,
        };

        return ::CreateActCtxA(&act_ctx);
    }
#endif // _WIN32

    // dialog implementation

    inline bool internal::dialog::ready(int timeout /* = default_wait_timeout */) const
    {
        return m_async->ready(timeout);
    }

    inline bool internal::dialog::kill() const
    {
        return m_async->kill();
    }

    inline internal::dialog::dialog()
        : m_async(std::make_shared<executor>())
    {
    }

    inline std::vector<std::string> internal::dialog::desktop_helper() const
    {
#if __APPLE__
        return { "osascript" };
#else
        return { flags(flag::has_zenity) ? "zenity"
               : flags(flag::has_matedialog) ? "matedialog"
               : flags(flag::has_qarma) ? "qarma"
               : flags(flag::has_kdialog) ? "kdialog"
               : "echo" };
#endif
    }

    inline std::string internal::dialog::buttons_to_name(choice _choice)
    {
        switch (_choice)
        {
        case choice::ok_cancel: return "okcancel";
        case choice::yes_no: return "yesno";
        case choice::yes_no_cancel: return "yesnocancel";
        case choice::retry_cancel: return "retrycancel";
        case choice::abort_retry_ignore: return "abortretryignore";
        /* case choice::ok: */ default: return "ok";
        }
    }

    inline std::string internal::dialog::get_icon_name(icon _icon)
    {
        switch (_icon)
        {
        case icon::warning: return "warning";
        case icon::error: return "error";
        case icon::question: return "question";
            // Zenity wants "information" but WinForms wants "info"
        /* case icon::info: */ default:
#if _WIN32
            return "info";
#else
            return "information";
#endif
        }
    }

    // This is only used for debugging purposes
    inline std::ostream& operator <<(std::ostream& s, std::vector<std::string> const& v)
    {
        int not_first = 0;
        for (auto& e : v)
            s << (not_first++ ? " " : "") << e;
        return s;
    }

    // Properly quote a string for Powershell: replace ' or " with '' or ""
    // FIXME: we should probably get rid of newlines!
    // FIXME: the \" sequence seems unsafe, too!
    // XXX: this is no longer used but I would like to keep it around just in case
    inline std::string internal::dialog::powershell_quote(std::string const& str) const
    {
        return "'" + std::regex_replace(str, std::regex("['\"]"), "$&$&") + "'";
    }

    // Properly quote a string for osascript: replace \ or " with \\ or \"
    // XXX: this also used to replace ' with \' when popen was used, but it would be
    // smarter to do shell_quote(osascript_quote(...)) if this is needed again.
    inline std::string internal::dialog::osascript_quote(std::string const& str) const
    {
        return "\"" + std::regex_replace(str, std::regex("[\\\\\"]"), "\\$&") + "\"";
    }

    // Properly quote a string for the shell: just replace ' with '\''
    // XXX: this is no longer used but I would like to keep it around just in case
    inline std::string internal::dialog::shell_quote(std::string const& str) const
    {
        return "'" + std::regex_replace(str, std::regex("'"), "'\\''") + "'";
    }

    // file_dialog implementation

    inline internal::file_dialog::file_dialog(type in_type,
        std::string const& title,
        std::string const& default_path /* = "" */,
        std::vector<std::string> const& filters /* = {} */,
        opt options /* = opt::none */)
    {
#if _WIN32
        std::string filter_list;
        std::regex whitespace("  *");
        for (size_t i = 0; i + 1 < filters.size(); i += 2)
        {
            filter_list += filters[i] + '\0';
            filter_list += std::regex_replace(filters[i + 1], whitespace, ";") + '\0';
        }
        filter_list += '\0';

        m_async->start_func([this, in_type, title, default_path, filter_list,
            options](int* exit_code) -> std::string
            {
                (void)exit_code;
        m_wtitle = internal::str2wstr(title);
        m_wdefault_path = internal::str2wstr(default_path);
        auto wfilter_list = internal::str2wstr(filter_list);

        // Initialise COM. This is required for the new folder selection window,
        // (see https://github.com/samhocevar/portable-file-dialogs/pull/21)
        // and to avoid random crashes with GetOpenFileNameW() (see
        // https://github.com/samhocevar/portable-file-dialogs/issues/51)
        ole32_dll ole32;

        // Folder selection uses a different method
        if (in_type == type::folder)
        {
#if PFD_HAS_IFILEDIALOG
            if (flags(flag::is_vista))
            {
                // On Vista and higher we should be able to use IFileDialog for folder selection
                IFileDialog* ifd;
                HRESULT hr = dll::proc<HRESULT WINAPI(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*)>(ole32, "CoCreateInstance")
                    (CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ifd));

                // In case CoCreateInstance fails (which it should not), try legacy approach
                if (SUCCEEDED(hr))
                    return select_folder_vista(ifd, options & opt::force_path);
            }
#endif

            BROWSEINFOW bi;
            memset(&bi, 0, sizeof(bi));

            bi.lpfn = &bffcallback;
            bi.lParam = (LPARAM)this;

            if (flags(flag::is_vista))
            {
                if (ole32.is_initialized())
                    bi.ulFlags |= BIF_NEWDIALOGSTYLE;
                bi.ulFlags |= BIF_EDITBOX;
                bi.ulFlags |= BIF_STATUSTEXT;
            }

            auto* list = SHBrowseForFolderW(&bi);
            std::string ret;
            if (list)
            {
                auto buffer = new wchar_t[MAX_PATH];
                SHGetPathFromIDListW(list, buffer);
                dll::proc<void WINAPI(LPVOID)>(ole32, "CoTaskMemFree")(list);
                ret = internal::wstr2str(buffer);
                delete[] buffer;
            }
            return ret;
        }

        OPENFILENAMEW ofn;
        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.hwndOwner = GetActiveWindow();

        ofn.lpstrFilter = wfilter_list.c_str();

        auto woutput = std::wstring(MAX_PATH * 256, L'\0');
        ofn.lpstrFile = (LPWSTR)woutput.data();
        ofn.nMaxFile = (DWORD)woutput.size();
        if (!m_wdefault_path.empty())
        {
            // If a directory was provided, use it as the initial directory. If
            // a valid path was provided, use it as the initial file. Otherwise,
            // let the Windows API decide.
            auto path_attr = GetFileAttributesW(m_wdefault_path.c_str());
            if (path_attr != INVALID_FILE_ATTRIBUTES && (path_attr & FILE_ATTRIBUTE_DIRECTORY))
                ofn.lpstrInitialDir = m_wdefault_path.c_str();
            else if (m_wdefault_path.size() <= woutput.size())
                //second argument is size of buffer, not length of string
                StringCchCopyW(ofn.lpstrFile, MAX_PATH * 256 + 1, m_wdefault_path.c_str());
            else
            {
                ofn.lpstrFileTitle = (LPWSTR)m_wdefault_path.data();
                ofn.nMaxFileTitle = (DWORD)m_wdefault_path.size();
            }
        }
        ofn.lpstrTitle = m_wtitle.c_str();
        ofn.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER;

        dll comdlg32("comdlg32.dll");

        // Apply new visual style (required for windows XP)
        new_style_context ctx;

        if (in_type == type::save)
        {
            if (!(options & opt::force_overwrite))
                ofn.Flags |= OFN_OVERWRITEPROMPT;

            dll::proc<BOOL WINAPI(LPOPENFILENAMEW)> get_save_file_name(comdlg32, "GetSaveFileNameW");
            if (get_save_file_name(&ofn) == 0)
                return "";
            return internal::wstr2str(woutput.c_str());
        }
        else
        {
            if (options & opt::multiselect)
                ofn.Flags |= OFN_ALLOWMULTISELECT;
            ofn.Flags |= OFN_PATHMUSTEXIST;

            dll::proc<BOOL WINAPI(LPOPENFILENAMEW)> get_open_file_name(comdlg32, "GetOpenFileNameW");
            if (get_open_file_name(&ofn) == 0)
                return "";
        }

        std::string prefix;
        for (wchar_t const* p = woutput.c_str(); *p; )
        {
            auto filename = internal::wstr2str(p);
            p += wcslen(p);
            // In multiselect mode, we advance p one wchar further and
            // check for another filename. If there is one and the
            // prefix is empty, it means we just read the prefix.
            if ((options & opt::multiselect) && *++p && prefix.empty())
            {
                prefix = filename + "/";
                continue;
            }

            m_vector_result.push_back(prefix + filename);
        }

        return "";
            });
#elif __EMSCRIPTEN__
        // FIXME: do something
        (void)in_type;
        (void)title;
        (void)default_path;
        (void)filters;
        (void)options;
#else
        auto command = desktop_helper();

        if (is_osascript())
        {
            std::string script = "set ret to choose";
            switch (in_type)
            {
            case type::save:
                script += " file name";
                break;
            case type::open: default:
                script += " file";
                if (options & opt::multiselect)
                    script += " with multiple selections allowed";
                break;
            case type::folder:
                script += " folder";
                break;
            }

            if (default_path.size())
            {
                if (in_type == type::folder || is_directory(default_path))
                    script += " default location ";
                else
                    script += " default name ";
                script += osascript_quote(default_path);
            }

            script += " with prompt " + osascript_quote(title);

            if (in_type == type::open)
            {
                // Concatenate all user-provided filter patterns
                std::string patterns;
                for (size_t i = 0; i < filters.size() / 2; ++i)
                    patterns += " " + filters[2 * i + 1];

                // Split the pattern list to check whether "*" is in there; if it
                // is, we have to disable filters because there is no mechanism in
                // OS X for the user to override the filter.
                std::regex sep("\\s+");
                std::string filter_list;
                bool has_filter = true;
                std::sregex_token_iterator iter(patterns.begin(), patterns.end(), sep, -1);
                std::sregex_token_iterator end;
                for (; iter != end; ++iter)
                {
                    auto pat = iter->str();
                    if (pat == "*" || pat == "*.*")
                        has_filter = false;
                    else if (internal::starts_with(pat, "*."))
                        filter_list += "," + osascript_quote(pat.substr(2, pat.size() - 2));
                }

                if (has_filter && filter_list.size() > 0)
                {
                    // There is a weird AppleScript bug where file extensions of length != 3 are
                    // ignored, e.g. type{"txt"} works, but type{"json"} does not. Fortunately if
                    // the whole list starts with a 3-character extension, everything works again.
                    // We use "///" for such an extension because we are sure it cannot appear in
                    // an actual filename.
                    script += " of type {\"///\"" + filter_list + "}";
                }
            }

            if (in_type == type::open && (options & opt::multiselect))
            {
                script += "\nset s to \"\"";
                script += "\nrepeat with i in ret";
                script += "\n  set s to s & (POSIX path of i) & \"\\n\"";
                script += "\nend repeat";
                script += "\ncopy s to stdout";
            }
            else
            {
                script += "\nPOSIX path of ret";
            }

            command.push_back("-e");
            command.push_back(script);
        }
        else if (is_zenity())
        {
            command.push_back("--file-selection");

            // If the default path is a directory, make sure it ends with "/" otherwise zenity will
            // open the file dialog in the parent directory.
            auto filename_arg = "--filename=" + default_path;
            if (in_type != type::folder && !ends_with(default_path, "/") && internal::is_directory(default_path))
                filename_arg += "/";
            command.push_back(filename_arg);

            command.push_back("--title");
            command.push_back(title);
            command.push_back("--separator=\n");

            for (size_t i = 0; i < filters.size() / 2; ++i)
            {
                command.push_back("--file-filter");
                command.push_back(filters[2 * i] + "|" + filters[2 * i + 1]);
            }

            if (in_type == type::save)
                command.push_back("--save");
            if (in_type == type::folder)
                command.push_back("--directory");
            if (!(options & opt::force_overwrite))
                command.push_back("--confirm-overwrite");
            if (options & opt::multiselect)
                command.push_back("--multiple");
        }
        else if (is_kdialog())
        {
            switch (in_type)
            {
            case type::save: command.push_back("--getsavefilename"); break;
            case type::open: command.push_back("--getopenfilename"); break;
            case type::folder: command.push_back("--getexistingdirectory"); break;
            }
            if (options & opt::multiselect)
            {
                command.push_back("--multiple");
                command.push_back("--separate-output");
            }

            command.push_back(default_path);

            std::string filter;
            for (size_t i = 0; i < filters.size() / 2; ++i)
                filter += (i == 0 ? "" : " | ") + filters[2 * i] + "(" + filters[2 * i + 1] + ")";
            command.push_back(filter);

            command.push_back("--title");
            command.push_back(title);
        }

        if (flags(flag::is_verbose))
            std::cerr << "pfd: " << command << std::endl;

        m_async->start_process(command);
#endif
    }

    inline std::string internal::file_dialog::string_result()
    {
#if _WIN32
        return m_async->result();
#else
        auto ret = m_async->result();
        // Strip potential trailing newline (zenity). Also strip trailing slash
        // added by osascript for consistency with other backends.
        while (!ret.empty() && (ret.back() == '\n' || ret.back() == '/'))
            ret.pop_back();
        return ret;
#endif
    }

    inline std::vector<std::string> internal::file_dialog::vector_result()
    {
#if _WIN32
        m_async->result();
        return m_vector_result;
#else
        std::vector<std::string> ret;
        auto result = m_async->result();
        for (;;)
        {
            // Split result along newline characters
            auto i = result.find('\n');
            if (i == 0 || i == std::string::npos)
                break;
            ret.push_back(result.substr(0, i));
            result = result.substr(i + 1, result.size());
        }
        return ret;
#endif
    }

#if _WIN32
    // Use a static function to pass as BFFCALLBACK for legacy folder select
    inline int CALLBACK internal::file_dialog::bffcallback(HWND hwnd, UINT uMsg,
        LPARAM, LPARAM pData)
    {
        auto inst = (file_dialog*)pData;
        switch (uMsg)
        {
        case BFFM_INITIALIZED:
            SendMessage(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)inst->m_wdefault_path.c_str());
            break;
        }
        return 0;
    }

#if PFD_HAS_IFILEDIALOG
    inline std::string internal::file_dialog::select_folder_vista(IFileDialog* ifd, bool force_path)
    {
        std::string result;

        IShellItem* folder;

        // Load library at runtime so app doesn't link it at load time (which will fail on windows XP)
        dll shell32("shell32.dll");
        dll::proc<HRESULT WINAPI(PCWSTR, IBindCtx*, REFIID, void**)>
            create_item(shell32, "SHCreateItemFromParsingName");

        if (!create_item)
            return "";

        auto hr = create_item(m_wdefault_path.c_str(),
            nullptr,
            IID_PPV_ARGS(&folder));

        // Set default folder if found. This only sets the default folder. If
        // Windows has any info about the most recently selected folder, it
        // will display it instead. Generally, calling SetFolder() to set the
        // current directory “is not a good or expected user experience and
        // should therefore be avoided”:
        // https://docs.microsoft.com/windows/win32/api/shobjidl_core/nf-shobjidl_core-ifiledialog-setfolder
        if (SUCCEEDED(hr))
        {
            if (force_path)
                ifd->SetFolder(folder);
            else
                ifd->SetDefaultFolder(folder);
            folder->Release();
        }

        // Set the dialog title and option to select folders
        ifd->SetOptions(FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
        ifd->SetTitle(m_wtitle.c_str());

        hr = ifd->Show(GetActiveWindow());
        if (SUCCEEDED(hr))
        {
            IShellItem* item;
            hr = ifd->GetResult(&item);
            if (SUCCEEDED(hr))
            {
                wchar_t* wname = nullptr;
                // This is unlikely to fail because we use FOS_FORCEFILESYSTEM, but try
                // to output a debug message just in case.
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &wname)))
                {
                    result = internal::wstr2str(std::wstring(wname));
                    dll::proc<void WINAPI(LPVOID)>(ole32_dll(), "CoTaskMemFree")(wname);
                }
                else
                {
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &wname)))
                    {
                        auto name = internal::wstr2str(std::wstring(wname));
                        dll::proc<void WINAPI(LPVOID)>(ole32_dll(), "CoTaskMemFree")(wname);
                        std::cerr << "pfd: failed to get path for " << name << std::endl;
                    }
                    else
                        std::cerr << "pfd: item of unknown type selected" << std::endl;
                }

                item->Release();
            }
        }

        ifd->Release();

        return result;
    }
#endif
#endif

    // notify implementation

    inline notify::notify(std::string const& title,
        std::string const& message,
        icon _icon /* = icon::info */)
    {
        if (_icon == icon::question) // Not supported by notifications
            _icon = icon::info;

#if _WIN32
        // Use a static shared pointer for notify_icon so that we can delete
        // it whenever we need to display a new one, and we can also wait
        // until the program has finished running.
        struct notify_icon_data : public NOTIFYICONDATAW
        {
            ~notify_icon_data() { Shell_NotifyIconW(NIM_DELETE, this); }
        };

        static std::shared_ptr<notify_icon_data> nid;

        // Release the previous notification icon, if any, and allocate a new
        // one. Note that std::make_shared() does value initialization, so there
        // is no need to memset the structure.
        nid = nullptr;
        nid = std::make_shared<notify_icon_data>();

        // For XP support
        nid->cbSize = NOTIFYICONDATAW_V2_SIZE;
        nid->hWnd = nullptr;
        nid->uID = 0;

        // Flag Description:
        // - NIF_ICON    The hIcon member is valid.
        // - NIF_MESSAGE The uCallbackMessage member is valid.
        // - NIF_TIP     The szTip member is valid.
        // - NIF_STATE   The dwState and dwStateMask members are valid.
        // - NIF_INFO    Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.
        // - NIF_GUID    Reserved.
        nid->uFlags = NIF_MESSAGE | NIF_ICON | NIF_INFO;

        // Flag Description
        // - NIIF_ERROR     An error icon.
        // - NIIF_INFO      An information icon.
        // - NIIF_NONE      No icon.
        // - NIIF_WARNING   A warning icon.
        // - NIIF_ICON_MASK Version 6.0. Reserved.
        // - NIIF_NOSOUND   Version 6.0. Do not play the associated sound. Applies only to balloon ToolTips
        switch (_icon)
        {
        case icon::warning: nid->dwInfoFlags = NIIF_WARNING; break;
        case icon::error: nid->dwInfoFlags = NIIF_ERROR; break;
        /* case icon::info: */ default: nid->dwInfoFlags = NIIF_INFO; break;
        }

        ENUMRESNAMEPROC icon_enum_callback = [](HMODULE, LPCTSTR, LPTSTR lpName, LONG_PTR lParam) -> BOOL
        {
            ((NOTIFYICONDATAW*)lParam)->hIcon = ::LoadIcon(GetModuleHandle(nullptr), lpName);
            return false;
        };

        nid->hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
        ::EnumResourceNames(nullptr, RT_GROUP_ICON, icon_enum_callback, (LONG_PTR)nid.get());

        nid->uTimeout = 5000;

        StringCchCopyW(nid->szInfoTitle, ARRAYSIZE(nid->szInfoTitle), internal::str2wstr(title).c_str());
        StringCchCopyW(nid->szInfo, ARRAYSIZE(nid->szInfo), internal::str2wstr(message).c_str());

        // Display the new icon
        Shell_NotifyIconW(NIM_ADD, nid.get());
#elif __EMSCRIPTEN__
        // FIXME: do something
        (void)title;
        (void)message;
#else
        auto command = desktop_helper();

        if (is_osascript())
        {
            command.push_back("-e");
            command.push_back("display notification " + osascript_quote(message) +
                " with title " + osascript_quote(title));
        }
        else if (is_zenity())
        {
            command.push_back("--notification");
            command.push_back("--window-icon");
            command.push_back(get_icon_name(_icon));
            command.push_back("--text");
            command.push_back(title + "\n" + message);
        }
        else if (is_kdialog())
        {
            command.push_back("--icon");
            command.push_back(get_icon_name(_icon));
            command.push_back("--title");
            command.push_back(title);
            command.push_back("--passivepopup");
            command.push_back(message);
            command.push_back("5");
        }

        if (flags(flag::is_verbose))
            std::cerr << "pfd: " << command << std::endl;

        m_async->start_process(command);
#endif
    }

    // message implementation

    inline message::message(std::string const& title,
        std::string const& text,
        choice _choice /* = choice::ok_cancel */,
        icon _icon /* = icon::info */)
    {
#if _WIN32
        // Use MB_SYSTEMMODAL rather than MB_TOPMOST to ensure the message window is brought
        // to front. See https://github.com/samhocevar/portable-file-dialogs/issues/52
        UINT style = MB_SYSTEMMODAL;
        switch (_icon)
        {
        case icon::warning: style |= MB_ICONWARNING; break;
        case icon::error: style |= MB_ICONERROR; break;
        case icon::question: style |= MB_ICONQUESTION; break;
        /* case icon::info: */ default: style |= MB_ICONINFORMATION; break;
        }

        switch (_choice)
        {
        case choice::ok_cancel: style |= MB_OKCANCEL; break;
        case choice::yes_no: style |= MB_YESNO; break;
        case choice::yes_no_cancel: style |= MB_YESNOCANCEL; break;
        case choice::retry_cancel: style |= MB_RETRYCANCEL; break;
        case choice::abort_retry_ignore: style |= MB_ABORTRETRYIGNORE; break;
        /* case choice::ok: */ default: style |= MB_OK; break;
        }

        m_mappings[IDCANCEL] = button::cancel;
        m_mappings[IDOK] = button::ok;
        m_mappings[IDYES] = button::yes;
        m_mappings[IDNO] = button::no;
        m_mappings[IDABORT] = button::abort;
        m_mappings[IDRETRY] = button::retry;
        m_mappings[IDIGNORE] = button::ignore;

        m_async->start_func([text, title, style](int* exit_code) -> std::string
            {
                auto wtext = internal::str2wstr(text);
        auto wtitle = internal::str2wstr(title);
        // Apply new visual style (required for all Windows versions)
        new_style_context ctx;
        *exit_code = MessageBoxW(GetActiveWindow(), wtext.c_str(), wtitle.c_str(), style);
        return "";
            });

#elif __EMSCRIPTEN__
        std::string full_message;
        switch (_icon)
        {
        case icon::warning: full_message = "⚠️"; break;
        case icon::error: full_message = "⛔"; break;
        case icon::question: full_message = "❓"; break;
        /* case icon::info: */ default: full_message = "ℹ"; break;
        }

        full_message += ' ' + title + "\n\n" + text;

        // This does not really start an async task; it just passes the
        // EM_ASM_INT return value to a fake start() function.
        m_async->start(EM_ASM_INT(
            {
                if ($1)
                    return window.confirm(UTF8ToString($0)) ? 0 : -1;
                alert(UTF8ToString($0));
                return 0;
            }, full_message.c_str(), _choice == choice::ok_cancel));
#else
        auto command = desktop_helper();

        if (is_osascript())
        {
            std::string script = "display dialog " + osascript_quote(text) +
                " with title " + osascript_quote(title);
            auto if_cancel = button::cancel;
            switch (_choice)
            {
            case choice::ok_cancel:
                script += "buttons {\"OK\", \"Cancel\"}"
                    " default button \"OK\""
                    " cancel button \"Cancel\"";
                break;
            case choice::yes_no:
                script += "buttons {\"Yes\", \"No\"}"
                    " default button \"Yes\""
                    " cancel button \"No\"";
                if_cancel = button::no;
                break;
            case choice::yes_no_cancel:
                script += "buttons {\"Yes\", \"No\", \"Cancel\"}"
                    " default button \"Yes\""
                    " cancel button \"Cancel\"";
                break;
            case choice::retry_cancel:
                script += "buttons {\"Retry\", \"Cancel\"}"
                    " default button \"Retry\""
                    " cancel button \"Cancel\"";
                break;
            case choice::abort_retry_ignore:
                script += "buttons {\"Abort\", \"Retry\", \"Ignore\"}"
                    " default button \"Abort\""
                    " cancel button \"Retry\"";
                if_cancel = button::retry;
                break;
            case choice::ok: default:
                script += "buttons {\"OK\"}"
                    " default button \"OK\""
                    " cancel button \"OK\"";
                if_cancel = button::ok;
                break;
            }
            m_mappings[1] = if_cancel;
            m_mappings[256] = if_cancel; // XXX: I think this was never correct
            script += " with icon ";
            switch (_icon)
            {
#define PFD_OSX_ICON(n) "alias ((path to library folder from system domain) as text " \
                "& \"CoreServices:CoreTypes.bundle:Contents:Resources:" n ".icns\")"
            case icon::info: default: script += PFD_OSX_ICON("ToolBarInfo"); break;
            case icon::warning: script += "caution"; break;
            case icon::error: script += "stop"; break;
            case icon::question: script += PFD_OSX_ICON("GenericQuestionMarkIcon"); break;
#undef PFD_OSX_ICON
            }

            command.push_back("-e");
            command.push_back(script);
        }
        else if (is_zenity())
        {
            switch (_choice)
            {
            case choice::ok_cancel:
                command.insert(command.end(), { "--question", "--cancel-label=Cancel", "--ok-label=OK" }); break;
            case choice::yes_no:
                // Do not use standard --question because it causes “No” to return -1,
                // which is inconsistent with the “Yes/No/Cancel” mode below.
                command.insert(command.end(), { "--question", "--switch", "--extra-button=No", "--extra-button=Yes" }); break;
            case choice::yes_no_cancel:
                command.insert(command.end(), { "--question", "--switch", "--extra-button=Cancel", "--extra-button=No", "--extra-button=Yes" }); break;
            case choice::retry_cancel:
                command.insert(command.end(), { "--question", "--switch", "--extra-button=Cancel", "--extra-button=Retry" }); break;
            case choice::abort_retry_ignore:
                command.insert(command.end(), { "--question", "--switch", "--extra-button=Ignore", "--extra-button=Abort", "--extra-button=Retry" }); break;
            case choice::ok:
            default:
                switch (_icon)
                {
                case icon::error: command.push_back("--error"); break;
                case icon::warning: command.push_back("--warning"); break;
                default: command.push_back("--info"); break;
                }
            }

            command.insert(command.end(), { "--title", title,
                                            "--width=300", "--height=0", // sensible defaults
                                            "--no-markup", // do not interpret text as Pango markup
                                            "--text", text,
                                            "--icon-name=dialog-" + get_icon_name(_icon) });
        }
        else if (is_kdialog())
        {
            if (_choice == choice::ok)
            {
                switch (_icon)
                {
                case icon::error: command.push_back("--error"); break;
                case icon::warning: command.push_back("--sorry"); break;
                default: command.push_back("--msgbox"); break;
                }
            }
            else
            {
                std::string flag = "--";
                if (_icon == icon::warning || _icon == icon::error)
                    flag += "warning";
                flag += "yesno";
                if (_choice == choice::yes_no_cancel)
                    flag += "cancel";
                command.push_back(flag);
                if (_choice == choice::yes_no || _choice == choice::yes_no_cancel)
                {
                    m_mappings[0] = button::yes;
                    m_mappings[256] = button::no;
                }
            }

            command.push_back(text);
            command.push_back("--title");
            command.push_back(title);

            // Must be after the above part
            if (_choice == choice::ok_cancel)
                command.insert(command.end(), { "--yes-label", "OK", "--no-label", "Cancel" });
        }

        if (flags(flag::is_verbose))
            std::cerr << "pfd: " << command << std::endl;

        m_async->start_process(command);
#endif
    }

    inline button message::result()
    {
        int exit_code;
        auto ret = m_async->result(&exit_code);
        // osascript will say "button returned:Cancel\n"
        // and others will just say "Cancel\n"
        if (internal::ends_with(ret, "Cancel\n"))
            return button::cancel;
        if (internal::ends_with(ret, "OK\n"))
            return button::ok;
        if (internal::ends_with(ret, "Yes\n"))
            return button::yes;
        if (internal::ends_with(ret, "No\n"))
            return button::no;
        if (internal::ends_with(ret, "Abort\n"))
            return button::abort;
        if (internal::ends_with(ret, "Retry\n"))
            return button::retry;
        if (internal::ends_with(ret, "Ignore\n"))
            return button::ignore;
        if (m_mappings.count(exit_code) != 0)
            return m_mappings[exit_code];
        return exit_code == 0 ? button::ok : button::cancel;
    }

    // open_file implementation

    inline open_file::open_file(std::string const& title,
        std::string const& default_path /* = "" */,
        std::vector<std::string> const& filters /* = { "All Files", "*" } */,
        opt options /* = opt::none */)
        : file_dialog(type::open, title, default_path, filters, options)
    {
    }

    inline open_file::open_file(std::string const& title,
        std::string const& default_path,
        std::vector<std::string> const& filters,
        bool allow_multiselect)
        : open_file(title, default_path, filters,
            (allow_multiselect ? opt::multiselect : opt::none))
    {
    }

    inline std::vector<std::string> open_file::result()
    {
        return vector_result();
    }

    // save_file implementation

    inline save_file::save_file(std::string const& title,
        std::string const& default_path /* = "" */,
        std::vector<std::string> const& filters /* = { "All Files", "*" } */,
        opt options /* = opt::none */)
        : file_dialog(type::save, title, default_path, filters, options)
    {
    }

    inline save_file::save_file(std::string const& title,
        std::string const& default_path,
        std::vector<std::string> const& filters,
        bool confirm_overwrite)
        : save_file(title, default_path, filters,
            (confirm_overwrite ? opt::none : opt::force_overwrite))
    {
    }

    inline std::string save_file::result()
    {
        return string_result();
    }

    // select_folder implementation

    inline select_folder::select_folder(std::string const& title,
        std::string const& default_path /* = "" */,
        opt options /* = opt::none */)
        : file_dialog(type::folder, title, default_path, {}, options)
    {
    }

    inline std::string select_folder::result()
    {
        return string_result();
    }

#endif // PFD_SKIP_IMPLEMENTATION

} // namespace pfd

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp continued                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/imgui/image_widgets.h included by src/immvision/internal/image.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace ImageWidgets
    {
        Point2d DisplayTexture_TrackMouse(const GlTexture& texture, ImVec2 displaySize, bool disableDragWindow);
        void ShowImageInfo(const ImageBuffer &image, double zoomFactor);
        void ShowPixelColorWidget(const ImageBuffer &image, Point pt, const ImageParams& params);

        // If true, the collapsing headers will be synced across instances
        extern bool s_CollapsingHeader_CacheState_Sync;
        bool CollapsingHeader_OptionalCacheState(const char *name, bool forceOpen = false);
    } // namespace ImageWidgets

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp continued                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image_cache.h included by src/immvision/internal/image.cpp      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace ImageCache
    {
        using KeyType = ImGuiID;

        class ImageTextureCache
        {
        public:
            // members
            struct CachedParams
            {
                // These caches are small and will persist during the application lifetime
                ImageParams* ParamsPtr = nullptr;
                ImVec2 LastDragDelta;
                std::vector<char> FilenameEditBuffer = std::vector<char>(1000, '\0');
                bool   IsMouseDragging = false;
                bool   WasZoomJustUpdatedByLink = false;
                bool   IsResizing = false;
                Size PreviousImageSize;
                struct ImageParams  PreviousParams;
            };
            struct CachedImageAndTexture
            {
                // These caches are heavy and will be destroyed
                // if not used (after about 5 seconds)
                ImageBuffer     mImageRgbaCache;             // Image with applied colormap, alpha grid & paper background
                std::unique_ptr<GlTexture> mGlTexture;
            };

            // returns true if new entry
            KeyType GetID(const std::string& id_label, bool use_id_stack);

            bool UpdateCache(KeyType id, const ImageBuffer& image, ImageParams* params, bool userRefresh);
            CachedParams& GetCacheParams(KeyType id);
            CachedImageAndTexture& GetCacheImageAndTexture(KeyType id);

            void ClearImagesCache();

            void OnDeleteImageParams(ImageParams* paramsPtr);

            ImageTextureCache(double cachedImagesTimeToLive): mCachedImagesTimeToLive(cachedImagesTimeToLive) {}
            ~ImageTextureCache();
        private:
            // Methods
            void UpdateLinkedZooms(KeyType id);
            void UpdateLinkedColormapSettings(KeyType id);
            bool AddEntryIfMissing(KeyType key);


            internal::Cache<KeyType, CachedParams> mCacheParams;
            double mCachedImagesTimeToLive = 5.;
            internal::ShortLivedCache<KeyType, CachedImageAndTexture> mCacheImages {mCachedImagesTimeToLive };
        };

        extern ImageTextureCache gImageTextureCache;
        extern bool gImageTextureCacheAlive;

    } // namespace ImageUtils


} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp continued                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/panic.h included by src/immvision/internal/image.cpp       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <exception>

namespace ImmVision
{
    void Panic(const std::exception& e);
    void Panic_UnknownCause();
} // namespace ImmVision
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp continued                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/inspector.h included by src/immvision/internal/image.cpp                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// IMMVISION_API is a marker for public API functions. IMMVISION_STRUCT_API is a marker for public API structs (in comment lines)
// Usage of ImmVision as a shared library is not recommended. No guaranty of ABI stability is provided
#ifndef IMMVISION_API
#define IMMVISION_API
#endif

namespace ImmVision
{
    // Add an image to the inspector. Call this from anywhere (e.g. at different steps
    // of an image processing pipeline). Later, call Inspector_Show() to display all collected images.
    //
    // :param image:
    //     The image to add.
    //     C++: accepts ImageBuffer directly, or cv::Mat (implicit conversion, zero-copy).
    //     Python: pass a numpy.ndarray.
    IMMVISION_API void Inspector_AddImage(
        const ImageBuffer& image,
        const std::string& legend,
        const std::string& zoomKey = "",
        const std::string& colormapKey = "",
        const Point2d & zoomCenter = Point2d(),
        double zoomRatio = -1.
    );

    IMMVISION_API void Inspector_Show();

    IMMVISION_API void Inspector_ClearImages();

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image.cpp continued                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stack>

// Image saving uses stb_image_write (supports PNG, BMP, JPG, HDR)

#ifndef IMMVISION_VERSION
#define IMMVISION_VERSION "unknown version"
#endif

namespace ImmVision
{
    // ================================ Color Order stuff ================================

    enum class PrivColorOrder
    {
        RGB,
        BGR
    };

    std::stack<PrivColorOrder> sColorOrderStack;

    void PushColorOrderRgb()
    {
        sColorOrderStack.push(PrivColorOrder::RGB);
    }
    void PushColorOrderBgr()
    {
        sColorOrderStack.push(PrivColorOrder::BGR);
    }
    void PopColorOrder()
    {
        if (sColorOrderStack.empty())
        {
            const char* errorMessage = R"(
Error in ImmVision
==================
PopColorOrder() called too many times. The color order stack is empty.

Ensure that each PushColorOrderRgb()/PushColorOrderBgr() call is paired with a PopColorOrder() call.
)";
            fprintf(stderr, "%s", errorMessage);
            throw std::runtime_error(errorMessage);
        }
        sColorOrderStack.pop();
    }


    void UseRgbColorOrder() { PushColorOrderRgb(); }
    void UseBgrColorOrder() { PushColorOrderBgr(); }

    static bool Priv_IsColorOrderBgr()
    {
        const char* errorMessage = R"(
Error in ImmVision
==================
You must set the image color order before displaying images. At the start of your program, call:
    ImmVision::UseRgbColorOrder() or ImmVision::UseBgrColorOrder() (C++)
or
    immvision.use_rgb_color_order() or immvision.use_bgr_color_order() (Python)

This is a required setup step. (Breaking change - October 2024)
)";
        if (sColorOrderStack.empty())
        {
            fprintf(stderr, "%s", errorMessage);
            throw std::runtime_error(errorMessage);
        }
        return sColorOrderStack.top() == PrivColorOrder::BGR;
    }
    bool IsUsingRgbColorOrder()
    {
        return !Priv_IsColorOrderBgr();
    }
    bool IsUsingBgrColorOrder()
    {
        return Priv_IsColorOrderBgr();
    }
    bool IsColorOrderUndefined()
    {
        return sColorOrderStack.empty();
    }


    // ================================ Rest ================================

    // With Image and ImageDisplay we can rely on the ID stack,
    // since calls to Image & ImageDisplay will have a reproducible id stack
    static bool sDoUseIdStack = true;


    void ClearTextureCache()
    {
        ImageCache::gImageTextureCache.ClearImagesCache();
        Icons::ClearIconsTextureCache();
        Inspector_ClearImages();
        Colormap::ClearColormapsTexturesCache();
    }

    static float FontSizeRatio()
    {
        float r = ImGui::GetFontSize() / 14.5;
        return r;
    }

    void Image(const std::string& label, const ImageBuffer& image, ImageParams* params)
    {
        // Note: although this function is long, it is well organized, and it behaves almost like a class
        // with members = (ImageBuffer& image, ImageParams* params).
        //
        // - it begins by defining a set a lambdas that display various widgets
        //   for the different zones of the Gui (those lambdas are named fnXXXX)
        //
        // - the core of the function is only a few lines long and begins after the line
        //    "Core of the function below"
        //
        // If your editor is able to collapse the lambda definitions, you will see the structure.

        using CachedParams = ImageCache::ImageTextureCache::CachedParams;
        using CachedImages = ImageCache::ImageTextureCache::CachedImageAndTexture;

        //
        // Lambda / is Label visible
        //
        auto fnIsLabelVisible = [&label]() -> bool {
            if (label.empty())
                return false;
            if (label.find("##") == 0)
                return false;
            return true;
        };
        //
        // Lambdas / Watched Pixels
        //
        bool wasWatchedPixelAdded = false;
        auto fnWatchedPixels_Add = [&params, &wasWatchedPixelAdded](const Point2d& pixelDouble)
        {
            Point pixel((int)(pixelDouble.x + .5), (int)(pixelDouble.y + .5));
            params->WatchedPixels.push_back(pixel);

            wasWatchedPixelAdded = true;
            if (! params->ShowOptionsInTooltip)
                params->ShowOptionsPanel = true;
        };
        auto fnWatchedPixels_Gui = [&params, &image]()
        {
            ImGui::Text("Double click the image...");

            int idxToRemove = -1;

            if (! params->WatchedPixels.empty())
            {
                ImGui::BeginTable("WatchedPixels", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX);
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                ImGui::TableNextColumn();
                ImGui::Text("#");
                ImGui::TableNextColumn();
                ImGui::Text("(x,y)");
                ImGui::TableNextColumn();
                ImGui::Text("Color");
                ImGui::TableNextColumn();
                ImGui::Text("%s", "");

                for (size_t i = 0; i < params->WatchedPixels.size(); ++i)
                {
                    Point watchedPixel = params->WatchedPixels[i];
                    ImGui::TableNextRow();

                    // index
                    ImGui::TableNextColumn();
                    ImGui::Text("#%i: ", (int)i);

                    // (x,y)
                    ImGui::TableNextColumn();
                    std::string posStr = std::string("(") + std::to_string(watchedPixel.x) + "," + std::to_string(watchedPixel.y) +")";
                    ImGui::Text("%s", posStr.c_str());

                    // Show Color Cell
                    ImGui::TableNextColumn();
                    ImageWidgets::ShowPixelColorWidget(image, watchedPixel, *params);

                    // Actions
                    ImGui::TableNextColumn();
                    std::string lblRemove = "x##" + std::to_string(i);
                    if (ImGui::SmallButton(lblRemove.c_str()))
                        idxToRemove = (int) i;
                    ImGui::SameLine();
                }
                ImGui::EndTable();
            }
            if (idxToRemove >= 0)
                params->WatchedPixels.erase(params->WatchedPixels.begin() + (std::ptrdiff_t)idxToRemove);

            ImGui::Checkbox("Add Watched Pixel on double click", &params->AddWatchedPixelOnDoubleClick);
            ImGui::Checkbox("Highlight Watched Pixels", &params->HighlightWatchedPixels);
        };

        //
        // Lambdas / Colormap
        //
        auto fnColormap_Gui = [&params, &image](float availableGuiWidth)
        {
            Rect roi = ZoomPanTransform::VisibleRoi(params->ZoomPanMatrix, params->ImageDisplaySize, image.size());
            Colormap::GuiShowColormapSettingsData(
                image,
                roi,
                availableGuiWidth,
                &params->ColormapSettings);
        };


        //
        // Lambdas / Options & Adjustments
        //
        auto fnSaveImage_Gui = [&image, &params](CachedParams & cacheParams, const ImageBuffer& imageWithColormap)
        {
            bool isFloatImage = ImageDepthIsFloat(image.depth);

            // Swap R and B channels in-place for a uint8 3 or 4 channel image
            auto fnSwapRB = [](const ImageBuffer& src) -> ImageBuffer {
                ImageBuffer dst = src.clone();
                int ch = dst.channels;
                for (int y = 0; y < dst.height; y++) {
                    uint8_t* row = dst.ptr<uint8_t>(y);
                    for (int x = 0; x < dst.width; x++)
                        std::swap(row[x * ch], row[x * ch + 2]);
                }
                return dst;
            };

            auto fnGetImageToSave = [&image, &fnSwapRB]() -> ImageBuffer
            {
                // stb_image_write expects RGB order; swap if source is BGR
                if ((image.depth == ImageDepth::uint8 && (image.channels == 3 || image.channels == 4)) && Priv_IsColorOrderBgr())
                    return fnSwapRB(image);
                return image;
            };
            auto fnGetImageWithColorMapToSave = [&imageWithColormap]() {
                // imageWithColormap is always RGBA (internal rendering pipeline outputs RGB order)
                // Drop alpha: create RGB from RGBA
                ImageBuffer rgb = ImageBuffer::Zeros(imageWithColormap.width, imageWithColormap.height, 3, ImageDepth::uint8);
                for (int y = 0; y < imageWithColormap.height; y++) {
                    const uint8_t* src = imageWithColormap.ptr<uint8_t>(y);
                    uint8_t* dst = rgb.ptr<uint8_t>(y);
                    for (int x = 0; x < imageWithColormap.width; x++) {
                        dst[x * 3]     = src[x * 4];
                        dst[x * 3 + 1] = src[x * 4 + 1];
                        dst[x * 3 + 2] = src[x * 4 + 2];
                    }
                }
                return rgb;
            };

            std::string tooltipSaveRawImage =
                "Saves the raw image\n"
                "Specify the format via the filename extension (.jpg, .png, .hdr, etc)\n"
                "\n"
                "- For uint8 3-channel images, use .jpg, .png, or .bmp\n"
                "- For 4 channel images, prefer to use .png\n"
                "- For float images, use .hdr";

            std::string tooltipExportColormap =
                "Export the colormap image as RGB\n"
                "(Hint: use a lossless format, such as .png or .bmp)";

            bool usePortableFileDialogs = pfd::settings::available();

            auto fnSaveImage = [usePortableFileDialogs](const std::string& filename, const ImageBuffer& imageToSave)
            {
                if (filename.empty() || imageToSave.empty())
                    return;

                auto fnReportError = [&](const std::string& msg) {
                    if (usePortableFileDialogs)
                        pfd::message("Error", msg, pfd::choice::ok, pfd::icon::error);
                    else
                        fprintf(stderr, "%s\n", msg.c_str());
                };

                // Only uint8 images can be saved with stb (except HDR which needs float)
                if (imageToSave.depth != ImageDepth::uint8 && imageToSave.depth != ImageDepth::float32)
                {
                    fnReportError("Cannot save image: only uint8 and float32 images are supported.\n"
                                  "Use .hdr for float32 images, or .png/.jpg/.bmp for uint8 images.");
                    return;
                }

                // Detect format from extension
                std::string ext;
                {
                    auto dot = filename.rfind('.');
                    if (dot != std::string::npos)
                    {
                        ext = filename.substr(dot);
                        for (auto& c : ext) c = (char)tolower(c);
                    }
                }

                int w = imageToSave.width, h = imageToSave.height, ch = imageToSave.channels;
                int ok = 0;
                if (ext == ".hdr" && imageToSave.depth == ImageDepth::float32)
                {
                    ok = stbi_write_hdr(filename.c_str(), w, h, ch,
                                        static_cast<const float*>(imageToSave.data));
                }
                else if (imageToSave.depth == ImageDepth::uint8)
                {
                    const uint8_t* data = static_cast<const uint8_t*>(imageToSave.data);
                    int stride = (int)imageToSave.step;
                    if (ext == ".png")
                        ok = stbi_write_png(filename.c_str(), w, h, ch, data, stride);
                    else if (ext == ".bmp")
                        ok = stbi_write_bmp(filename.c_str(), w, h, ch, data);
                    else if (ext == ".jpg" || ext == ".jpeg")
                        ok = stbi_write_jpg(filename.c_str(), w, h, ch, data, 95);
                    else if (ext == ".tga")
                        ok = stbi_write_tga(filename.c_str(), w, h, ch, data);
                    else
                    {
                        fnReportError("Unsupported format: " + ext + "\nSupported: .png, .bmp, .jpg, .tga, .hdr");
                        return;
                    }
                }
                else
                {
                    fnReportError("Use .hdr extension for float32 images");
                    return;
                }

                if (!ok)
                    fnReportError("Could not save image to: " + filename);
            };

            auto fnAskForFilenameWithPfd = []() -> std::string
            {
                pfd::settings::verbose(true);
                std::string filename = pfd::save_file("Select a file", ".",
                                                      { "Image Files", "*.png *.jpg *.jpeg *.jpg *.bmp *.gif *.hdr *.exr",
                                                        "All Files", "*" }).result();
                return filename;
            };
            auto fnAskForFilenameWithImGui = [&cacheParams]() -> std::string
            {
                char *filename = cacheParams.FilenameEditBuffer.data();
                return filename;
            };
            auto fnAskForFilename = [usePortableFileDialogs, fnAskForFilenameWithImGui, fnAskForFilenameWithPfd]() {
                return usePortableFileDialogs ? fnAskForFilenameWithPfd() : fnAskForFilenameWithImGui();
            };
            auto fnInputFilenameWithImGui = [&cacheParams]()
            {
                ImGui::Text("File name");
                char *filename = cacheParams.FilenameEditBuffer.data();
                ImGui::SetNextItemWidth(200.f * FontSizeRatio());
                ImGui::InputText("##filename", filename, 1000);
                ImGui::Text("The image will be saved in the current folder");
            };

            if (!usePortableFileDialogs)
                fnInputFilenameWithImGui();
            // Save image button
            if (ImGuiImm::ButtonWithTooltip("Save image", tooltipSaveRawImage))
                fnSaveImage(fnAskForFilename(), fnGetImageToSave());
            // For float images, give the possibility to save them with the colormap applied
            if (isFloatImage && ImGuiImm::ButtonWithTooltip("Export colormap image", tooltipExportColormap))
                fnSaveImage(fnAskForFilename(), fnGetImageWithColorMapToSave());

        };

        auto fnImageDisplayOptions_Gui = [&params, &image]()
        {
            ImGui::Checkbox("Show school paper background", &params->ShowSchoolPaperBackground);
            if (image.depth == ImageDepth::uint8 && image.channels == 4)
                ImGui::Checkbox("Show alpha channel checkerboard", &params->ShowAlphaChannelCheckerboard);
            if (image.channels > 1)
            {
                ImGui::Text("Channels: ");
                ImGui::RadioButton("All", &params->SelectedChannel, -1); ImGui::SameLine();
                for (int channel_id = 0; channel_id < image.channels; ++channel_id)
                {
                    ImGui::RadioButton(std::to_string(channel_id).c_str(), &params->SelectedChannel, channel_id);
                    ImGui::SameLine();
                }
                ImGui::NewLine();
            }
            {
                ImGuiImm::BeginGroupPanel("High zoom options");
                ImGui::Checkbox("Grid", &params->ShowGrid);
                ImGui::Checkbox("Draw values on pixels", &params->DrawValuesOnZoomedPixels);
                ImGuiImm::EndGroupPanel();
            }

        };

        auto fnMiscOptions_Gui = [&params]()
        {
            {
                ImGuiImm::BeginGroupPanel("Image display options");
                ImGui::Checkbox("Show image info", &params->ShowImageInfo);
                ImGui::Checkbox("Show pixel info", &params->ShowPixelInfo);
                ImGui::Checkbox("Show zoom buttons", &params->ShowZoomButtons);
                ImGuiImm::EndGroupPanel();
            }

            ImGui::Checkbox("Pan with mouse", &params->PanWithMouse);
            ImGui::Checkbox("Zoom with mouse wheel", &params->ZoomWithMouseWheel);
            ImGui::Checkbox("Resize keep aspect ratio", &params->ResizeKeepAspectRatio);

            ImGui::Separator();
            if (ImGui::Checkbox("Show Options in tooltip window", &params->ShowOptionsInTooltip))
            {
                if (!params->ShowOptionsInTooltip) // We were in a tooltip when clicking
                    params->ShowOptionsPanel = true;
            }
        };

        auto fnOptionsInnerGui = [&params, &image, &fnWatchedPixels_Gui, &wasWatchedPixelAdded,
                                  &fnColormap_Gui, &fnSaveImage_Gui, &fnImageDisplayOptions_Gui, &fnMiscOptions_Gui]
                                      (CachedParams & cacheParams, const ImageBuffer& imageWithColormap)
        {
            float optionsWidth = 260.f * FontSizeRatio();


            // Group with fixed width, so that Collapsing headers stop at optionsWidth
            ImGuiImm::BeginGroupFixedWidth(optionsWidth);

            // Colormap
            if (Colormap::CanColormap(image) && ImageWidgets::CollapsingHeader_OptionalCacheState("Colormap"))
                fnColormap_Gui(optionsWidth);

            // Watched Pixels
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Watched Pixels", wasWatchedPixelAdded))
                fnWatchedPixels_Gui();

            // Image display options
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Image Display"))
                fnImageDisplayOptions_Gui();

            // Misc options
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Misc"))
                fnMiscOptions_Gui();

            // Save Image
            if (ImageWidgets::CollapsingHeader_OptionalCacheState("Save"))
                fnSaveImage_Gui(cacheParams, imageWithColormap);

            ImGuiImm::EndGroupFixedWidth();

        };

        auto fnToggleShowOptions = [&params]()
        {
            if (params->ShowOptionsInTooltip)
                ImGui::OpenPopup("Options");
            else
                params->ShowOptionsPanel = !params->ShowOptionsPanel;
        };

        auto fnOptionGui = [&params, &fnOptionsInnerGui](CachedParams & cacheParams, const ImageBuffer& imageWithColormap)
        {
            if (params->ShowOptionsInTooltip)
            {
                if (ImGui::BeginPopup("Options"))
                {
                    fnOptionsInnerGui(cacheParams, imageWithColormap);
                    ImGui::EndPopup();
                }
            }
            else if (params->ShowOptionsPanel)
            {
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("Options");
                fnOptionsInnerGui(cacheParams, imageWithColormap);
                ImGui::EndGroup();
            }
        };

        //
        // Lambdas / Handle Zoom
        //
        // Mouse dragging
        auto fnHandleMouseDragging = [&params](CachedParams & cacheParams)
        {
            if (cacheParams.IsResizing)
                return;
            Matrix33d zoomMatrix = params->ZoomPanMatrix;

            int mouseDragButton = 0;
            bool isMouseDraggingInside = ImGui::IsMouseDragging(mouseDragButton) && ImGui::IsItemHovered();
            if (isMouseDraggingInside)
                cacheParams.IsMouseDragging = true;
            if (! ImGui::IsMouseDown(mouseDragButton))
            {
                cacheParams.IsMouseDragging = false;
                cacheParams.LastDragDelta = ImVec2(0.f, 0.f);
            }
            if (cacheParams.IsMouseDragging && params->PanWithMouse )
            {
                ImVec2 dragDelta = ImGui::GetMouseDragDelta(mouseDragButton);
                ImVec2 dragDeltaDelta(dragDelta.x - cacheParams.LastDragDelta.x, dragDelta.y - cacheParams.LastDragDelta.y);
                zoomMatrix = zoomMatrix * ZoomPanTransform::ComputePanMatrix(
                    Point2d((double)dragDeltaDelta.x, (double)dragDeltaDelta.y),
                    zoomMatrix(0, 0));
                cacheParams.LastDragDelta = dragDelta;
            }
            params->ZoomPanMatrix = zoomMatrix;
        };
        auto fnHandleMouseWheel = [&params](const Point2d& mouseLocation)
        {
            if (!params->ZoomWithMouseWheel)
                return;
            ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

            if ((fabs(ImGui::GetIO().MouseWheel) > 0.f) && (ImGui::IsItemHovered()))
            {
                double zoomRatio = (double)ImGui::GetIO().MouseWheel / 4.;
                ImGui::GetIO().MouseWheel = 0.f;

                double currentZoom = params->ZoomPanMatrix(0, 0);
                bool isZoomIn = zoomRatio > 0.;

                bool refuseZoom;
                {
                    if (isZoomIn)
                        refuseZoom = currentZoom > 5000.;
                    else
                        refuseZoom = currentZoom < 0.005;
                }
                if (refuseZoom)
                    return;
                params->ZoomPanMatrix = params->ZoomPanMatrix * ZoomPanTransform::ComputeZoomMatrix(mouseLocation, exp(zoomRatio));
            }
        };
        auto fnShowZoomButtons = [&params, &image]()
        {
            if (params->ShowZoomButtons)
            {
                Matrix33d zoomMatrix = params->ZoomPanMatrix;

                Point2d viewportCenter_originalImage = ZoomPanTransform::Apply(
                    zoomMatrix.inv(),
                    Point2d(
                        (double)params->ImageDisplaySize.width / 2.,
                        (double)params->ImageDisplaySize.height / 2.)
                );

                {
                    Point2d zoomCenter = params->WatchedPixels.empty() ?
                                viewportCenter_originalImage
                            :   Point2d(params->WatchedPixels.back());
                    ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
                    if (Icons::IconButton(Icons::IconType::ZoomPlus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1.1);

                    ImGui::SameLine();

                    if (Icons::IconButton(Icons::IconType::ZoomMinus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1. / 1.1);

                    ImGui::PopItemFlag();
                }
                ImGui::SameLine();
                // Scale1 & Full View Zoom  buttons
                {
                    auto scaleOneZoomInfo = ZoomPanTransform::MakeScaleOne(image.size(), params->ImageDisplaySize);
                    auto fullViewZoomInfo = ZoomPanTransform::MakeFullView(image.size(), params->ImageDisplaySize);
                    if (Icons::IconButton(
                        Icons::IconType::ZoomScaleOne,
                        ZoomPanTransform::IsEqual(zoomMatrix, scaleOneZoomInfo)) // disabled flag
                        )
                        zoomMatrix = scaleOneZoomInfo;

                    ImGui::SameLine();

                    if (Icons::IconButton(
                        Icons::IconType::ZoomFullView,
                        ZoomPanTransform::IsEqual(zoomMatrix, fullViewZoomInfo)) // disabled flag
                        )
                        zoomMatrix = fullViewZoomInfo;
                }
                params->ZoomPanMatrix = zoomMatrix;
            }

        };
        //
        // Lambda / Show image
        //
        auto fnShowImage = [&params](const GlTexture& glTexture) ->  MouseInformation
        {
            bool disableDragWindow = params->PanWithMouse;
            Point2d mouseLocation = ImageWidgets::DisplayTexture_TrackMouse(
                    glTexture,
                    ImVec2((float)params->ImageDisplaySize.width, (float)params->ImageDisplaySize.height), disableDragWindow);

            MouseInformation mouseInfo;
            if (ImGui::IsItemHovered())
            {
                mouseInfo.IsMouseHovering = true;
                mouseInfo.MousePosition = ZoomPanTransform::Apply(params->ZoomPanMatrix.inv(), mouseLocation);
                mouseInfo.MousePosition_Displayed = Point((int)mouseLocation.x, (int)mouseLocation.y);
            }
            return mouseInfo;
        };

        //
        // Lambda / Show resize widget in the bottom right corner
        //
        auto fnShowResizeWidget = [&params, &image](CachedParams & cacheParams)
        {
            if (!params->CanResize)
                return;
            ImVec2 imageBottomRight = ImGui::GetItemRectMax();
            float em = ImGui::GetFontSize();
            float size = em * 1.0f;
            ImVec2 br(imageBottomRight.x, imageBottomRight.y);
            ImVec2 bl(br.x - size, br.y);
            ImVec2 tr(br.x, br.y - size);
            ImVec2 tl(br.x - size, br.y - size);

            ImRect zone(tl, br);

            if (!params->PanWithMouse)
            {
                auto cursorPos = ImGui::GetCursorScreenPos();
                ImGui::SetCursorScreenPos(tl);
                ImGui::InvisibleButton("##resize", zone.GetSize());
                ImGui::SetCursorScreenPos(cursorPos);
                ImGui::Dummy(ImVec2(0.f, 0.f));
            }

            bool isMouseHoveringWidget = ImGui::IsMouseHoveringRect(zone.Min, zone.Max);
            ImU32 color = isMouseHoveringWidget ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) : ImGui::GetColorU32(ImGuiCol_Button);
            ImGui::GetWindowDrawList()->AddTriangleFilled(br, bl, tr, color);

            if (!cacheParams.IsResizing)
            {
                if (isMouseHoveringWidget && ImGui::IsMouseDown(0))
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
                    cacheParams.IsResizing = true;
                }
            }
            if (cacheParams.IsResizing)
            {
                if (ImGui::IsMouseDown(0))
                {
                    if (ImGui::GetIO().MouseDelta.x != 0. || ImGui::GetIO().MouseDelta.y != 0.)
                    {
                        params->ImageDisplaySize.width += (int)ImGui::GetIO().MouseDelta.x;
                        params->ImageDisplaySize.height += (int)ImGui::GetIO().MouseDelta.y;

                        if (params->ImageDisplaySize.width < 5)
                            params->ImageDisplaySize.width = 5;
                        if (params->ImageDisplaySize.height < 5)
                            params->ImageDisplaySize.height = 5;

                        if (params->ResizeKeepAspectRatio)
                        {
                            float imageDisplayRatio = (float)params->ImageDisplaySize.width / (float)params->ImageDisplaySize.height;
                            float imageRatio = (float)image.width / (float)image.height;
                            if (imageDisplayRatio > imageRatio)
                                params->ImageDisplaySize.width = (int)((float)params->ImageDisplaySize.height * imageRatio);
                            else
                                params->ImageDisplaySize.height = (int)((float)params->ImageDisplaySize.width / imageRatio);
                        }
                    }
                }
                else
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
                    cacheParams.IsResizing = false;
                }
            }
        };


        //
        // Lambda / Show pixel info
        //
        auto fnShowPixelInfo = [&image, &params](const Point2d& mouseLocation)
        {
            Point mouseLoc =
                mouseLocation.x >= 0. ?
                        Point((int)(mouseLocation.x + 0.5), (int)(mouseLocation.y + 0.5))
                    :   Point(-1, -1)
                    ;
            if (mouseLoc.x >= 0)
            {
                ImGui::Text("(%i,%i)", mouseLoc.x, mouseLoc.y);
                ImGui::SameLine();
            }
            ImageWidgets::ShowPixelColorWidget(image, mouseLoc, *params);
        };

        //
        // Lambda / Show full Gui
        //
        auto fnShowFullGui = [&](CachedParams& cacheParams, CachedImages &cacheImages) -> MouseInformation
        {

            ImGui::BeginGroup();
            // Show image
            auto mouseInfo = fnShowImage(*cacheImages.mGlTexture);
            fnShowResizeWidget(cacheParams);
            // Add Watched Pixel on double click
            if (   params->AddWatchedPixelOnDoubleClick
                && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
                && ImGui::IsItemHovered())
                fnWatchedPixels_Add(mouseInfo.MousePosition);

            // Handle Mouse
            fnHandleMouseDragging(cacheParams);
            fnHandleMouseWheel(mouseInfo.MousePosition);

            // Zoom+ / Zoom- buttons
            fnShowZoomButtons();
            // adjust button
            if (params->ShowOptionsButton)
            {
                if (!params->ShowZoomButtons)
                    ImGui::NewLine();
                ImGuiImm::SameLineAlignRight(20.f, (float)params->ImageDisplaySize.width);
                if (Icons::IconButton(Icons::IconType::AdjustLevels))
                    fnToggleShowOptions();
            }

            // Show infos
            if (params->ShowImageInfo)
                ImageWidgets::ShowImageInfo(image, params->ZoomPanMatrix(0, 0));
            if (params->ShowPixelInfo)
                fnShowPixelInfo(mouseInfo.MousePosition);
            ImGui::EndGroup();

            // Show Options
            fnOptionGui(cacheParams, cacheImages.mImageRgbaCache);

            return mouseInfo;
        };
        auto fnShowFullGui_WithBorder = [&](CachedParams& cacheParams, CachedImages &cacheImages) -> MouseInformation
        {
            // BeginGroupPanel
            bool drawBorder =  fnIsLabelVisible();
            std::string title = label + "##title";
            if (drawBorder)
                ImGuiImm::BeginGroupPanel_FlagBorder(title.c_str(), drawBorder);
            else
                ImGui::BeginGroup();
            auto mouseInfo = fnShowFullGui(cacheParams, cacheImages);
            if (drawBorder)
                ImGuiImm::EndGroupPanel_FlagBorder();
            else
                ImGui::EndGroup();
            return mouseInfo;
        };


        /////////////////////////////////////////////////////////////////////////////////////////
        //
        // Core of the function below (there are only lambdas declarations before)
        //
        /////////////////////////////////////////////////////////////////////////////////////////
        if (image.empty())
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f),
                               "%s -> empty image !!!", label.c_str());
            params->MouseInfo = MouseInformation();
            return;
        }

        ImGui::PushID(label.c_str());
        try
        {
            auto id = ImageCache::gImageTextureCache.GetID(label, sDoUseIdStack);
            bool isNewImage = ImageCache::gImageTextureCache.UpdateCache(id, image, params, params->RefreshImage);
            auto &cacheParams = ImageCache::gImageTextureCache.GetCacheParams(id);
            auto &cacheImages = ImageCache::gImageTextureCache.GetCacheImageAndTexture(id);
            params->MouseInfo = fnShowFullGui_WithBorder(cacheParams, cacheImages);

            // Handle Colormap
            Rect roi = ZoomPanTransform::VisibleRoi(params->ZoomPanMatrix, params->ImageDisplaySize, image.size());
            if (isNewImage || params->RefreshImage)
                Colormap::InitStatsOnNewImage(image, roi, &params->ColormapSettings);
            if (! ZoomPanTransform::IsEqual(cacheParams.PreviousParams.ZoomPanMatrix, params->ZoomPanMatrix))
                Colormap::UpdateRoiStatsInteractively(image, roi, &params->ColormapSettings);
        }
        catch(std::exception& e)
        {
            Panic(e);
        }
        catch(...)
        {
            Panic_UnknownCause();
        }
        ImGui::PopID();
    }


    Point2d ImageDisplay(
        const std::string& label_id,
        const ImageBuffer& image,
        const Size& imageDisplaySize,
        bool refreshImage,
        bool showOptionsButton
        )
    {
        ImGuiID id = ImGui::GetID(label_id.c_str());
        static std::map<ImGuiID, ImageParams> s_Params;
        if (s_Params.find(id) == s_Params.end())
        {
            ImageParams params = showOptionsButton ? ImageParams() : FactorImageParamsDisplayOnly();
            s_Params[id] = params;
        }

        ImageParams& params = s_Params.at(id);
        {
            params.ShowOptionsButton = showOptionsButton;
            params.ImageDisplaySize = imageDisplaySize;
            params.RefreshImage = refreshImage;

            Size displayedSize = ImGuiImm::ComputeDisplayImageSize(imageDisplaySize, image.size());
            params.ZoomPanMatrix = ZoomPanTransform::MakeFullView(image.size(), displayedSize);
        }

        Image(label_id, image, &params);
        return params.MouseInfo.MousePosition;
    }


    static std::map<ImGuiID, ImVec2> s_ImageDisplayResizable_Sizes;

    IMMVISION_API Point2d ImageDisplayResizable(
        const std::string& label_id,
        const ImageBuffer& image,
        ImVec2* size,
        bool refreshImage,
        bool resizable,
        bool showOptionsButton
    )
    {
        if (size == nullptr)
        {
            ImGuiID id = ImGui::GetID(label_id.c_str());
            if (s_ImageDisplayResizable_Sizes.find(id) == s_ImageDisplayResizable_Sizes.end())
                s_ImageDisplayResizable_Sizes[id] = ImVec2(0, 0);
            size = &s_ImageDisplayResizable_Sizes[id];
        }

        ImGuiID id = ImGui::GetID(label_id.c_str());
        static std::map<ImGuiID, ImageParams> s_Params;
        if (s_Params.find(id) == s_Params.end())
        {
            ImageParams params = showOptionsButton ? ImageParams() : FactorImageParamsDisplayOnly();
            s_Params[id] = params;
        }

        Size imageDisplaySize = Size((int)size->x, (int)size->y);
        ImageParams& params = s_Params.at(id);
        {
            params.ShowOptionsButton = showOptionsButton;
            params.ImageDisplaySize = imageDisplaySize;
            params.CanResize = resizable;
            params.RefreshImage = refreshImage;

            Size displayedSize = ImGuiImm::ComputeDisplayImageSize(imageDisplaySize, image.size());
            params.ZoomPanMatrix = ZoomPanTransform::MakeFullView(image.size(), displayedSize);
        }
        std::string hiddenLabel = std::string("##") + label_id;
        Image(hiddenLabel, image, &params);

        *size = ImVec2((float)params.ImageDisplaySize.width, (float)params.ImageDisplaySize.height);
        return params.MouseInfo.MousePosition;
    }


    ImageParams FactorImageParamsDisplayOnly()
    {
        ImageParams imageParams;
        {
            imageParams.ShowOptionsButton = false;
            imageParams.ShowOptionsPanel = false;
            imageParams.ZoomWithMouseWheel = false;
            imageParams.PanWithMouse = false;
            imageParams.CanResize = false;
            imageParams.ResizeKeepAspectRatio = true;
            imageParams.ShowPixelInfo = false;
            imageParams.ShowImageInfo = false;
            imageParams.ShowGrid = false;
            imageParams.ShowAlphaChannelCheckerboard = false;
            imageParams.ShowSchoolPaperBackground = false;
            imageParams.ShowZoomButtons = false;
            imageParams.AddWatchedPixelOnDoubleClick = false;
        }
        return imageParams;
    }


    std::string VersionInfo()
    {
        char msg[2000];
        snprintf(msg, 2000, "immvision version %s (%s)", IMMVISION_VERSION, __TIMESTAMP__);
        return msg;
    }

    std::vector<std::string> AvailableColormaps()
    {
        return Colormap::AvailableColormaps();
    }


    ImageBuffer GetCachedRgbaImage(const std::string& label)
    {
        auto id = ImageCache::gImageTextureCache.GetID(label, sDoUseIdStack);
        return ImageCache::gImageTextureCache.GetCacheImageAndTexture(id).mImageRgbaCache;
    }

    ImageParams::~ImageParams()
    {
        if (ImageCache::gImageTextureCacheAlive)
            ImageCache::gImageTextureCache.OnDeleteImageParams(this);
    }

    ImageBuffer ImRead(const std::string& filename)
    {
        int w, h, channels;
        // Try HDR first
        if (stbi_is_hdr(filename.c_str()))
        {
            float* data = stbi_loadf(filename.c_str(), &w, &h, &channels, 0);
            if (!data)
                return ImageBuffer();
            ImageBuffer buf = ImageBuffer::Zeros(w, h, channels, ImageDepth::float32);
            std::memcpy(buf.data, data, (size_t)w * h * channels * sizeof(float));
            stbi_image_free(data);
            return buf;
        }
        // Standard uint8 image
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &channels, 0);
        if (!data)
            return ImageBuffer();
        ImageBuffer buf = ImageBuffer::Zeros(w, h, channels, ImageDepth::uint8);
        std::memcpy(buf.data, data, (size_t)w * h * channels);
        stbi_image_free(data);
        return buf;
    }

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image_cache.cpp                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace ImageCache
    {

        static KeyType hash_str(const std::string& str)
        {
            std::string str2 = str + "mlkyqsdadsfklqqsmax!(((!' "; // I let my cat walk on the keyboard
            const std::hash<std::string> hasher;
            size_t hashResult = hasher(str2);
            return hashResult;
        }

        void InitializeMissingParams(ImageParams* params, const ImageBuffer& image)
        {
            if (Colormap::IsNone(params->ColormapSettings))
                params->ColormapSettings = Colormap::ComputeInitialColormapSettings(image);
            {
                Rect fullRoi(Point(0, 0), image.size());
                Colormap::InitStatsOnNewImage(image, fullRoi, &params->ColormapSettings);
            }
            if (params->ZoomPanMatrix == Matrix33d::eye())
                params->ZoomPanMatrix = ZoomPanTransform::MakeFullView(image.size(), params->ImageDisplaySize);
        }

        bool ShallRefreshRgbaCache(const ImageParams& v1, const ImageParams& v2)
        {
            if (v1.ColormapSettings.Colormap != v2.ColormapSettings.Colormap)
                return true;
            if (v1.ColormapSettings.internal_ColormapHovered != v2.ColormapSettings.internal_ColormapHovered)
                return true;
            if (! Colormap::IsEqual(v1.ColormapSettings, v2.ColormapSettings))
                return true;
            if (v1.SelectedChannel != v2.SelectedChannel)
                return true;
            if (v1.ShowAlphaChannelCheckerboard != v2.ShowAlphaChannelCheckerboard)
                return true;
            if (v1.ShowSchoolPaperBackground != v2.ShowSchoolPaperBackground)
                return true;
            return false;
        }

        bool ShallRefreshTexture(const ImageParams& v1, const ImageParams& v2)
        {
            if (v1.ColormapSettings.Colormap != v2.ColormapSettings.Colormap)
                return true;
            if (v1.ColormapSettings.internal_ColormapHovered != v2.ColormapSettings.internal_ColormapHovered)
                return true;
            if (v1.ImageDisplaySize != v2.ImageDisplaySize)
                return true;
            if (! ZoomPanTransform::IsEqual(v1.ZoomPanMatrix, v2.ZoomPanMatrix))
                return true;
            if (! Colormap::IsEqual(v1.ColormapSettings, v2.ColormapSettings))
                return true;
            if (v1.ShowGrid != v2.ShowGrid)
                return true;
            if (v1.SelectedChannel != v2.SelectedChannel)
                return true;
            if (v1.ShowSchoolPaperBackground != v2.ShowSchoolPaperBackground)
                return true;
            if (v1.WatchedPixels.size() != v2.WatchedPixels.size())
                return true;
            if (v1.HighlightWatchedPixels != v2.HighlightWatchedPixels)
                return true;
            if (v1.DrawValuesOnZoomedPixels != v2.DrawValuesOnZoomedPixels)
                return true;
            return false;
        }


        //
        // ImageTextureCache impl below
        //
        bool ImageTextureCache::AddEntryIfMissing(KeyType key)
        {
            bool isNewEntry = false;
            if (! mCacheParams.Contains(key))
            {
                isNewEntry = true;
                mCacheParams.AddKey(key);
            }
            if (! mCacheImages.Contains(key))
            {
                mCacheImages.AddKey(key);
                isNewEntry = true;
                mCacheImages.Get(key).mGlTexture = std::make_unique<GlTexture>();
            }
            return isNewEntry;
        }


        bool ImageTextureCache::UpdateCache(KeyType id, const ImageBuffer& image, ImageParams* params, bool userRefresh)
        {
            // Update cache entries
            bool isNewEntry = AddEntryIfMissing(id);

            // Get caches
            CachedParams& cachedParams = mCacheParams.Get(id);
            CachedImageAndTexture& cachedImage = mCacheImages.Get(id);
            cachedParams.ParamsPtr = params;
            ImageParams oldParams = cachedParams.PreviousParams;

            // Update current params if needed
            {
                params->ImageDisplaySize = ImGuiImm::ComputeDisplayImageSize(params->ImageDisplaySize, image.size());

                if (isNewEntry)
                    InitializeMissingParams(params, image);

                bool wasDisplaySizeChanged = oldParams.ImageDisplaySize != params->ImageDisplaySize;
                bool wasImageSizeChanged = ( (cachedParams.PreviousImageSize.area() != 0)
                                             && (cachedParams.PreviousImageSize != image.size()));
                bool isDisplaySizeEmpty = (oldParams.ImageDisplaySize.area() == 0);

                bool tryAdaptZoomToNewDisplaySize = wasDisplaySizeChanged && !wasImageSizeChanged && !isDisplaySizeEmpty;
                if (tryAdaptZoomToNewDisplaySize)
                {
                    params->ZoomPanMatrix = ZoomPanTransform::UpdateZoomMatrix_DisplaySizeChanged(
                        oldParams.ZoomPanMatrix, oldParams.ImageDisplaySize, params->ImageDisplaySize);
                }
            }

            bool shallRefreshTexture = false;
            bool shallRefreshRgbaCache = false;
            {
                bool fullRefresh =
                    (      userRefresh
                        || isNewEntry
                        || (cachedImage.mGlTexture->ImageSize.empty())
                        || ShallRefreshRgbaCache(oldParams, *params));
                if (fullRefresh)
                {
                    shallRefreshTexture = true;
                    shallRefreshRgbaCache = true;
                }
                if (ShallRefreshTexture(oldParams, *params))
                    shallRefreshTexture = true;
                if (cachedParams.WasZoomJustUpdatedByLink)
                {
                    shallRefreshTexture = true;
                    cachedParams.WasZoomJustUpdatedByLink = false;
                }
            }

            if (shallRefreshTexture)
            {
                ImageDrawing::BlitImageTexture(
                    *params, image, cachedImage.mImageRgbaCache, shallRefreshRgbaCache, cachedImage.mGlTexture.get());
            }

            if (!cachedParams.WasZoomJustUpdatedByLink && !ZoomPanTransform::IsEqual(oldParams.ZoomPanMatrix, params->ZoomPanMatrix))
                UpdateLinkedZooms(id);
            if (! Colormap::IsEqual(oldParams.ColormapSettings, params->ColormapSettings))
                UpdateLinkedColormapSettings(id);

            cachedParams.PreviousParams = *params;
            cachedParams.PreviousImageSize = image.size();
            mCacheImages.ClearOldEntries();

            return isNewEntry;
        }

        KeyType ImageTextureCache::GetID(const std::string& id_label, bool use_id_stack)
        {
            if (use_id_stack)
            {
                ImGuiID id = ImGui::GetID(id_label.c_str());
                return id;
            }
            else
            {
                return hash_str(id_label);
            }
        }

        ImageTextureCache::CachedParams& ImageTextureCache::GetCacheParams(KeyType id)
        {
            return mCacheParams.Get(id);
        }
        ImageTextureCache::CachedImageAndTexture& ImageTextureCache::GetCacheImageAndTexture(KeyType id)
        {
            return mCacheImages.Get(id);
        }

        void ImageTextureCache::ClearImagesCache()
        {
            mCacheImages.Clear();
        }

        void ImageTextureCache::OnDeleteImageParams(ImageParams *paramsPtr)
        {
            auto keys = mCacheParams.Keys();

            std::vector<KeyType> keysToDelete;

            for(auto key: keys)
            {
                auto& cachedValue = mCacheParams.Get(key);
                if (cachedValue.ParamsPtr == paramsPtr)
                    keysToDelete.push_back(key);
            }

            if (keysToDelete.size() > 0)
            {
                for (auto key: keysToDelete)
                    mCacheParams.RemoveKey(key);
            }
        }

        void ImageTextureCache::UpdateLinkedZooms(KeyType id)
        {
            auto & currentCache = mCacheParams.Get(id);
            std::string zoomKey = currentCache.ParamsPtr->ZoomKey;
            if (zoomKey.empty())
                return;

            Matrix33d newZoom = currentCache.ParamsPtr->ZoomPanMatrix;
            double currentZoomRatio = newZoom(0, 0);

            Size displayedImageSize = currentCache.ParamsPtr->ImageDisplaySize;
            Point2d visibleImageCenter_ImageCoords;
            {
                Point2d visibleCenter_Viewport(
                    (double)displayedImageSize.width / 2., (double)displayedImageSize.height / 2.);
                visibleImageCenter_ImageCoords = ZoomPanTransform::Apply(newZoom.inv(), visibleCenter_Viewport);
            }

            for (auto& otherCacheKey : mCacheParams.Keys())
            {
                CachedParams & otherCache = mCacheParams.Get(otherCacheKey);

                if (otherCache.ParamsPtr == currentCache.ParamsPtr)
                    continue;

                if ((otherCacheKey != id) && (otherCache.ParamsPtr->ZoomKey == zoomKey))
                {
                    Size otherDisplayedImageSize = otherCache.ParamsPtr->ImageDisplaySize;

                    double sizeRatioOtherImage = (double)otherDisplayedImageSize.width / (double)displayedImageSize.width;
                    double zoomRatioOtherImage = currentZoomRatio * sizeRatioOtherImage;
                    auto zoomMatrixOtherImage = ZoomPanTransform::MakeZoomMatrix(
                        visibleImageCenter_ImageCoords, zoomRatioOtherImage, otherDisplayedImageSize);
                    otherCache.ParamsPtr->ZoomPanMatrix = zoomMatrixOtherImage;
                    otherCache.PreviousParams.ZoomPanMatrix = zoomMatrixOtherImage;
                    otherCache.WasZoomJustUpdatedByLink = true;
                }
            }
        }
        void ImageTextureCache::UpdateLinkedColormapSettings(KeyType id)
        {
            auto & currentCache = mCacheParams.Get(id);
            std::string colormapKey = currentCache.ParamsPtr->ColormapKey;
            if (colormapKey.empty())
                return;
            ColormapSettingsData newColorAdjustments = currentCache.ParamsPtr->ColormapSettings;
            for (auto& otherCacheKey : mCacheParams.Keys())
            {
                CachedParams & otherCache = mCacheParams.Get(otherCacheKey);
                if ((otherCacheKey != id) && (otherCache.ParamsPtr->ColormapKey == colormapKey))
                    otherCache.ParamsPtr->ColormapSettings = newColorAdjustments;
            }
        }

        ImageTextureCache::~ImageTextureCache()
        {
            if (this == &gImageTextureCache)
                gImageTextureCacheAlive = false;
        }


        ImageTextureCache gImageTextureCache(5.); // TTL of 5 seconds
        bool gImageTextureCacheAlive = true;
    } // namespace ImageUtils


} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/image_params_serialize.cpp                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef IMMVISION_SERIALIZE_JSON

#include "nlohmann/json.hpp"

namespace nlohmann
{
    template <>
    struct adl_serializer<ImmVision::Size>
    {
        static void to_json(json& j, const ImmVision::Size& size) {
            j = json{{"width", size.width}, {"height", size.height}};
        }

        static void from_json(const json& j, ImmVision::Size& size) {
            j.at("width").get_to(size.width);
            j.at("height").get_to(size.height);
        }
    };

    template <>
    struct adl_serializer<ImmVision::Point>
    {
        static void to_json(json& j, const ImmVision::Point& point) {
            j = json{{"x", point.x}, {"y", point.y}};
        }

        static void from_json(const json& j, ImmVision::Point& point) {
            j.at("x").get_to(point.x);
            j.at("y").get_to(point.y);
        }
    };

    template <>
    struct adl_serializer<ImmVision::Matrix33d>
    {
        static void to_json(json& j, const ImmVision::Matrix33d& mat) {
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    j[std::to_string(row) + std::to_string(col)] = mat.m[row][col];
                }
            }
        }

        static void from_json(const json& j, ImmVision::Matrix33d& mat) {
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    mat.m[row][col] = j[std::to_string(row) + std::to_string(col)];
                }
            }
        }
    };

    template <>
    struct adl_serializer<ImmVision::ColormapScaleFromStatsData>
    {
        static void to_json(json& j, const ImmVision::ColormapScaleFromStatsData& data) {
            j = json{
                {"ColorMapStatsType", data.ColorMapStatsType},
                {"NbSigmas", data.NbSigmas},
                {"UseStatsMin", data.UseStatsMin},
                {"UseStatsMax", data.UseStatsMax}
            };
        }

        static void from_json(const json& j, ImmVision::ColormapScaleFromStatsData& data) {
            j.at("ColorMapStatsType").get_to(data.ColorMapStatsType);
            j.at("NbSigmas").get_to(data.NbSigmas);
            j.at("UseStatsMin").get_to(data.UseStatsMin);
            j.at("UseStatsMax").get_to(data.UseStatsMax);
        }
    };

    template <>
    struct adl_serializer<ImmVision::ColormapSettingsData>
    {
        static void to_json(json& j, const ImmVision::ColormapSettingsData& data) {
            j = json{
                {"Colormap", data.Colormap},
                {"ColormapScaleMin", data.ColormapScaleMin},
                {"ColormapScaleMax", data.ColormapScaleMax},
                {"ColormapScaleFromStats", data.ColormapScaleFromStats},
                {"internal_ColormapHovered", data.internal_ColormapHovered}
            };
        }

        static void from_json(const json& j, ImmVision::ColormapSettingsData& data) {
            j.at("Colormap").get_to(data.Colormap);
            j.at("ColormapScaleMin").get_to(data.ColormapScaleMin);
            j.at("ColormapScaleMax").get_to(data.ColormapScaleMax);
            j.at("ColormapScaleFromStats").get_to(data.ColormapScaleFromStats);
            j.at("internal_ColormapHovered").get_to(data.internal_ColormapHovered);
        }
    };

    template <>
    struct adl_serializer<ImmVision::ImageParams>
    {
        static void to_json(json& j, const ImmVision::ImageParams& params)
        {
            j = json{
                {"RefreshImage", params.RefreshImage},
                {"ImageDisplaySize", params.ImageDisplaySize},
                {"ZoomPanMatrix", params.ZoomPanMatrix},
                {"ZoomKey", params.ZoomKey},
                {"ColormapSettings", params.ColormapSettings},
                {"ColormapKey", params.ColormapKey},
                {"PanWithMouse", params.PanWithMouse},
                {"ZoomWithMouseWheel", params.ZoomWithMouseWheel},
                {"SelectedChannel", params.SelectedChannel},
                {"ShowSchoolPaperBackground", params.ShowSchoolPaperBackground},
                {"ShowAlphaChannelCheckerboard", params.ShowAlphaChannelCheckerboard},
                {"ShowGrid", params.ShowGrid},
                {"DrawValuesOnZoomedPixels", params.DrawValuesOnZoomedPixels},
                {"ShowImageInfo", params.ShowImageInfo},
                {"ShowPixelInfo", params.ShowPixelInfo},
                {"ShowZoomButtons", params.ShowZoomButtons},
                {"ShowOptionsPanel", params.ShowOptionsPanel},
                {"ShowOptionsInTooltip", params.ShowOptionsInTooltip},
                {"ShowOptionsButton", params.ShowOptionsButton},
                {"WatchedPixels", params.WatchedPixels},
                {"AddWatchedPixelOnDoubleClick", params.AddWatchedPixelOnDoubleClick},
                {"HighlightWatchedPixels", params.HighlightWatchedPixels}
            };
        }

        static void from_json(const json& j, ImmVision::ImageParams& params)
        {
            j.at("RefreshImage").get_to(params.RefreshImage);
            j.at("ImageDisplaySize").get_to(params.ImageDisplaySize);
            j.at("ZoomPanMatrix").get_to(params.ZoomPanMatrix);
            j.at("ZoomKey").get_to(params.ZoomKey);
            j.at("ColormapSettings").get_to(params.ColormapSettings);
            j.at("ColormapKey").get_to(params.ColormapKey);
            j.at("PanWithMouse").get_to(params.PanWithMouse);
            j.at("ZoomWithMouseWheel").get_to(params.ZoomWithMouseWheel);
            j.at("SelectedChannel").get_to(params.SelectedChannel);
            j.at("ShowSchoolPaperBackground").get_to(params.ShowSchoolPaperBackground);
            j.at("ShowAlphaChannelCheckerboard").get_to(params.ShowAlphaChannelCheckerboard);
            j.at("ShowGrid").get_to(params.ShowGrid);
            j.at("DrawValuesOnZoomedPixels").get_to(params.DrawValuesOnZoomedPixels);
            j.at("ShowImageInfo").get_to(params.ShowImageInfo);
            j.at("ShowPixelInfo").get_to(params.ShowPixelInfo);
            j.at("ShowZoomButtons").get_to(params.ShowZoomButtons);
            j.at("ShowOptionsPanel").get_to(params.ShowOptionsPanel);
            j.at("ShowOptionsInTooltip").get_to(params.ShowOptionsInTooltip);
            j.at("ShowOptionsButton").get_to(params.ShowOptionsButton);
            j.at("WatchedPixels").get_to(params.WatchedPixels);
            j.at("AddWatchedPixelOnDoubleClick").get_to(params.AddWatchedPixelOnDoubleClick);
            j.at("HighlightWatchedPixels").get_to(params.HighlightWatchedPixels);
        }

    };
} // namespace nlohmann


namespace ImmVision
{
    using json = nlohmann::json;


    std::string ImageParamsToJson(const ImageParams& params)
    {
        json j = params;
        return j.dump(4);
    }

    void FillImageParamsFromJson(const std::string& json, ImageParams* params)
    {
        nlohmann::json j = nlohmann::json::parse(json);
        nlohmann::adl_serializer<ImageParams>::from_json(j, *params);
    }

    ImageParams ImageParamsFromJson(const std::string& json)
    {
        ImageParams params;
        FillImageParamsFromJson(json, &params);
        return params;
    }

}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/imgui/image_widgets.cpp                                         //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>

namespace ImmVision
{
    namespace ImageWidgets
    {
        void GlTexture_Draw_DisableDragWindow(const GlTexture& texture, const ImVec2 &size, bool disableDragWindow)
        {
            ImVec2 size_(size);
            if (size.x == 0.f)
                size_ = texture.SizeImVec2();

            ImVec2 imageTl = ImGui::GetCursorScreenPos();
            ImVec2 imageBr(imageTl.x + size.x, imageTl.y + size.y);
            std::stringstream id;
            id << "##" << texture.TextureId;
            if (disableDragWindow)
                ImGui::InvisibleButton(id.str().c_str(), size);
            else
                ImGui::Dummy(size);
            ImGui::GetWindowDrawList()->AddImage(texture.TextureId, imageTl, imageBr);
        }

        float FontSizeRatio()
        {
            float r = ImGui::GetFontSize() / 14.5;
            return r;
        }

        Point2d DisplayTexture_TrackMouse(const GlTexture& texture, ImVec2 displaySize, bool disableDragWindow)
        {
            ImVec2 imageTopLeft = ImGui::GetCursorScreenPos();
            GlTexture_Draw_DisableDragWindow(texture, displaySize, disableDragWindow);
            bool isImageHovered = ImGui::IsItemHovered();
            ImVec2 mouse = ImGui::GetMousePos();
            if (isImageHovered)
                return Point2d((double)(mouse.x - imageTopLeft.x), (double)(mouse.y - imageTopLeft.y));
            else
                return Point2d(-1., -1.);
        }

        void ShowImageInfo(const ImageBuffer &image, double zoomFactor)
        {
            std::string info = MatrixInfoUtils::_MatInfo(image);
            ImGui::Text("%s - Zoom:%.3lf", info.c_str(), zoomFactor);
        }


        void ShowPixelColorWidget(
            const ImageBuffer &image,
            Point pt,
            const ImageParams& params)
        {
            bool isInImage = Rect(Point(0, 0), image.size()).contains(pt);
            auto UCharToFloat = [](int v) { return (float)((float) v / 255.f); };

            bool done = false;
            std::string id = std::string("##pixelcolor_") + std::to_string(pt.x) + "," + std::to_string(pt.y);
            if (image.depth == ImageDepth::uint8)
            {
                ImGuiColorEditFlags editFlags =
                    ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_AlphaPreviewHalf
                    | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Uint8;
                if (!isInImage)
                {
                    float dummyColor[4]{0.f, 0.f, 0.f, 255.f};
                    ImGui::SetNextItemWidth(1.f);
                    int colorEditFlags =
                        ImGuiColorEditFlags_NoInputs
                        | ImGuiColorEditFlags_InputRGB
                        | ImGuiColorEditFlags_DisplayRGB;
                    ImGui::ColorEdit4(id.c_str(), dummyColor, colorEditFlags );
                    done = true;
                }
                else if (image.channels == 3)
                {
                    const uint8_t* pixel = image.ptr<uint8_t>(pt.y) + pt.x * 3;
                    bool isColorOrderBgr = IsUsingBgrColorOrder();
                    ImVec4 colorAsImVec = isColorOrderBgr ?
                        ImVec4(UCharToFloat(pixel[2]), UCharToFloat(pixel[1]), UCharToFloat(pixel[0]), UCharToFloat(255))
                        : ImVec4(UCharToFloat(pixel[0]), UCharToFloat(pixel[1]), UCharToFloat(pixel[2]), UCharToFloat(255));
                    ImGui::SetNextItemWidth(150.f * FontSizeRatio());
                    ImGui::ColorEdit3(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
                else if (image.channels == 4)
                {
                    const uint8_t* pixel = image.ptr<uint8_t>(pt.y) + pt.x * 4;
                    bool isColorOrderBgr = IsUsingBgrColorOrder();
                    ImVec4 colorAsImVec = isColorOrderBgr ?
                        ImVec4(UCharToFloat(pixel[2]), UCharToFloat(pixel[1]), UCharToFloat(pixel[0]), UCharToFloat(pixel[3]))
                        : ImVec4(UCharToFloat(pixel[0]), UCharToFloat(pixel[1]), UCharToFloat(pixel[2]), UCharToFloat(pixel[3]));
                    ImGui::SetNextItemWidth(200.f * FontSizeRatio());
                    ImGui::ColorEdit4(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
            }
            if (! done)
            {
                std::string pixelInfo = MatrixInfoUtils::MatPixelColorInfo(image, pt.x, pt.y);
                ImGui::Text("%s", pixelInfo.c_str());
            }
        }


        // If true, the collapsing headers will be synced across instances
        bool s_CollapsingHeader_CacheState_Sync = false;

        bool CollapsingHeader_OptionalCacheState(const char *name, bool forceOpen)
        {
            static std::map<std::string, bool> collapsingHeadersState;
            bool shallOpen = forceOpen;
            if (s_CollapsingHeader_CacheState_Sync)
            {
                if (collapsingHeadersState.find(name) != collapsingHeadersState.end())
                {
                    bool wasOpenedLastTime = collapsingHeadersState.at(name);
                    if (wasOpenedLastTime)
                        shallOpen = true;
                }
            }
            if (shallOpen)
                ImGui::SetNextItemOpen(shallOpen, ImGuiCond_Always);
            bool opened = ImGui::CollapsingHeader(name);
            collapsingHeadersState[name] = opened;
            return opened;
        };

    } // namespace ImageWidgets

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/imgui/imgui_imm.cpp                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImGuiImm
{
    template<typename AnyFloat>
    bool SliderAnyFloat(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min,
        AnyFloat v_max,
        float width,
        bool logarithmic,
        int nb_decimals)
    {
        float vf = (float)*v;
        std::string formatString = std::string("%.") + std::to_string(nb_decimals) + "f";
        ImGui::SetNextItemWidth(width);
        ImGuiSliderFlags flags = 0;
        if (logarithmic)
            flags |= ImGuiSliderFlags_Logarithmic;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, formatString.c_str(), flags);
        if (changed)
            *v = (AnyFloat)vf;
        return changed;
    }

#define EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(AnyFloat)                       \
    template bool SliderAnyFloat<AnyFloat>(                                     \
    const char*label, AnyFloat* v, AnyFloat v_min, AnyFloat v_max, float width, \
    bool logarithmic, int nb_decimals);

    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(float);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(double);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT(long double);


    template<typename AnyFloat>
    bool SliderAnyFloatLogarithmic(
        const char*label,
        AnyFloat* v,
        AnyFloat v_min,
        AnyFloat v_max,
        float width,
        int nb_decimals)
    {
        return SliderAnyFloat(label, v, v_min, v_max, width, true, nb_decimals);
    }

#define EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(AnyFloat)                   \
    template bool SliderAnyFloatLogarithmic<AnyFloat>(                                  \
    const char*label, AnyFloat* v, AnyFloat v_min, AnyFloat v_max, float width,         \
    int nb_decimals);

    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(float);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(double);
    EXPLICIT_INSTANTIATION_SLIDER_ANY_FLOAT_LOGARITHMIC(long double);


    ImVec2 ComputeDisplayImageSize(
        ImVec2 askedImageSize,
        ImVec2 realImageSize
    )
    {
        if ((askedImageSize.x == 0.f) && (askedImageSize.y == 0.f))
            return realImageSize;
        else if ((askedImageSize.x == 0.f) && (realImageSize.y >= 1.f))
            return ImVec2(askedImageSize.y * realImageSize.x / realImageSize.y, askedImageSize.y);
        else if ((askedImageSize.y == 0.f) && (realImageSize.x >= 1.f))
            return ImVec2(askedImageSize.x, askedImageSize.x * realImageSize.y / realImageSize.x);
        else
            return askedImageSize;
    }
    ImmVision::Size ComputeDisplayImageSize(ImmVision::Size askedImageSize, ImmVision::Size realImageSize)
    {
        auto toSize = [](ImVec2 v) { return ImmVision::Size((int)((double)v.x + 0.5), (int)((double)v.y + 0.5)); };
        auto toImVec2 = [](ImmVision::Size v) { return ImVec2((float)v.width, (float)v.height); };
        return toSize( ComputeDisplayImageSize(toImVec2(askedImageSize), toImVec2(realImageSize)) );
    }

    void PushDisabled()
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.7f);

    }
    void PopDisabled()
    {
        ImGui::PopStyleVar();
        ImGui::PopItemFlag();
    }

    void SameLineAlignRight(float rightMargin, float alignRegionWidth)
    {
        auto window = ImGui::GetCurrentWindow();
        if (alignRegionWidth < 0.f)
            alignRegionWidth = window->Size.x;

        // Formulas taken from ImGui::ItemSize() code
        float xLineStart = IM_TRUNC(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
        float y = window->DC.CursorPosPrevLine.y;

        float x = xLineStart + alignRegionWidth - rightMargin;
        ImGui::SetCursorScreenPos({x, y});
    }



    // cf https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353
    static ImVector<ImRect> s_GroupPanelLabelStack;
    void BeginGroupPanel(const char* name, const ImVec2& size)
    {
        ImGui::BeginGroup();

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();
        ImGui::BeginGroup();

        ImVec2 effectiveSize = size;
        if (size.x < 0.0f)
            effectiveSize.x = ImGui::GetWindowWidth();
        else
            effectiveSize.x = size.x;
        ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        if (strlen(name) > 0)
            ImGui::TextUnformatted(name);

        auto labelMin = ImGui::GetItemRectMin();
        auto labelMax = ImGui::GetItemRectMax();
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
        ImGui::BeginGroup();

        //ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

        ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x          -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x         -= frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
        ImGui::GetCurrentWindow()->Size.x                   -= frameHeight;

        auto itemWidth = ImGui::CalcItemWidth();
        ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

        s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
    }

    void EndGroupPanel()
    {
        ImGui::PopItemWidth();

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();

        ImGui::EndGroup();

        //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

        ImGui::EndGroup();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

        ImGui::EndGroup();

        auto itemMin = ImGui::GetItemRectMin();
        auto itemMax = ImGui::GetItemRectMax();
        //ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

        auto labelRect = s_GroupPanelLabelStack.back();
        s_GroupPanelLabelStack.pop_back();

        ImVec2 halfFrame = ImVec2(frameHeight * 0.25f * 0.5f, frameHeight * 0.5f);
        ImRect frameRect = ImRect(ImVec2(itemMin.x + halfFrame.x, itemMin.y + halfFrame.y), ImVec2(itemMax.x - halfFrame.x, itemMax.y));
        labelRect.Min.x -= itemSpacing.x;
        labelRect.Max.x += itemSpacing.x;

        for (int i = 0; i < 4; ++i)
        {
            switch (i)
            {
                // left half-plane
                case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true); break;
                    // right half-plane
                case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true); break;
                    // top
                case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true); break;
                    // bottom
                case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true); break;
            }

            ImGui::GetWindowDrawList()->AddRect(
                frameRect.Min, frameRect.Max,
                ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
                halfFrame.x);

            ImGui::PopClipRect();
        }

        ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x          += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x         += frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
        ImGui::GetCurrentWindow()->Size.x                   += frameHeight;

        ImGui::Dummy(ImVec2(0.0f, 0.0f));

        ImGui::EndGroup();
    }


    static std::stack<bool> s_GroupPanel_FlagBorder_DrawBorder;
    static std::stack<std::string> s_GroupPanel_FlagBorder_Names;
    static std::unordered_map<std::string, ImVec2> s_GroupPanel_FlagBorder_Sizes;

    void BeginGroupPanel_FlagBorder(const char* name, bool draw_border, const ImVec2& size)
    {
        std::string name_s(name);
        std::string name_displayed;
        {
            auto pos = name_s.find("##");
            if (pos != std::string::npos)
                name_displayed =  name_s.substr(0, pos);
            else
                name_displayed = name_s;
        }

        ImGui::BeginGroup();
        s_GroupPanel_FlagBorder_DrawBorder.push(draw_border);
        s_GroupPanel_FlagBorder_Names.push(name);
        if (draw_border)
            BeginGroupPanel(name_displayed.c_str(), size);
        else
        {
            ImGui::BeginGroup();
            if (strlen(name) > 0)
                ImGui::Text("%s", name_displayed.c_str());
        }
    }

    void EndGroupPanel_FlagBorder()
    {
        bool drawBorder = s_GroupPanel_FlagBorder_DrawBorder.top();
        s_GroupPanel_FlagBorder_DrawBorder.pop();
        if (drawBorder)
            EndGroupPanel();
        else
            ImGui::EndGroup();

        ImGui::EndGroup();

        // Store size
        {
            std::string name = s_GroupPanel_FlagBorder_Names.top();
            s_GroupPanel_FlagBorder_Names.pop();
            s_GroupPanel_FlagBorder_Sizes[name] = ImGui::GetItemRectSize();
        }
    }

    ImVec2 GroupPanel_FlagBorder_LastKnownSize(const char* name)
    {
        if (s_GroupPanel_FlagBorder_Sizes.find(name) == s_GroupPanel_FlagBorder_Sizes.end())
            return ImVec2(3.f, 3.f);
        else
            return s_GroupPanel_FlagBorder_Sizes.at(name);
    }

    std::stack<ImRect> s_OldWorkRects;
    void BeginGroupFixedWidth(float width)
    {
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(width, 1.f));
        ImRect oldWorkRect = ImGui::GetCurrentWindow()->WorkRect;
        {
            ImRect newRect = oldWorkRect;
            newRect.Max.x = ImGui::GetCursorScreenPos().x + width - ImGui::GetStyle().ItemSpacing.x;
            ImGui::GetCurrentWindow()->WorkRect = newRect;
            s_OldWorkRects.push(oldWorkRect);
        }
    }

    void EndGroupFixedWidth()
    {
        ImGui::EndGroup();
        assert(!s_OldWorkRects.empty());
        ImRect oldWorkRect = s_OldWorkRects.top();
        s_OldWorkRects.pop();
        ImGui::GetCurrentWindow()->WorkRect = oldWorkRect;
    }

    // Draw a fixed width Separator
    // useful when ImGui::Separator() overflows to the right
    void SeparatorFixedWidth(float width)
    {
        ImVec2 a = ImGui::GetCursorScreenPos();
        a.y += 4.f;
        ImVec2 b = a;
        b.x += width;
        auto col = ImGui::GetStyle().Colors[ImGuiCol_Separator];
        ImGui::GetWindowDrawList()->AddLine(a, b, ImGui::GetColorU32(col), 1.f);
        ImGui::NewLine();
    }

    bool ButtonWithTooltip(const std::string& label, const std::string& tooltip)
    {
        bool r = ImGui::Button(label.c_str());
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", tooltip.c_str());
        return r;
    }

    void Theme_Debug()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.21f, 0.25f, 0.21f, 0.70f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.65f, 0.90f, 0.70f, 0.45f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.54f, 0.28f, 0.83f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.80f, 0.43f, 0.20f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.63f, 0.33f, 0.87f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.55f, 0.45f, 0.80f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.80f, 0.53f, 0.30f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.80f, 0.48f, 0.40f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.80f, 0.54f, 0.40f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.40f, 0.67f, 0.47f, 0.60f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.50f, 0.67f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.05f, 0.20f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.38f, 0.76f, 0.17f, 0.45f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.90f, 0.47f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.87f, 0.53f, 0.80f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    }


    // Theme CorporateGrey: https://github.com/ocornut/imgui/issues/707#issuecomment-468798935
    void Theme_CorporateGrey()
    {
        ImGuiStyle & style = ImGui::GetStyle();
        ImVec4 * colors = style.Colors;

        /// 0 = FLAT APPEARENCE
        /// 1 = MORE "3D" LOOK
        int is3D = 0;

        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
        colors[ImGuiCol_Separator]              = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavCursor]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

        style.PopupRounding = 3;

        style.WindowPadding = ImVec2(4, 4);
        style.FramePadding  = ImVec2(6, 4);
        style.ItemSpacing   = ImVec2(6, 2);

        style.ScrollbarSize = 18;

        style.WindowBorderSize = 1;
        style.ChildBorderSize  = 1;
        style.PopupBorderSize  = 1;
        style.FrameBorderSize  = (float)is3D;

        style.WindowRounding    = 3;
        style.ChildRounding     = 3;
        style.FrameRounding     = 3;
        style.ScrollbarRounding = 2;
        style.GrabRounding      = 3;

#ifdef IMGUI_HAS_DOCK
        style.TabBorderSize = (float)is3D;
        style.TabRounding   = 3;

        colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_Tab]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_TabSelected]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_TabDimmed]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_DockingPreview]     = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
#endif
    }

    void Theme_Dark()
    {
        //ImGui::GetIO().Fonts->AddFontFromFileTTF("../data/Fonts/Ruda-Bold.ttf", 15.0f, &config);
        ImGui::GetStyle().FrameRounding = 4.0f;
        ImGui::GetStyle().GrabRounding = 4.0f;

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_TabDimmed] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    }

    void Theme_EmbraceTheDarkness()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabSelected]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabDimmed]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabDimmedSelected]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavCursor]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding                     = ImVec2(8.00f, 8.00f);
        style.FramePadding                      = ImVec2(5.00f, 2.00f);
        style.CellPadding                       = ImVec2(6.00f, 6.00f);
        style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
        style.IndentSpacing                     = 25;
        style.ScrollbarSize                     = 15;
        style.GrabMinSize                       = 10;
        style.WindowBorderSize                  = 1;
        style.ChildBorderSize                   = 1;
        style.PopupBorderSize                   = 1;
        style.FrameBorderSize                   = 1;
        style.TabBorderSize                     = 1;
        style.WindowRounding                    = 7;
        style.ChildRounding                     = 4;
        style.FrameRounding                     = 3;
        style.PopupRounding                     = 4;
        style.ScrollbarRounding                 = 9;
        style.GrabRounding                      = 3;
        style.LogSliderDeadzone                 = 4;
        style.TabRounding                       = 4;
    }

    // https://github.com/ocornut/imgui/issues/707#issuecomment-226993714
    void Theme_Binks( bool bStyleDark_, float alpha_  )
    {
        ImGuiStyle& style = ImGui::GetStyle();

        // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
        style.Alpha = 1.0f;
        style.FrameRounding = 3.0f;
        style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
        style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
        style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
        style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

        if( bStyleDark_ )
        {
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                ImVec4& col = style.Colors[i];
                float H, S, V;
                ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

                if( S < 0.1f )
                {
                    V = 1.0f - V;
                }
                ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
                if( col.w < 1.00f )
                {
                    col.w *= alpha_;
                }
            }
        }
        else
        {
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                ImVec4& col = style.Colors[i];
                if( col.w < 1.00f )
                {
                    col.x *= alpha_;
                    col.y *= alpha_;
                    col.z *= alpha_;
                    col.w *= alpha_;
                }
            }
        }
    }
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/inspector.cpp                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace ImmVision
{
    static ImVec2 gInspectorImageSize(0.f, 0.f);

    struct Inspector_ImageAndParams
    {
        ImageCache::KeyType id;
        std::string Label;
        ImageBuffer Image;
        ImageParams Params;

        Point2d InitialZoomCenter = Point2d();
        double InitialZoomRatio = 1.;
        bool WasSentToTextureCache = false;
    };

    static std::vector<Inspector_ImageAndParams> s_Inspector_ImagesAndParams;
    ImageCache::ImageTextureCache sInspectorImageTextureCache(3600. * 24. * 365. * 10.); // 10 years TTL!
    static size_t s_Inspector_CurrentIndex = 0;

    // In the inspector, we cannot rely on the ID stack, since calls to AddImages will have a different stack
    // than when will later display the image
    static bool sDontUseIdStack = false;


    void Inspector_AddImage(
        const ImageBuffer& image_buf,
        const std::string& legend,
        const std::string& zoomKey,
        const std::string& colormapKey,
        const Point2d & zoomCenter,
        double zoomRatio
    )
    {
        ImageBuffer image = image_buf.clone();
        ImageParams params;
        params.ZoomKey = zoomKey;
        params.ColormapKey = colormapKey;
        params.ShowOptionsPanel = true;

        if (gInspectorImageSize.x > 0.f)
            params.ImageDisplaySize = Size((int)gInspectorImageSize.x, (int)gInspectorImageSize.y);

        std::string label = legend + "##" + std::to_string(s_Inspector_ImagesAndParams.size());
        auto id = sInspectorImageTextureCache.GetID(label, sDontUseIdStack);
        s_Inspector_ImagesAndParams.push_back({id, label, image, params, zoomCenter, zoomRatio});

        // bump cache
        {
            auto& imageAndParams = s_Inspector_ImagesAndParams.back();
            sInspectorImageTextureCache.UpdateCache(id, imageAndParams.Image, &imageAndParams.Params, true);
        }
    }

    void priv_Inspector_ShowImagesListbox(float width)
    {
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        int idxSuppress = -1;
        if (ImGui::BeginListBox("##ImageList",
                                ImVec2(width - 10.f, ImGui::GetContentRegionAvail().y)))
        {
            for (size_t i = 0; i < s_Inspector_ImagesAndParams.size(); ++i)
            {
                ImGui::PushID(i * 3424553);

                auto& imageAndParams = s_Inspector_ImagesAndParams[i];

                const bool is_selected = (s_Inspector_CurrentIndex == i);

                auto id = sInspectorImageTextureCache.GetID(imageAndParams.Label, sDontUseIdStack);
                auto &cacheImage = sInspectorImageTextureCache.GetCacheImageAndTexture(id);

                ImVec2 itemSize(width - 10.f, ImGui::GetFontSize() * 3.f);
                float imageHeight = itemSize.y - ImGui::GetTextLineHeight();
                ImVec2 pos = ImGui::GetCursorScreenPos();

                {
                    auto col = ImGui::GetStyle().Colors[ImGuiCol_Button];
                    col.x = 1.;
                    ImGui::PushStyleColor(ImGuiCol_Button, col);
                    if (ImGui::SmallButton("x"))
                        idxSuppress = i;
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                }

                std::string id_selectable = imageAndParams.Label + "##_" + std::to_string(i);
                if (ImGui::Selectable(id_selectable.c_str(), is_selected, 0, itemSize))
                    s_Inspector_CurrentIndex = i;

                float imageRatio = cacheImage.mGlTexture->SizeImVec2().x / cacheImage.mGlTexture->SizeImVec2().y;
                ImVec2 image_tl(pos.x, pos.y + ImGui::GetTextLineHeight());
                ImVec2 image_br(pos.x + imageRatio * imageHeight, image_tl.y + imageHeight);

                ImGui::GetWindowDrawList()->AddImage(cacheImage.mGlTexture->TextureId, image_tl, image_br);

                ImGui::PopID();
            }
            ImGui::EndListBox();

            if (idxSuppress >= 0)
                s_Inspector_ImagesAndParams.erase(s_Inspector_ImagesAndParams.begin() + (size_t)idxSuppress);
        }
    };

    void priv_Inspector_CleanImagesParams(const ImVec2& imageSize)
    {
        for (auto& i :s_Inspector_ImagesAndParams)
        {
            // Force image size
            i.Params.ImageDisplaySize = Size((int)imageSize.x, (int)imageSize.y);

            // Store in texture cache
            if (! i.WasSentToTextureCache)
            {
                if (i.InitialZoomRatio > 0.)
                {
                    i.Params.ZoomPanMatrix = ZoomPanTransform::MakeZoomMatrix(
                        i.InitialZoomCenter, i.InitialZoomRatio, i.Params.ImageDisplaySize);
                }

                sInspectorImageTextureCache.UpdateCache(i.id, i.Image, &i.Params, true);
                i.WasSentToTextureCache = true;
            }
        }

        // Propagate current options to hidden images
        if (s_Inspector_CurrentIndex < s_Inspector_ImagesAndParams.size())
        {
            const auto& currentParams = s_Inspector_ImagesAndParams[s_Inspector_CurrentIndex].Params;
            for (auto& v : s_Inspector_ImagesAndParams)
            {
                v.Params.ShowImageInfo = currentParams.ShowImageInfo;
                v.Params.ShowPixelInfo = currentParams.ShowPixelInfo;
                v.Params.ShowZoomButtons = currentParams.ShowZoomButtons;
                v.Params.ShowOptionsPanel = currentParams.ShowOptionsPanel;
                v.Params.ShowOptionsInTooltip = currentParams.ShowOptionsInTooltip;
                v.Params.PanWithMouse = currentParams.PanWithMouse;
                v.Params.ZoomWithMouseWheel = currentParams.ZoomWithMouseWheel;
                v.Params.AddWatchedPixelOnDoubleClick = currentParams.AddWatchedPixelOnDoubleClick;
            }
        }
    };

    ImVec2 priv_Inspector_ImageSize(float listWidth, bool showOptionsColumn)
    {
        ImVec2 imageSize;

        float emSize = ImGui::GetFontSize();
        float x_margin = emSize * 2.f;
        float y_margin = emSize / 3.f;
        float image_info_height =  ImGui::GetFontSize() * 10.f;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto &params = s_Inspector_ImagesAndParams.front().Params;
            if (!params.ShowImageInfo)
                image_info_height -= emSize * 1.5f;
            if (!params.ShowPixelInfo)
                image_info_height -= emSize * 1.5f;
        }
        float image_options_width = showOptionsColumn ? ImGui::GetFontSize() * 19.f : 0.f;
        ImVec2 winSize = ImGui::GetWindowSize();
        imageSize = ImVec2(
            winSize.x - listWidth - x_margin - image_options_width,
            winSize.y - y_margin - image_info_height);
        if (imageSize.x < 1.f)
            imageSize.x = 1.f;
        if (imageSize.y < 1.f)
            imageSize.y = 1.f;

        gInspectorImageSize = imageSize;
        return imageSize;
    };


    void Inspector_Show()
    {
        ImageWidgets::s_CollapsingHeader_CacheState_Sync = true;

        bool showOptionsColumn = true;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto& params = s_Inspector_ImagesAndParams.front().Params;
            if ( (params.ShowOptionsInTooltip) || (!params.ShowOptionsPanel))
                showOptionsColumn = false;
        }

        static float initialListWidth = ImGui::GetFontSize() * 8.5f;
        static float currentListWidth = initialListWidth;

        ImVec2 imageSize = priv_Inspector_ImageSize(currentListWidth, showOptionsColumn);
        priv_Inspector_CleanImagesParams(imageSize);

        ImGui::Columns(2);

        //
        // First column: image list
        //
        {
            // Set column width
            {
                static int idxFrame = 0;
                ++idxFrame;
                if (idxFrame <= 2) // The column width is not set at the first frame
                    ImGui::SetColumnWidth(0, initialListWidth);
                ImGui::Text("Image list");
                currentListWidth = ImGui::GetColumnWidth(0);
            }
            // Show image list
            priv_Inspector_ShowImagesListbox(currentListWidth);
        }

        ImGui::NextColumn();

        //
        // Second column : image
        //
        {
            if (s_Inspector_ImagesAndParams.empty())
                s_Inspector_CurrentIndex = 0;
            else if (s_Inspector_CurrentIndex >= s_Inspector_ImagesAndParams.size())
                s_Inspector_CurrentIndex = s_Inspector_ImagesAndParams.size() - 1;

            if (s_Inspector_CurrentIndex < s_Inspector_ImagesAndParams.size())
            {
                auto& imageAndParams = s_Inspector_ImagesAndParams[s_Inspector_CurrentIndex];
                ImmVision::Image(imageAndParams.Label, imageAndParams.Image, &imageAndParams.Params);
            }
        }

        ImGui::Columns(1);

        ImageWidgets::s_CollapsingHeader_CacheState_Sync = false;
    }

    void Inspector_ClearImages()
    {
        s_Inspector_ImagesAndParams.clear();
        sInspectorImageTextureCache.ClearImagesCache();
        s_Inspector_CurrentIndex = 0;
    }

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/immvision_to_string.cpp                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/immvision_to_string.h included by src/immvision/internal/misc/immvision_to_string.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    // <autogen:tostring_decl> // Autogenerated code below! Do not edit!

    std::string ToString(const ColormapScaleFromStatsData& params);
    std::string ToString(const ColormapSettingsData& params);
    std::string ToString(const MouseInformation& params);
    std::string ToString(const ImageParams& params);

    // </autogen:tostring_decl> // Autogenerated code end
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/immvision_to_string.cpp continued                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    std::string ToString(ColorMapStatsTypeId id)
    {
        if (id == ColorMapStatsTypeId::FromFullImage)
            return "FromFullImage";
        else
            return "FromVisibleROI";
    }

    std::string ToString(const ColormapScaleFromStatsData& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "ColormapScaleFromStatsData\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "ColorMapStatsType: " + ToString(v.ColorMapStatsType) + "\n";
        inner = inner + "NbSigmas: " + ToString(v.NbSigmas) + "\n";
        inner = inner + "UseStatsMin: " + ToString(v.UseStatsMin) + "\n";
        inner = inner + "UseStatsMax: " + ToString(v.UseStatsMax) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }
    
    std::string ToString(const ColormapSettingsData& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "ColormapSettingsData\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "Colormap: " + ToString(v.Colormap) + "\n";
        inner = inner + "ColormapScaleMin: " + ToString(v.ColormapScaleMin) + "\n";
        inner = inner + "ColormapScaleMax: " + ToString(v.ColormapScaleMax) + "\n";
        inner = inner + "ColormapScaleFromStats: " + ToString(v.ColormapScaleFromStats) + "\n";
        inner = inner + "internal_ColormapHovered: " + ToString(v.internal_ColormapHovered) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }
    
    std::string ToString(const MouseInformation& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "MouseInformation\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "IsMouseHovering: " + ToString(v.IsMouseHovering) + "\n";
        inner = inner + "MousePosition: " + ToString(v.MousePosition) + "\n";
        inner = inner + "MousePosition_Displayed: " + ToString(v.MousePosition_Displayed) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }

    std::string ToString(const ImageParams& v)
    {

        using namespace ImmVision::StringUtils;
        
        std::string r;
        r += "ImageParams\n";
        r += "{\n";
    
        std::string inner;

        inner = inner + "RefreshImage: " + ToString(v.RefreshImage) + "\n";
        inner = inner + "ImageDisplaySize: " + ToString(v.ImageDisplaySize) + "\n";
        inner = inner + "ZoomPanMatrix: " + ToString(v.ZoomPanMatrix) + "\n";
        inner = inner + "ZoomKey: " + ToString(v.ZoomKey) + "\n";
        inner = inner + "ColormapSettings: " + ToString(v.ColormapSettings) + "\n";
        inner = inner + "ColormapKey: " + ToString(v.ColormapKey) + "\n";
        inner = inner + "PanWithMouse: " + ToString(v.PanWithMouse) + "\n";
        inner = inner + "ZoomWithMouseWheel: " + ToString(v.ZoomWithMouseWheel) + "\n";
        inner = inner + "SelectedChannel: " + ToString(v.SelectedChannel) + "\n";
        inner = inner + "ShowSchoolPaperBackground: " + ToString(v.ShowSchoolPaperBackground) + "\n";
        inner = inner + "ShowAlphaChannelCheckerboard: " + ToString(v.ShowAlphaChannelCheckerboard) + "\n";
        inner = inner + "ShowGrid: " + ToString(v.ShowGrid) + "\n";
        inner = inner + "DrawValuesOnZoomedPixels: " + ToString(v.DrawValuesOnZoomedPixels) + "\n";
        inner = inner + "ShowImageInfo: " + ToString(v.ShowImageInfo) + "\n";
        inner = inner + "ShowPixelInfo: " + ToString(v.ShowPixelInfo) + "\n";
        inner = inner + "ShowZoomButtons: " + ToString(v.ShowZoomButtons) + "\n";
        inner = inner + "ShowOptionsPanel: " + ToString(v.ShowOptionsPanel) + "\n";
        inner = inner + "ShowOptionsInTooltip: " + ToString(v.ShowOptionsInTooltip) + "\n";
        inner = inner + "ShowOptionsButton: " + ToString(v.ShowOptionsButton) + "\n";
        inner = inner + "WatchedPixels: " + ToString(v.WatchedPixels) + "\n";
        inner = inner + "AddWatchedPixelOnDoubleClick: " + ToString(v.AddWatchedPixelOnDoubleClick) + "\n";
        inner = inner + "HighlightWatchedPixels: " + ToString(v.HighlightWatchedPixels) + "\n";
        inner = inner + "MouseInfo: " + ToString(v.MouseInfo) + "\n";

        r = r + IndentLines(inner, 4);
        r += "}";
        return r;
    }
    
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/insertion_order_map.cpp                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/math_utils.cpp                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace MathUtils
    {
        double MaximumValue(const std::vector<double> &v)
        {
            return *std::min_element(v.begin(), v.end());
        }

        double MinimumValue(const std::vector<double> &v)
        {
            return *std::max_element(v.begin(), v.end());
        }

        int RoundInt(double v)
        {
            return (int) std::round(v);
        }

    } // namespace MathUtils
} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/panic.cpp                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/debugbreak.h included by src/immvision/internal/misc/panic.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From https://github.com/scottt/debugbreak
// https://raw.githubusercontent.com/scottt/debugbreak/master/debugbreak.h
//
// Thanks to Scott Tsai !

/* Copyright (c) 2011-2021, Scott Tsai
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef DEBUG_BREAK_H
#define DEBUG_BREAK_H

#ifdef _MSC_VER

#define debug_break __debugbreak

#else

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_BREAK_USE_TRAP_INSTRUCTION 1
#define DEBUG_BREAK_USE_BULTIN_TRAP      2
#define DEBUG_BREAK_USE_SIGTRAP          3

#if defined(__i386__) || defined(__x86_64__)
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__inline__ static void trap_instruction(void)
{
    __asm__ volatile("int $0x03");
}
#elif defined(__thumb__)
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
/* FIXME: handle __THUMB_INTERWORK__ */
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'arm-linux-tdep.c' in GDB source.
	 * Both instruction sequences below work. */
#if 1
	/* 'eabi_linux_thumb_le_breakpoint' */
	__asm__ volatile(".inst 0xde01");
#else
	/* 'eabi_linux_thumb2_le_breakpoint' */
	__asm__ volatile(".inst.w 0xf7f0a000");
#endif

	/* Known problem:
	 * After a breakpoint hit, can't 'stepi', 'step', or 'continue' in GDB.
	 * 'step' would keep getting stuck on the same instruction.
	 *
	 * Workaround: use the new GDB commands 'debugbreak-step' and
	 * 'debugbreak-continue' that become available
	 * after you source the script from GDB:
	 *
	 * $ gdb -x debugbreak-gdb.py <... USUAL ARGUMENTS ...>
	 *
	 * 'debugbreak-step' would jump over the breakpoint instruction with
	 * roughly equivalent of:
	 * (gdb) set $instruction_len = 2
	 * (gdb) tbreak *($pc + $instruction_len)
	 * (gdb) jump   *($pc + $instruction_len)
	 */
}
#elif defined(__arm__) && !defined(__thumb__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'arm-linux-tdep.c' in GDB source,
	 * 'eabi_linux_arm_le_breakpoint' */
	__asm__ volatile(".inst 0xe7f001f0");
	/* Known problem:
	 * Same problem and workaround as Thumb mode */
}
#elif defined(__aarch64__) && defined(__APPLE__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_BULTIN_DEBUGTRAP
#elif defined(__aarch64__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'aarch64-tdep.c' in GDB source,
	 * 'aarch64_default_breakpoint' */
	__asm__ volatile(".inst 0xd4200000");
}
#elif defined(__powerpc__)
	/* PPC 32 or 64-bit, big or little endian */
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'rs6000-tdep.c' in GDB source,
	 * 'rs6000_breakpoint' */
	__asm__ volatile(".4byte 0x7d821008");

	/* Known problem:
	 * After a breakpoint hit, can't 'stepi', 'step', or 'continue' in GDB.
	 * 'step' stuck on the same instruction ("twge r2,r2").
	 *
	 * The workaround is the same as ARM Thumb mode: use debugbreak-gdb.py
	 * or manually jump over the instruction. */
}
#elif defined(__riscv)
	/* RISC-V 32 or 64-bit, whether the "C" extension
	 * for compressed, 16-bit instructions are supported or not */
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'riscv-tdep.c' in GDB source,
	 * 'riscv_sw_breakpoint_from_kind' */
	__asm__ volatile(".4byte 0x00100073");
}
#else
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_SIGTRAP
#endif


#ifndef DEBUG_BREAK_IMPL
#error "debugbreak.h is not supported on this target"
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
    trap_instruction();
}
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_BULTIN_DEBUGTRAP
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
	__builtin_debugtrap();
}
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_BULTIN_TRAP
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
	__builtin_trap();
}
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_SIGTRAP
#include <signal.h>
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
	raise(SIGTRAP);
}
#else
#error "invalid DEBUG_BREAK_IMPL value"
#endif

#ifdef __cplusplus
}
#endif

#endif /* ifdef _MSC_VER */

#endif /* ifndef DEBUG_BREAK_H */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/panic.cpp continued                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    //
    // If you arrive here, it is likely that something very wrong happened
    //

    void Cleanup()
    {
        // Reset the caches
        ImmVision_GlProvider::ResetGlProvider();
    }

    void Panic_UnknownCause()
    {
        std::cerr << "ImmVision: Panic! " << "\n";
        // Put a debugger breakpoint, to help diagnose the issue
#ifndef NDEBUG
        debug_break();
#endif
        // Do some cleanup
        Cleanup();
        std::cerr << "ImmVision: Panic! => Did Cleanup" << "\n";
        // And die...
        std::cerr << "ImmVision: Panic! => will terminate!" << "\n";
        std::terminate();
    }

    void Panic(const std::exception& e)
    {
        std::cerr << "ImmVision: Panic! " << e.what() << "\n";
        // Put a debugger breakpoint, to help diagnose the issue
#ifndef NDEBUG
        debug_break();
#endif
        // Do some cleanup
        Cleanup();
        std::cerr << "ImmVision: Panic! => Did Cleanup" << "\n";
        // And rethrow
        std::cerr << "ImmVision: Panic! => re-throw!" << "\n";
        throw;
    }
} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/misc/string_utils.cpp                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace ImmVision
{
    namespace StringUtils
    {
        std::string LowerString(const std::string& s)
        {
            // <rant>
            // Welcome to a (C++) world where everyone is welcomed, asked, forced or awkwardly proud
            // to reinvent a wheel that was "left as an exercise".
            //
            // This is probably the implementation number 1,000,001 in the world.
            // Let' hope it is not broken, and does not explode in a buffer overflow exploit 10 years from now.
            // </rant>
            auto sane_tolower_char = [](char c) -> char
            {
                // See, the exercise was easy!
                return static_cast<char>(::tolower(static_cast<int>(c)));
            };
            std::string r = s;
            std::transform(r.begin(), r.end(), r.begin(), sane_tolower_char);
            return r;
        }

        std::vector<std::string> SplitString(const std::string &s, char delimiter)
        {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter))
                tokens.push_back(token);
            return tokens;
        }

        std::string JoinStrings(const std::vector<std::string>&v, const std::string& separator)
        {
            std::string r;
            for (size_t i = 0; i < v.size(); ++ i)
            {
                r += v[i];
                if (i < v.size() - 1)
                    r += separator;
            }
            return r;
        }

        std::string IndentLine(const std::string& s, int indentSize)
        {
            return std::string((size_t)indentSize, ' ') + s;
        }

        std::string IndentLines(const std::string& s, int indentSize)
        {
            auto lines = SplitString(s, '\n');
            std::string r = "";
            for (auto line: lines)
                r = r + IndentLine(line, indentSize) + "\n";
            return r;
        }


        std::string ToString(const std::string& s)
        {
            return "\"" + s + "\"";
        }

        std::string ToString(const double& v)
        {
            char buf[200];
            snprintf(buf, 200, "%7G", v);
            return buf;
        }

        std::string ToString(const int& v)
        {
            return std::to_string(v);
        }

        std::string ToString(const float& v)
        {
            return ToString((double)v);
        }

        std::string ToString(bool v)
        {
            return (v ? "true" : "false");
        }

    } // namespace StringUtils
} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/stb/stb_image_impl.cpp                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stb_image implementation for standalone immvision builds.
// When built with hello_imgui, the implementation is already compiled there,
// so this file compiles to nothing (IMMVISION_PROVIDE_STB_IMAGE is not defined).
#ifdef IMMVISION_PROVIDE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/stb/stb_image_write_impl.cpp                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stb_image_write implementation for standalone immvision builds.
// When built with hello_imgui, the implementation is already compiled there,
// so this file compiles to nothing (IMMVISION_PROVIDE_STB_IMAGE_WRITE is not defined).
#ifdef IMMVISION_PROVIDE_STB_IMAGE_WRITE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

