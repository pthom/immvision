// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.h                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision_types.h included by src/immvision/immvision.h                  //
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
        IMMVISION_API ImageBuffer(const cv::Mat& mat);
        // Zero-copy view: valid while this ImageBuffer lives
        IMMVISION_API cv::Mat to_cv_mat() const;
        // Deep copy: safe even after ImageBuffer is destroyed
        IMMVISION_API cv::Mat to_cv_mat_clone() const;
        #endif
    };

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.h continued                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.h included by src/immvision/immvision.h                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "imgui.h"


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
//                       src/immvision/immvision.h continued                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/inspector.h included by src/immvision/immvision.h                        //
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
//                       src/immvision/immvision.h continued                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/gl_texture.h included by src/immvision/immvision.h                       //
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
