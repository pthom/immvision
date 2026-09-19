#pragma once
#include <cstddef>   // size_t
#include <cstdint>   // uint8_t
#include <memory>    // shared_ptr
#include <vector>
#include <algorithm> // std::min, std::max
#include <cstring>   // memcpy
#include <string>

#ifdef IMMVISION_HAS_OPENCV
#include <opencv2/core.hpp>
#include <stdexcept>
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
    //     If your application uses OpenCV, define IMMVISION_HAS_OPENCV before including ImmVision
    //     (preferably for the whole target), and link OpenCV in your application, e.g. with CMake:
    //         find_package(OpenCV REQUIRED)
    //         target_compile_definitions(my_app PRIVATE IMMVISION_HAS_OPENCV)
    //         target_link_libraries(my_app PRIVATE opencv_core)
    //     All types then provide implicit conversions to/from their OpenCV equivalents
    //     (these conversions are header-only):
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
        Matrix33d(const cv::Matx33d& mat);
        operator cv::Matx33d() const;
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
    //     If IMMVISION_HAS_OPENCV is defined, you can pass cv::Mat directly to any ImmVision function
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

        // Non-owning view from a raw pointer.
        // Wraps existing pixel data without copying or taking ownership.
        // The caller must ensure the data stays alive while this ImageBuffer is used.
        // If step is 0, assumes contiguous rows (step = width * channels * element_size).
        //
        // Works with any image source: stb_image, SDL_Surface, custom buffers, etc.
        //
        // Example with stb_image:
        //     int w, h, ch;
        //     unsigned char* pixels = stbi_load("photo.jpg", &w, &h, &ch, 0);
        //     ImmVision::Image("photo", ImmVision::ImageBuffer(pixels, w, h, ch), &params);
        //     stbi_image_free(pixels);
        //
        // Example with SDL_Surface:
        //     ImmVision::ImageBuffer(surface->pixels, surface->w, surface->h, 4,
        //                            ImmVision::ImageDepth::uint8, surface->pitch);
        ImageBuffer(void* data, int width, int height, int channels,
                    ImageDepth depth = ImageDepth::uint8, size_t step = 0)
            : data(data), width(width), height(height), channels(channels), depth(depth),
              step(step ? step : (size_t)width * channels * ImageDepthSize(depth))
        {}

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
        ImageBuffer(const cv::Mat& mat);
        // Zero-copy view: valid while this ImageBuffer lives
        cv::Mat to_cv_mat() const;
        // Deep copy: safe even after ImageBuffer is destroyed
        cv::Mat to_cv_mat_clone() const;
        #endif
    };


#ifdef IMMVISION_HAS_OPENCV
    //
    // OpenCV conversions (header-only, compiled in the application, not in ImmVision)
    //
    namespace Internal
    {
        inline ImageDepth CvDepthToImageDepth(int cv_depth)
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
                    throw std::runtime_error("CvDepthToImageDepth: unsupported cv depth");
            }
        }

        inline int ImageDepthToCvDepth(ImageDepth depth)
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
            throw std::runtime_error("ImageDepthToCvDepth: unknown depth");
        }
    } // namespace Internal

    inline Matrix33d::Matrix33d(const cv::Matx33d& mat)
    {
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                m[r][c] = mat(r, c);
    }

    inline Matrix33d::operator cv::Matx33d() const
    {
        cv::Matx33d r;
        for (int r_ = 0; r_ < 3; r_++)
            for (int c = 0; c < 3; c++)
                r(r_, c) = m[r_][c];
        return r;
    }

    inline ImageBuffer::ImageBuffer(const cv::Mat& mat)
    {
        // Ensure contiguous memory: clone if needed (e.g. ROI sub-matrices)
        cv::Mat continuous = mat.isContinuous() ? mat : mat.clone();
        data = continuous.data;
        width = continuous.cols;
        height = continuous.rows;
        channels = continuous.channels();
        depth = Internal::CvDepthToImageDepth(continuous.depth());
        step = continuous.step[0];
        // Keep the cv::Mat header alive: its refcount keeps the pixel data alive
        _ref_keeper = std::make_shared<cv::Mat>(continuous);
    }

    inline cv::Mat ImageBuffer::to_cv_mat() const
    {
        if (empty())
            return cv::Mat();
        int cv_type = CV_MAKETYPE(Internal::ImageDepthToCvDepth(depth), channels);
        return cv::Mat(height, width, cv_type, data, step);
    }

    inline cv::Mat ImageBuffer::to_cv_mat_clone() const
    {
        return to_cv_mat().clone();
    }
#endif // IMMVISION_HAS_OPENCV

} // namespace ImmVision
