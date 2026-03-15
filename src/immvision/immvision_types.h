#pragma once
#include <cstddef>   // size_t
#include <cstdint>   // uint8_t
#include <memory>    // shared_ptr
#include <vector>

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
        bool operator==(const Point2d& o) const { return x == o.x && y == o.y; }
        bool operator!=(const Point2d& o) const { return !(*this == o); }
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

        #ifdef IMMVISION_HAS_OPENCV
        IMMVISION_API Matrix33d(const cv::Matx33d& mat);
        IMMVISION_API operator cv::Matx33d() const;
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
