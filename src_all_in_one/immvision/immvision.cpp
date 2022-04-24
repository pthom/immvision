// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.cpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.h included by src/immvision/image.cpp                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <opencv2/core.hpp>
#include "imgui.h"

/**
@@md#ImmVision::Image

 __ImmVision::Image()__ will show an image from a cv::Mat.

Two signatures are provided:

* `ImmVision::Image(const cv::Mat &mat, bool refresh, const cv::Size& size = cv::Size(0, 0))`:
   * `mat` is the image to show
   * `refresh` indicates whether this cv::Mat was updated since it was last shown.
     If refresh is false, then the corresponding displayed texture will be fetched from a cache
     (i.e the cv::Mat data is not transferred again to the GPU)
     If refresh is true, then the corresponding displayed texture will be refreshed.
   * `size` is the desired display size.
        * If it is (0,0) then the actual cv::Mat size will be used.
        * If it is (w,0) then the display width will be w, and the displayed image height
          will conform to the vertical/horizontal ratio of the image size.
        * If it is (0,h) then the display height will be h, and the displayed image width
          will conform to the vertical/horizontal ratio of the image size.
        * If it is (w,h) then this is the displayed size

* `ImmVision::ImageStill(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0))`:
    Same a 'ImmVision::Image` where refresh is set to false

* `ImmVision::ImageAnimated(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0))`:
    Same a 'ImmVision::Image` where refresh is set to true

* `cv::Point2d ImmVision::GetImageMousePos()`: returns the position of the mouse on the last displayed image.
 This position is in *unscaled image coords*, i.e if the image was displayed with a smaller/bigger size, the returned
 mouse position is still between (0,0) and (image.cols, image.rows)

* `ImmVision::ClearTextureCache()`: clear the internal GPU texture cache. You should rarely/never need to do it.
  (the cache is handled automatically, and older GPU textures are cleared from the cache after 3 seconds if they are
  not displayed anymore)
@@md
*/

namespace ImmVision
{
    void Image(
        const cv::Mat &mat,
        bool refresh,
        const cv::Size& size = cv::Size(0, 0),
        bool isBgrOrBgra = true
    );

    inline void ImageStill(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0), bool isBgrOrBgra = true)
        { Image(mat, false, size, isBgrOrBgra); }
    inline void ImageAnimated(const cv::Mat &mat, const cv::Size& size = cv::Size(0, 0), bool isBgrOrBgra = true)
        { Image(mat, true, size, isBgrOrBgra); }

    cv::Point2d GetImageMousePos();

    void ClearImageTextureCache();
    void ClearAllTextureCaches();

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.cpp continued                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl_texture.h included by src/immvision/image.cpp                //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <memory>

namespace ImmVision
{
    /// GlTexture holds a OpenGL Texture (created via glGenTextures)
    /// You can blit (i.e transfer) image buffer onto it.
    /// The linked OpenGL texture lifetime is linked to this.
    /// GlTexture is not copiable (since it holds a reference to a texture stored on the GPU)
    struct GlTexture
    {
        GlTexture();
        virtual ~GlTexture();

        // non copiable
        GlTexture(const GlTexture& ) = delete;
        GlTexture& operator=(const GlTexture& ) = delete;

        void Draw(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0)) const;
        bool DrawButton(const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1)) const;
        void Draw_DisableDragWindow(const ImVec2& size = ImVec2(0, 0)) const;

        void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height);

        // members
        ImVec2 mImageSize;
        unsigned int mImTextureId;
    };


    struct GlTextureCv : public GlTexture
    {
        GlTextureCv() = default;
        GlTextureCv(const cv::Mat& mat, bool isBgrOrBgra);
        ~GlTextureCv() override = default;

        void BlitMat(const cv::Mat& mat, bool isBgrOrBgra);
    };

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.cpp continued                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/imgui_imm.h included by src/immvision/image.cpp                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Some extensions to ImGui, specific to ImmVision
namespace ImGuiImm
{
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags);

    ImVec2 ComputeDisplayImageSize(ImVec2 askedImageSize, ImVec2 realImageSize);
    cv::Size ComputeDisplayImageSize(cv::Size askedImageSize, cv::Size realImageSize);

    void PushDisabled();
    void PopDisabled();
    void SameLineAlignRight(float rightMargin = 0.f, float alignRegionWidth = -1.f);

    // cf https://github.com/ocornut/imgui/issues/1496#issuecomment-655048353
    void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel();

    void BeginGroupPanel_FlagBorder(const char* name, bool draw_border, const ImVec2& size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel_FlagBorder();
    ImVec2 GroupPanel_FlagBorder_LastKnownSize(const char* name);

    void BeginGroupFixedWidth(float width);
    void EndGroupFixedWidth();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.cpp continued                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.h included by src/immvision/image.cpp                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.h included by src/immvision/immvision.h                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>

namespace ImmVision
{
    //
    // struct ColorAdjustmentsValues: Color adjustments for float matrixes
    //
    // For float matrixes, pre-multiply and add a Delta before displaying
    // (the navigator expects values between 0 and 1, and will convert them to 0..255)
    struct ColorAdjustmentsValues
    {
        double Factor = 1., Delta = 0.;
    };

    
    //
    // struct ImageNavigatorParams
    //
    // Stores all the display parameters and options for an ImageNavigator,
    // (its parameters default values are reasonable choices)
    struct ImageNavigatorParams
    {
        //
        // Display size and title
        //

        // Size of the navigator (can be different from the image size)
        cv::Size ImageDisplaySize = cv::Size();
        // Title displayed in the border
        std::string Legend = "Image Navigator";

        //
        // Zoom and Pan
        //

        // ZoomMatrix can be created using MakeZoomMatrix to create a view centered around a given point
        cv::Matx33d ZoomMatrix = cv::Matx33d::eye();
        // If displaying several navigators, those with the same ZoomKey will zoom and pan together
        std::string ZoomKey = "";

        //
        // Color adjustments for float matrixes
        //

        ColorAdjustmentsValues ColorAdjustments = {};
        // If displaying several navigators, those with the same ColorAdjustmentsKey will adjust together
        std::string ColorAdjustmentsKey = "";

        //
        // Zoom and pan with the mouse
        //
        bool PanWithMouse = true;
        bool ZoomWithMouseWheel = true;

        //
        // Color Order: RGB or RGBA versus BGR or BGRA (Note: by default OpenCV uses BGR and BGRA)
        //
        bool IsColorOrderBGR = true;

        //
        // Image display options
        //
        // if SelectedChannel >= 0 then only this channel is displayed
        int  SelectedChannel = -1;
        bool ShowAlphaChannelCheckerboard = true;
        // Image display options when the zoom is high
        bool ShowGrid = true;
        bool DrawValuesOnZoomedPixels = true;

        //
        // Navigator display options
        //
        bool ShowImageInfo = true;
        bool ShowPixelInfo = true;
        bool ShowZoomButtons = true;
        bool ShowLegendBorder = true;
        bool ShowOptions = false;
        bool ShowOptionsInTooltip = false;

        //
        // Watched Pixels
        //
        std::vector<cv::Point> WatchedPixels;
        bool HighlightWatchedPixels = true;
    };


    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams* params,
        bool refreshImage = false
        );


    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        const cv::Size& imageDisplaySize = cv::Size(),
        const std::string& legend = "Image Navigator",
        bool refreshImage = false,
        bool showOptionsWhenAppearing = false,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = ""
    );


    cv::Matx33d MakeZoomMatrix(
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        const cv::Size displayedImageSize
    );


    void ClearNavigatorTextureCache();


    ////////////////////////////////////////////
    // Inspector
    ////////////////////////////////////////////
    void Inspector_AddImage(
        const cv::Mat& image,
        const std::string& legend,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = "",
        const cv::Point2d & zoomCenter = cv::Point2d(),
        double zoomRatio = -1.,
        bool isColorOrderBGR = true
    );
    void Inspector_Show();
    void Inspector_ClearImages();

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.cpp continued                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/internal_icons.h included by src/immvision/image.cpp            //
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
        unsigned int GetIcon(IconType iconType);

        bool IconButton(IconType iconType, bool disabled = false);

        void ClearIconsTextureCache();

        void DevelPlaygroundGui();

    } // namespace Icons
} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image.cpp continued                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/short_lived_cache.h included by src/immvision/image.cpp         //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <map>


namespace ImmVision
{
    namespace internal
    {
        double TimerSeconds();

        template<typename Key, typename Value>
        class Cache
        {
        public:
            bool Contains(const Key& k)
            {
                return mDict.find(k) != mDict.end();
            }

            Value& Get(const Key& k)
            {
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
//                       src/immvision/image.cpp continued                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <chrono>

namespace ImmVision
{
    namespace internal
    {
        double cacheTimeToLive = 5.;
        ShortLivedCache<const cv::Mat*, std::unique_ptr<GlTextureCv>> gTextureCache(cacheTimeToLive);


        struct
        {
            cv::Point2d lastDisplayRatio = cv::Point2d(1., 1.);
            ImVec2      lastImageCursorPos = ImVec2(0.f, 0.f);
        } gLastImageInfo;


    } // namespace internal


    void Image(
        const cv::Mat &mat,
        bool refresh,
        const cv::Size& size,
        bool isBgrOrBgra
    )
    {
        auto & textureCache = internal::gTextureCache;
        if ( ! textureCache.Contains(&mat))
        {
            textureCache.AddKey(&mat);
            textureCache.Get(&mat) = std::make_unique<GlTextureCv>();
        }

        auto& glTextureCv = textureCache.Get(&mat);

        // Detect if the texture was not transfered to OpenGL yet
        if ((glTextureCv->mImageSize.x == 0.f) && (glTextureCv->mImageSize.y == 0.f))
            refresh = true;

        if (refresh)
            glTextureCv->BlitMat(mat, isBgrOrBgra);

        ImVec2 sizeImVec2((float)size.width, (float)size.height);
        ImVec2 displaySize = ImGuiImm::ComputeDisplayImageSize(sizeImVec2, glTextureCv->mImageSize);

        internal::gLastImageInfo.lastDisplayRatio = cv::Point2d(
            (double)(glTextureCv->mImageSize.x / displaySize.x),
            (double)(glTextureCv->mImageSize.y / displaySize.y)
        );
        internal::gLastImageInfo.lastImageCursorPos = ImGui::GetCursorScreenPos();

        glTextureCv->Draw_DisableDragWindow(displaySize);
    }

    ImVec2 GetImageMousePos_OnScreen()
    {
        ImVec2 mouse = ImGui::GetMousePos();
        ImVec2 imagePos = internal::gLastImageInfo.lastImageCursorPos;

        return ImVec2(mouse.x - imagePos.x, mouse.y - imagePos.y);
    }

    cv::Point2d GetImageMousePos()
    {
        ImVec2 mousePos_OnScreen = GetImageMousePos_OnScreen();
        cv::Point2d mousePositionOriginal =
            {
                (double)mousePos_OnScreen.x * internal::gLastImageInfo.lastDisplayRatio.x,
                (double)mousePos_OnScreen.y * internal::gLastImageInfo.lastDisplayRatio.y
            };
        return mousePositionOriginal;
    }

    void ClearImageTextureCache()
    {
        internal::gTextureCache.Clear();
    }

    void ClearAllTextureCaches()
    {
        ClearImageTextureCache();
        Icons::ClearIconsTextureCache();
        ClearNavigatorTextureCache();
    }
    
} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.cpp                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv_drawing_utils.h included by src/immvision/image_navigator.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <opencv2/core/core.hpp>

namespace ImmVision
{

    namespace CvDrawingUtils
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

        cv::Scalar ColorsToScalar(Colors value);

        inline cv::Scalar Black()
        { return {0, 0, 0, 255}; }

        inline cv::Scalar Red()
        { return {0, 0, 255, 255}; }

        inline cv::Scalar Green()
        { return {0, 255, 0, 255}; }

        inline cv::Scalar Blue()
        { return {255, 0, 0, 255}; }

        inline cv::Scalar White()
        { return {255, 255, 255, 255}; }

        inline cv::Scalar Yellow()
        { return {0, 255, 255, 255}; }

        inline cv::Scalar Cyan()
        { return {255, 255, 0, 255}; }

        inline cv::Scalar Violet()
        { return {200, 50, 200, 255}; }

        inline cv::Scalar Orange()
        { return {255, 128, 0, 255}; }


        void line(cv::Mat &image,
                  const cv::Point2d &a,
                  const cv::Point2d &b,
                  cv::Scalar color,
                  int thickness = 1);

        void ellipse(cv::Mat &image,
                     const cv::Point2d &center,
                     const cv::Size2d &size,
                     const cv::Scalar &color,
                     double angle = 0.,
                     double start_angle = 0.,
                     double end_angle = 360.,
                     int thickness = 1);

        void circle(cv::Mat &image,
                    const cv::Point2d &center,
                    double radius,
                    cv::Scalar color,
                    int thickness = 1);

        void rectangle(cv::Mat &image,
                       const cv::Point2d &pt1,
                       const cv::Point2d &pt2,
                       const cv::Scalar &color,
                       bool fill = false,
                       int thickness = 1);


        void rectangle_size(cv::Mat &img,
                            const cv::Point2d &pt,
                            const cv::Size2d &size,
                            const cv::Scalar &color,
                            bool fill = false,
                            int thickness = 1);

        void text(cv::Mat &img,
                  const cv::Point2d &position,
                  const std::string &msg,
                  const cv::Scalar &color,
                  bool center_around_point = false,
                  bool add_cartouche = false,
                  double fontScale = 0.4,
                  int thickness = 1);

        void cross_hole(cv::Mat &img,
                        const cv::Point2d &position,
                        const cv::Scalar &color,
                        double size = 2.,
                        double size_hole = 2.,
                        int thickness = 1);

        void draw_named_feature(cv::Mat &img,
                                const cv::Point2d &position,
                                const std::string &name,
                                const cv::Scalar &color,
                                bool add_cartouche = false,
                                double size = 3.,
                                double size_hole = 2.,
                                int thickness = 1);

        void draw_transparent_pixel(
            cv::Mat &img_rgba,
            const cv::Point2d &position,
            const cv::Scalar &color,
            double alpha
        );

        void draw_grid(
            cv::Mat& img_rgba,
            cv::Scalar lineColor,
            double alpha,
            double x_spacing, double y_spacing,
            double x_start, double y_start,
            double x_end, double y_end
        );

        cv::Mat stack_images_vertically(const cv::Mat &img1, const cv::Mat &img2);
        cv::Mat stack_images_horizontally(const cv::Mat &img1, const cv::Mat &img2);

        cv::Mat make_alpha_channel_checkerboard_image(const cv::Size& size, int squareSize = 30);

        using Image_RGB = cv::Mat;
        using Image_RGBA = cv::Mat;

        Image_RGB overlay_alpha_image_precise(const cv::Mat &background_rgb_or_rgba,
                                              const Image_RGBA &overlay_rgba,
                                              double alpha);
        Image_RGBA converted_to_rgba_image(const cv::Mat &inputMat, bool isBgrOrBgra);

    }  // namespace CvDrawingUtils
}  // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.cpp continued                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/portable_file_dialogs.h included by src/immvision/image_navigator.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Portable File Dialogs :
//  Thanks to Sam Hocevar <sam@hocevar.net>
//  https://github.com/samhocevar/portable-file-dialogs
//
//
//  Copyright © 2018—2020 Sam Hocevar <sam@hocevar.net>
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
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h> // IFileDialog
#include <shellapi.h>
#include <strsafe.h>
#include <future>     // std::async

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
#include <unistd.h>   // read(), pipe(), dup2()
#include <csignal>    // ::kill, std::signal
#include <sys/wait.h> // waitpid()
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
        multiselect     = 0x1,
        // For file save, force overwrite and disable the confirmation dialog.
        force_overwrite = 0x2,
        // For folder select, force path to be the provided argument instead
        // of the last opened directory, which is the Microsoft-recommended,
        // user-friendly behaviour.
        force_path      = 0x4,
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

        bool check_program(std::string const &program);

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
        bool const &flags(flag in_flag) const;

        // Non-const getter for the static array of flags
        bool &flags(flag in_flag);
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
            std::string result(int *exit_code = nullptr);

            // High level function to abort
            bool kill();

#if _WIN32
            void start_func(std::function<std::string(int *)> const &fun);
    static BOOL CALLBACK enum_windows_callback(HWND hwnd, LPARAM lParam);
#elif __EMSCRIPTEN__
            void start(int exit_code);
#else
            void start_process(std::vector<std::string> const &command);
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
        dll(std::string const &name);
        ~dll();

        template<typename T> class proc
        {
        public:
            proc(dll const &lib, std::string const &sym)
              : m_proc(reinterpret_cast<T *>(::GetProcAddress(lib.handle, sym.c_str())))
            {}

            operator bool() const { return m_proc != nullptr; }
            operator T *() const { return m_proc; }

        private:
            T *m_proc;
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

        protected:
            explicit dialog();

            std::vector<std::string> desktop_helper() const;
            static std::string buttons_to_name(choice _choice);
            static std::string get_icon_name(icon _icon);

            std::string powershell_quote(std::string const &str) const;
            std::string osascript_quote(std::string const &str) const;
            std::string shell_quote(std::string const &str) const;

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
                        std::string const &title,
                        std::string const &default_path = "",
                        std::vector<std::string> const &filters = {},
                        opt options = opt::none);

        protected:
            std::string string_result();
            std::vector<std::string> vector_result();

#if _WIN32
            static int CALLBACK bffcallback(HWND hwnd, UINT uMsg, LPARAM, LPARAM pData);
#if PFD_HAS_IFILEDIALOG
    std::string select_folder_vista(IFileDialog *ifd, bool force_path);
#endif

    std::wstring m_wtitle;
    std::wstring m_wdefault_path;

    std::vector<std::string> m_vector_result;
#endif
        };

    } // namespace internal

//
// The notify widget
//

    class notify : public internal::dialog
    {
    public:
        notify(std::string const &title,
               std::string const &message,
               icon _icon = icon::info);
    };

//
// The message widget
//

    class message : public internal::dialog
    {
    public:
        message(std::string const &title,
                std::string const &text,
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
        open_file(std::string const &title,
                  std::string const &default_path = "",
                  std::vector<std::string> const &filters = { "All Files", "*" },
                  opt options = opt::none);

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
        // Backwards compatibility
    [[deprecated("Use pfd::opt::multiselect instead of allow_multiselect")]]
#endif
#endif
        open_file(std::string const &title,
                  std::string const &default_path,
                  std::vector<std::string> const &filters,
                  bool allow_multiselect);

        std::vector<std::string> result();
    };

    class save_file : public internal::file_dialog
    {
    public:
        save_file(std::string const &title,
                  std::string const &default_path = "",
                  std::vector<std::string> const &filters = { "All Files", "*" },
                  opt options = opt::none);

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
        // Backwards compatibility
    [[deprecated("Use pfd::opt::force_overwrite instead of confirm_overwrite")]]
#endif
#endif
        save_file(std::string const &title,
                  std::string const &default_path,
                  std::vector<std::string> const &filters,
                  bool confirm_overwrite);

        std::string result();
    };

    class select_folder : public internal::file_dialog
    {
    public:
        select_folder(std::string const &title,
                      std::string const &default_path = "",
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
        static inline std::wstring str2wstr(std::string const &str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring ret(len, '\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), (LPWSTR)ret.data(), (int)ret.size());
    return ret;
}

static inline std::string wstr2str(std::wstring const &str)
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

        static inline bool ends_with(std::string const &str, std::string const &suffix)
        {
            return suffix.size() <= str.size() &&
                   str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
        }

        static inline bool starts_with(std::string const &str, std::string const &prefix)
        {
            return prefix.size() <= str.size() &&
                   str.compare(0, prefix.size(), prefix) == 0;
        }

    } // namespace internal

// settings implementation

    inline settings::settings(bool resync)
    {
        flags(flag::is_scanned) &= !resync;

        if (flags(flag::is_scanned))
            return;

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
        auto desktop_name = std::getenv("XDG_SESSION_DESKTOP");
        if (desktop_name && desktop_name == std::string("gnome"))
            flags(flag::has_kdialog) = false;
        else if (desktop_name && desktop_name == std::string("KDE"))
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
    inline bool settings::check_program(std::string const &program)
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
        async.start_process({"/bin/sh", "-c", "which " + program});
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

    inline bool const &settings::flags(flag in_flag) const
    {
        static bool flags[size_t(flag::max_flag)];
        return flags[size_t(in_flag)];
    }

    inline bool &settings::flags(flag in_flag)
    {
        return const_cast<bool &>(static_cast<settings const *>(this)->flags(in_flag));
    }

// executor implementation

    inline std::string internal::executor::result(int *exit_code /* = nullptr */)
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
                SendMessage(hwnd, WM_CLOSE, 0, 0);
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
    auto that = (executor *)lParam;

    DWORD pid;
    auto tid = GetWindowThreadProcessId(hwnd, &pid);
    if (tid == that->m_tid)
        that->m_windows.insert(hwnd);
    return TRUE;
}
#endif

#if _WIN32
    inline void internal::executor::start_func(std::function<std::string(int *)> const &fun)
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
    inline void internal::executor::start_process(std::vector<std::string> const &command)
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

            std::vector<char *> args;
            std::transform(command.cbegin(), command.cend(), std::back_inserter(args),
                           [](std::string const &s) { return const_cast<char *>(s.c_str()); });
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
            m_stdout += std::string(buf, (size_t)received);
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
    inline internal::platform::dll::dll(std::string const &name)
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
    auto coinit = proc<HRESULT WINAPI (LPVOID, DWORD)>(*this, "CoInitializeEx");
    m_state = coinit(nullptr, COINIT_MULTITHREADED);
}

inline internal::platform::ole32_dll::~ole32_dll()
{
    if (is_initialized())
        proc<void WINAPI ()>(*this, "CoUninitialize")();
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
        "shell32.dll", 0, 0, sys_dir.c_str(), (LPCSTR)124,
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
            case choice::ok: default: return "ok";
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
                case icon::info:  default:
#if _WIN32
                return "info";
#else
                return "information";
#endif
        }
    }

// THis is only used for debugging purposes
    inline std::ostream& operator <<(std::ostream &s, std::vector<std::string> const &v)
    {
        int not_first = 0;
        for (auto &e : v)
            s << (not_first++ ? " " : "") << e;
        return s;
    }

// Properly quote a string for Powershell: replace ' or " with '' or ""
// FIXME: we should probably get rid of newlines!
// FIXME: the \" sequence seems unsafe, too!
// XXX: this is no longer used but I would like to keep it around just in case
    inline std::string internal::dialog::powershell_quote(std::string const &str) const
    {
        return "'" + std::regex_replace(str, std::regex("['\"]"), "$&$&") + "'";
    }

// Properly quote a string for osascript: replace \ or " with \\ or \"
// XXX: this also used to replace ' with \' when popen was used, but it would be
// smarter to do shell_quote(osascript_quote(...)) if this is needed again.
    inline std::string internal::dialog::osascript_quote(std::string const &str) const
    {
        return "\"" + std::regex_replace(str, std::regex("[\\\\\"]"), "\\$&") + "\"";
    }

// Properly quote a string for the shell: just replace ' with '\''
// XXX: this is no longer used but I would like to keep it around just in case
    inline std::string internal::dialog::shell_quote(std::string const &str) const
    {
        return "'" + std::regex_replace(str, std::regex("'"), "'\\''") + "'";
    }

// file_dialog implementation

    inline internal::file_dialog::file_dialog(type in_type,
                                              std::string const &title,
                                              std::string const &default_path /* = "" */,
                                              std::vector<std::string> const &filters /* = {} */,
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
                         options](int *exit_code) -> std::string
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
                IFileDialog *ifd;
                HRESULT hr = dll::proc<HRESULT WINAPI (REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID *)>(ole32, "CoCreateInstance")
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

            auto *list = SHBrowseForFolderW(&bi);
            std::string ret;
            if (list)
            {
                auto buffer = new wchar_t[MAX_PATH];
                SHGetPathFromIDListW(list, buffer);
                dll::proc<void WINAPI (LPVOID)>(ole32, "CoTaskMemFree")(list);
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
                StringCchCopyW(ofn.lpstrFile, MAX_PATH*256+1, m_wdefault_path.c_str());
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

            dll::proc<BOOL WINAPI (LPOPENFILENAMEW)> get_save_file_name(comdlg32, "GetSaveFileNameW");
            if (get_save_file_name(&ofn) == 0)
                return "";
            return internal::wstr2str(woutput.c_str());
        }
        else
        {
            if (options & opt::multiselect)
                ofn.Flags |= OFN_ALLOWMULTISELECT;
            ofn.Flags |= OFN_PATHMUSTEXIST;

            dll::proc<BOOL WINAPI (LPOPENFILENAMEW)> get_open_file_name(comdlg32, "GetOpenFileNameW");
            if (get_open_file_name(&ofn) == 0)
                return "";
        }

        std::string prefix;
        for (wchar_t const *p = woutput.c_str(); *p; )
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
                script += " default location " + osascript_quote(default_path);
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
                for ( ; iter != end; ++iter)
                {
                    auto pat = iter->str();
                    if (pat == "*" || pat == "*.*")
                        has_filter = false;
                    else if (internal::starts_with(pat, "*."))
                        filter_list += (filter_list.size() == 0 ? "" : ",") +
                                       osascript_quote(pat.substr(2, pat.size() - 2));
                }
                if (has_filter && filter_list.size() > 0)
                    script += " of type {" + filter_list + "}";
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
            command.push_back("--filename=" + default_path);
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
    auto inst = (file_dialog *)pData;
    switch (uMsg)
    {
        case BFFM_INITIALIZED:
            SendMessage(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)inst->m_wdefault_path.c_str());
            break;
    }
    return 0;
}

#if PFD_HAS_IFILEDIALOG
inline std::string internal::file_dialog::select_folder_vista(IFileDialog *ifd, bool force_path)
{
    std::string result;

    IShellItem *folder;

    // Load library at runtime so app doesn't link it at load time (which will fail on windows XP)
    dll shell32("shell32.dll");
    dll::proc<HRESULT WINAPI (PCWSTR, IBindCtx*, REFIID, void**)>
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
    ifd->SetOptions(FOS_PICKFOLDERS);
    ifd->SetTitle(m_wtitle.c_str());

    hr = ifd->Show(GetActiveWindow());
    if (SUCCEEDED(hr))
    {
        IShellItem* item;
        hr = ifd->GetResult(&item);
        if (SUCCEEDED(hr))
        {
            wchar_t* wselected = nullptr;
            item->GetDisplayName(SIGDN_FILESYSPATH, &wselected);
            item->Release();

            if (wselected)
            {
                result = internal::wstr2str(std::wstring(wselected));
                dll::proc<void WINAPI (LPVOID)>(ole32_dll(), "CoTaskMemFree")(wselected);
            }
        }
    }

    ifd->Release();

    return result;
}
#endif
#endif

// notify implementation

    inline notify::notify(std::string const &title,
                          std::string const &message,
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
        ((NOTIFYICONDATAW *)lParam)->hIcon = ::LoadIcon(GetModuleHandle(nullptr), lpName);
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

    inline message::message(std::string const &title,
                            std::string const &text,
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
            switch (_choice)
            {
                case choice::ok_cancel:
                    script += "buttons {\"OK\", \"Cancel\"}"
                              " default button \"OK\""
                              " cancel button \"Cancel\"";
                    m_mappings[256] = button::cancel;
                    break;
                case choice::yes_no:
                    script += "buttons {\"Yes\", \"No\"}"
                              " default button \"Yes\""
                              " cancel button \"No\"";
                    m_mappings[256] = button::no;
                    break;
                case choice::yes_no_cancel:
                    script += "buttons {\"Yes\", \"No\", \"Cancel\"}"
                              " default button \"Yes\""
                              " cancel button \"Cancel\"";
                    m_mappings[256] = button::cancel;
                    break;
                case choice::retry_cancel:
                    script += "buttons {\"Retry\", \"Cancel\"}"
                              " default button \"Retry\""
                              " cancel button \"Cancel\"";
                    m_mappings[256] = button::cancel;
                    break;
                case choice::abort_retry_ignore:
                    script += "buttons {\"Abort\", \"Retry\", \"Ignore\"}"
                              " default button \"Retry\""
                              " cancel button \"Retry\"";
                    m_mappings[256] = button::cancel;
                    break;
                case choice::ok: default:
                    script += "buttons {\"OK\"}"
                              " default button \"OK\""
                              " cancel button \"OK\"";
                    m_mappings[256] = button::ok;
                    break;
            }
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
        if (exit_code < 0 || // this means cancel
            internal::ends_with(ret, "Cancel\n"))
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

    inline open_file::open_file(std::string const &title,
                                std::string const &default_path /* = "" */,
                                std::vector<std::string> const &filters /* = { "All Files", "*" } */,
                                opt options /* = opt::none */)
        : file_dialog(type::open, title, default_path, filters, options)
    {
    }

    inline open_file::open_file(std::string const &title,
                                std::string const &default_path,
                                std::vector<std::string> const &filters,
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

    inline save_file::save_file(std::string const &title,
                                std::string const &default_path /* = "" */,
                                std::vector<std::string> const &filters /* = { "All Files", "*" } */,
                                opt options /* = opt::none */)
        : file_dialog(type::save, title, default_path, filters, options)
    {
    }

    inline save_file::save_file(std::string const &title,
                                std::string const &default_path,
                                std::vector<std::string> const &filters,
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

    inline select_folder::select_folder(std::string const &title,
                                        std::string const &default_path /* = "" */,
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
//                       src/immvision/image_navigator.cpp continued                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/imgui_imm_gl_image.h included by src/immvision/image_navigator.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Wrappers for ImGui::Image, ImGui::ImageButton and ImGui::GetWindowDrawList()->AddImage
//
// They have the same behavior under C++, but under python this is transferred to the python interpreter
// (see gl_provider_python.cpp for their python definition)
//
namespace ImGuiImmGlImage
{
    IMGUI_API void  Image(unsigned int user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    IMGUI_API bool  ImageButton(unsigned int user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
    IMGUI_API void  GetWindowDrawList_AddImage(unsigned int user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);
} // namespace ImGuiImmGlImage

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.cpp continued                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/zoom_pan_transform.h included by src/immvision/image_navigator.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = cv::Matx33d;

        MatrixType Identity();

        MatrixType ComputeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio);
        MatrixType ComputePanMatrix(const cv::Point2d& dragDelta, double currentZoom);
        MatrixType MakeScaleOne(cv::Size imageSize, cv::Size viewportSize);
        MatrixType MakeFullView(cv::Size imageSize, cv::Size viewportSize);
        cv::Matx33d MakeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio,const cv::Size displayedImageSize);

        bool IsEqual(const MatrixType & v1, const MatrixType & v2);

        cv::Point2d Apply(const MatrixType& zoomMatrix, const cv::Point2d &p);

        cv::Matx23d ZoomMatrixToM23(const cv::Matx33d &m);

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const cv::Size& oldDisplaySize, const cv::Size& newDisplaySize);

    } // namespace ZoomPanTransform

    cv::Matx33d MakeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio,const cv::Size displayedImageSize);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.cpp continued                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/color_adjustment_utils.h included by src/immvision/image_navigator.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace ColorAdjustmentsUtils
    {
        bool IsNone(const ColorAdjustmentsValues& a);
        bool IsEqual(const ColorAdjustmentsValues& v1, const ColorAdjustmentsValues& v2);
        cv::Mat Adjust(const ColorAdjustmentsValues& a, const cv::Mat &image);
        ColorAdjustmentsValues ComputeInitialImageAdjustments(const cv::Mat& m);
    } // namespace ImageAdjustmentsUtils

} // namespace ImmVision
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.cpp continued                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/matrix_info_utils.h included by src/immvision/image_navigator.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const cv::Mat& m);
        std::string _MatInfo(const cv::Mat &m);
        std::vector<double> MatValuesAt(const cv::Mat& m, int x, int y);
        std::string MatPixelColorInfo(const cv::Mat & m, int x, int y, char separator = ',', bool add_paren = true);

    } // namespace MatrixInfoUtils

} // namespace ImmVision
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/image_navigator.cpp continued                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "imgui_internal.h"

#include <iostream>

namespace ImmVision
{



    namespace ImageNavigatorDrawing
    {
        cv::Mat DrawWatchedPixels(const cv::Mat& image, const ImageNavigatorParams& params)
        {
            cv::Mat r = image.clone();

            std::vector<std::pair<size_t, cv::Point2d>> visiblePixels;
            {
                for (size_t i = 0; i < params.WatchedPixels.size(); ++i)
                {
                    cv::Point w = params.WatchedPixels[i];
                    cv::Point2d p = ZoomPanTransform::Apply(params.ZoomMatrix, w);
                    if (cv::Rect(cv::Point(0, 0), params.ImageDisplaySize).contains(p))
                        visiblePixels.push_back({i, p});
                }
            }

            for (const auto& kv : visiblePixels)
            {
                CvDrawingUtils::draw_named_feature(
                    r,         // img
                    kv.second, // position,
                    std::to_string(kv.first),       // name
                    cv::Scalar(255, 255, 255, 255), // color
                    true, // add_cartouche
                    4.,   // size
                    2.5,  // size_hole
                    1     // thickness
                );
            }

            return r;
        }

        void DrawGrid(cv::Mat& inOutImageRgba, const ImageNavigatorParams& params)
        {
            double x_spacing = (double) params.ZoomMatrix(0, 0);
            double y_spacing = (double) params.ZoomMatrix(1, 1);

            double x_start, y_start;
            {
                cv::Point2d origin_unzoomed = ZoomPanTransform::Apply(params.ZoomMatrix.inv(), cv::Point2d(0., 0.));
                origin_unzoomed = cv::Point2d(std::floor(origin_unzoomed.x) + 0.5, std::floor(origin_unzoomed.y) + 0.5);
                cv::Point2d origin_zoomed = ZoomPanTransform::Apply(params.ZoomMatrix, origin_unzoomed);
                x_start = origin_zoomed.x;
                y_start = origin_zoomed.y;
            }
            double x_end = (double)inOutImageRgba.cols - 1.;
            double y_end = (double)inOutImageRgba.rows - 1.;

            auto lineColor = cv::Scalar(255, 255, 0, 255);
            double alpha = 0.3;
            CvDrawingUtils::draw_grid(inOutImageRgba, lineColor, alpha, x_spacing, y_spacing, x_start, y_start, x_end, y_end);
        }

        cv::Mat DrawValuesOnZoomedPixels(const cv::Mat& drawingImage, const cv::Mat& valuesImage,
                                         const ImageNavigatorParams& params, bool drawPixelCoords)
        {
            assert(drawingImage.type() == CV_8UC4);

            cv::Mat r = drawingImage;
            cv::Point tl, br;
            {
                cv::Point2d tld = ZoomPanTransform::Apply(params.ZoomMatrix.inv(), cv::Point2d(0., 0.));
                cv::Point2d brd = ZoomPanTransform::Apply(params.ZoomMatrix.inv(),
                                                          cv::Point2d((double)params.ImageDisplaySize.width,
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

                    cv::Point2d position = ZoomPanTransform::Apply(params.ZoomMatrix, cv::Point2d((double)x, (double )y));

                    cv::Scalar textColor;
                    {
                        cv::Scalar white(255, 255, 255, 255);
                        cv::Scalar black(0, 0, 0, 255);
                        cv::Vec4b backgroundColor(0, 0, 0, 0);
                        if ( cv::Rect(cv::Point(), drawingImage.size()).contains({(int)position.x, (int)position.y}))
                            backgroundColor = drawingImage.at<cv::Vec4b>((int)position.y, (int)position.x);
                        double luminance = backgroundColor[2] * 0.2126 + backgroundColor[1] * 0.7152 + backgroundColor[0] * 0.0722;
                        if (luminance > 170.)
                            textColor = black;
                        else
                            textColor = white;
                    }
                    CvDrawingUtils::text(
                        r,
                        position,
                        pixelInfo,
                        textColor,
                        true, // center_around_point
                        false, // add_cartouche
                        0.3,  //fontScale
                        1     //int thickness
                    );
                }
            }
            return r;
        };


        cv::Mat MakeWarpPaperBackground(cv::Size s)
        {
            cv::Mat mat(s, CV_8UC4);

            auto paperColor = cv::Scalar(205, 215, 220, 255);
            auto lineColor = cv::Scalar(199, 196, 184, 255);
            mat = paperColor;
            int quadSize = 17;
            for (int y = 0; y < s.height; y+= quadSize)
            {
                auto linePtr = mat.ptr<cv::Vec4b>(y);
                for (int x = 0; x < s.width; ++x)
                {
                    *linePtr = lineColor;
                    linePtr++;
                }
            }
            for (int y = 0; y < s.height; y++)
            {
                auto linePtr = mat.ptr<cv::Vec4b>(y);
                for (int x = 0; x < s.width; x+=quadSize)
                {
                    *linePtr = lineColor;
                    linePtr += quadSize;
                }
            }
            return mat;
        }

        void BlitImageNavigatorTexture(
            const ImageNavigatorParams& params,
            const cv::Mat& image,
            cv::Mat& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTextureCv* outTexture
        )
        {
            if (image.empty())
                return;

            cv::Mat finalImage = image.clone();

            //
            // Adjustements needed before conversion to rgba
            //
            auto fnAdjustColor = [&finalImage, params]()
            {
                // Selected channels
                if (finalImage.channels() > 1 && (params.SelectedChannel >= 0) && (params.SelectedChannel < finalImage.channels()))
                {
                    std::vector<cv::Mat> channels;
                    cv::split(finalImage, channels);
                    finalImage = channels[(size_t)params.SelectedChannel];
                }

                // Alpha checkerboard
                if (finalImage.type() == CV_8UC4 && params.ShowAlphaChannelCheckerboard)
                {
                    cv::Mat background = CvDrawingUtils::make_alpha_channel_checkerboard_image(finalImage.size());
                    finalImage = CvDrawingUtils::overlay_alpha_image_precise(background, finalImage, 1.);
                }

                // Color adjustments
                finalImage = ColorAdjustmentsUtils::Adjust(params.ColorAdjustments, finalImage);
            };

            //
            // Convert to rgba with adjustments if needed
            //
            if (shall_refresh_rgba)
            {
                fnAdjustColor();
                finalImage = CvDrawingUtils::converted_to_rgba_image(finalImage, params.IsColorOrderBGR);
                in_out_rgba_image_cache = finalImage;
                assert(finalImage.type() == CV_8UC4);
            }
            else
            {
                finalImage = in_out_rgba_image_cache;
                assert(finalImage.type() == CV_8UC4);
                assert(!finalImage.empty());
            }

            //
            // Zoom
            //
            {
                cv::Mat imageZoomed = MakeWarpPaperBackground(params.ImageDisplaySize);
                cv::warpAffine(finalImage, imageZoomed,
                               ZoomPanTransform::ZoomMatrixToM23(params.ZoomMatrix),
                               params.ImageDisplaySize,
                               cv::INTER_NEAREST,
                               cv::BorderTypes::BORDER_TRANSPARENT,
                               cv::Scalar(127, 127, 127, 127)
                );
                finalImage = imageZoomed;
            }

            //
            // Drawings on final image
            //
            {
                // Draw grid
                double gridMinZoomFactor = 12.;
                double zoomFactor = (double)params.ZoomMatrix(0, 0);
                if (params.ShowGrid && zoomFactor >= gridMinZoomFactor)
                    DrawGrid(finalImage, params);

                // Draw Pixel Values
                double drawPixelvaluesMinZoomFactor = (image.depth() == CV_8U) ? 36. : 48.;
                if (params.DrawValuesOnZoomedPixels && zoomFactor > drawPixelvaluesMinZoomFactor)
                {
                    double drawPixelCoordsMinZoomFactor = 60.;
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
            outTexture->BlitMat(finalImage, false);
        }

    } // namespace ImageNavigatorDrawing


    namespace ImageNavigatorWidgets
    {
        cv::Point2d DisplayTexture_TrackMouse(const GlTextureCv& texture, ImVec2 displaySize)
        {
            ImVec2 imageTopLeft = ImGui::GetCursorScreenPos();
            texture.Draw_DisableDragWindow(displaySize);
            bool isImageHovered = ImGui::IsItemHovered();
            ImVec2 mouse = ImGui::GetMousePos();
            if (isImageHovered)
                return cv::Point2d((double)(mouse.x - imageTopLeft.x), (double)(mouse.y - imageTopLeft.y));
            else
                return cv::Point2d(-1., -1.);
        }

        void ShowImageInfo(const cv::Mat &image, double zoomFactor)
        {
            std::string info = MatrixInfoUtils::_MatInfo(image);
            ImGui::Text("%s - Zoom:%.3lf", info.c_str(), zoomFactor);
        }


        void ShowPixelColorWidget(
            const cv::Mat &image,
            cv::Point pt,
            const ImageNavigatorParams& params)
        {
            bool isInImage = cv::Rect(cv::Point(0, 0), image.size()).contains((pt));
            auto UCharToFloat = [](int v) { return (float)((float) v / 255.f); };
            auto Vec3bToImVec4 = [&UCharToFloat, &params](cv::Vec3b v) {
                return params.IsColorOrderBGR ?
                       ImVec4(UCharToFloat(v[2]), UCharToFloat(v[1]), UCharToFloat(v[0]), UCharToFloat(255))
                                              :   ImVec4(UCharToFloat(v[0]), UCharToFloat(v[1]), UCharToFloat(v[2]), UCharToFloat(255));
            };
            auto Vec4bToImVec4 = [&UCharToFloat, &params](cv::Vec4b v) {
                return params.IsColorOrderBGR ?
                       ImVec4(UCharToFloat(v[2]), UCharToFloat(v[1]), UCharToFloat(v[0]), UCharToFloat(v[3]))
                                              :    ImVec4(UCharToFloat(v[0]), UCharToFloat(v[1]), UCharToFloat(v[2]), UCharToFloat(v[3]));
            };

            bool done = false;
            std::string id = std::string("##pixelcolor_") + std::to_string(pt.x) + "," + std::to_string(pt.y);
            if (image.depth() == CV_8U)
            {
                ImGuiColorEditFlags editFlags =
                    ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_AlphaPreviewHalf
                    | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Uint8;
                if (!isInImage)
                {
                    // ColorEdit4 introduces a strange line spacing on the next group
                    // which cannot be simulated with ImGui::Dummy
                    // => we add a dummy one (hopefully black on a black background)
                    float dummyColor[4]{0.f, 0.f, 0.f, 255.f};
                    ImGui::SetNextItemWidth(1.f);
                    int colorEditFlags =
                              ImGuiColorEditFlags_NoInputs
                            | ImGuiColorEditFlags_InputRGB
                            | ImGuiColorEditFlags_DisplayRGB;
                    ImGui::ColorEdit4(id.c_str(), dummyColor, colorEditFlags );
                    done = true;
                }
                else if (image.channels() == 3)
                {
                    cv::Vec3b col = image.at<cv::Vec3b>(pt.y, pt.x);
                    ImVec4 colorAsImVec = Vec3bToImVec4(col);
                    ImGui::SetNextItemWidth(150.f);
                    ImGui::ColorEdit3(id.c_str(), (float*)&colorAsImVec, editFlags);
                    done = true;
                }
                else if (image.channels() == 4)
                {
                    cv::Vec4b col = image.at<cv::Vec4b>(pt.y, pt.x);
                    ImVec4 colorAsImVec = Vec4bToImVec4(col);
                    ImGui::SetNextItemWidth(200.f);
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
        static bool s_CollapsingHeader_CacheState_Sync = false;

        bool CollapsingHeader_OptionalCacheState(const char *name, bool forceOpen = false)
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

    } // namespace ImageNavigatorWidgets


    namespace ImageNavigatorUtils
    {
        void InitializeMissingParams(ImageNavigatorParams* params, const cv::Mat& image)
        {
            if (ColorAdjustmentsUtils::IsNone(params->ColorAdjustments))
                params->ColorAdjustments = ColorAdjustmentsUtils::ComputeInitialImageAdjustments(image);
            if (params->ZoomMatrix == cv::Matx33d::eye())
                params->ZoomMatrix = ZoomPanTransform::MakeFullView(image.size(), params->ImageDisplaySize);
        }

        bool ShallRefreshRgbaCache(const ImageNavigatorParams& v1, const ImageNavigatorParams& v2)
        {
            if (! ColorAdjustmentsUtils::IsEqual(v1.ColorAdjustments, v2.ColorAdjustments))
                return true;
            if (v1.SelectedChannel != v2.SelectedChannel)
                return true;
            if (v1.ShowAlphaChannelCheckerboard != v2.ShowAlphaChannelCheckerboard)
                return true;
            if (v1.IsColorOrderBGR != v2.IsColorOrderBGR)
                return true;
            return false;
        }

        bool ShallRefreshTexture(const ImageNavigatorParams& v1, const ImageNavigatorParams& v2)
        {
            if (v1.ImageDisplaySize != v2.ImageDisplaySize)
                return true;
            if (! ZoomPanTransform::IsEqual(v1.ZoomMatrix, v2.ZoomMatrix))
                return true;
            if (! ColorAdjustmentsUtils::IsEqual(v1.ColorAdjustments, v2.ColorAdjustments))
                return true;
            if (v1.ShowGrid != v2.ShowGrid)
                return true;
            if (v1.SelectedChannel != v2.SelectedChannel)
                return true;
            if (v1.ShowAlphaChannelCheckerboard != v2.ShowAlphaChannelCheckerboard)
                return true;
            if (v1.IsColorOrderBGR != v2.IsColorOrderBGR)
                return true;
            if (v1.WatchedPixels.size() != v2.WatchedPixels.size())
                return true;
            if (v1.HighlightWatchedPixels != v2.HighlightWatchedPixels)
                return true;
            if (v1.DrawValuesOnZoomedPixels != v2.DrawValuesOnZoomedPixels)
                return true;
            return false;
        }


        class ImageNavigatorTextureCache
        {
        public:
            // members
            struct CachedParams
            {
                // This caches are small and will persist during the application lifetime
                ImageNavigatorParams* NavigatorParams = nullptr;
                ImVec2 LastDragDelta;
                std::vector<char> FilenameEditBuffer = std::vector<char>(1000, '\0');
                bool   IsMouseDragging = false;
                struct ImageNavigatorParams  PreviousParams;
            };
            struct CachedImages
            {
                // This caches are heavy and will be destroyed
                // if not used (after about 5 seconds)
                cv::Mat     ImageRgbaCache;
                std::unique_ptr<GlTextureCv> GlTexture;
            };

            void UpdateCache(const cv::Mat& image, ImageNavigatorParams* params, bool refresh)
            {
                auto cacheKey = &image;
                params->ImageDisplaySize = ImGuiImm::ComputeDisplayImageSize(params->ImageDisplaySize, image.size());

                bool needsRefreshTexture = refresh;
                bool shallRefreshRgbaCache = false;

                if (! mCacheParams.Contains(cacheKey))
                {
                    InitializeMissingParams(params, image);
                    needsRefreshTexture = true;
                    shallRefreshRgbaCache = true;
                    mCacheParams.AddKey(cacheKey);
                }
                if (! mCacheImages.Contains(cacheKey))
                {
                    mCacheImages.AddKey(cacheKey);
                    needsRefreshTexture = true;
                    shallRefreshRgbaCache = true;
                    mCacheImages.Get(cacheKey).GlTexture = std::make_unique<GlTextureCv>();
                }

                auto& cachedParams = mCacheParams.Get(cacheKey);
                auto& cachedImages = mCacheImages.Get(cacheKey);
                cachedParams.NavigatorParams = params;

                ImageNavigatorParams oldParams = cachedParams.PreviousParams;
                *cachedParams.NavigatorParams = *params;

                if (cachedImages.GlTexture->mImageSize.x == 0.f)
                    needsRefreshTexture = true;
                if (ShallRefreshTexture(oldParams, *params))
                    needsRefreshTexture = true;
                if (!(oldParams.ImageDisplaySize.area() == 0) && (oldParams.ImageDisplaySize != params->ImageDisplaySize))
                    params->ZoomMatrix = ZoomPanTransform::UpdateZoomMatrix_DisplaySizeChanged(
                        oldParams.ZoomMatrix, oldParams.ImageDisplaySize, params->ImageDisplaySize);
                if (needsRefreshTexture)
                {
                    if (ShallRefreshRgbaCache(oldParams, *params))
                        shallRefreshRgbaCache = true;
                    ImageNavigatorDrawing::BlitImageNavigatorTexture(
                        *params, image, cachedImages.ImageRgbaCache, shallRefreshRgbaCache, cachedImages.GlTexture.get());
                }

                if (! ZoomPanTransform::IsEqual(oldParams.ZoomMatrix, params->ZoomMatrix))
                    UpdateLinkedZooms(image);
                if (! ColorAdjustmentsUtils::IsEqual(oldParams.ColorAdjustments, params->ColorAdjustments))
                    UpdateLinkedColorAdjustments(image);

                cachedParams.PreviousParams = *params;

                mCacheImages.ClearOldEntries();
            }

            CachedParams& GetCacheParams(const cv::Mat& image)
            {
                return mCacheParams.Get(&image);
            }
            CachedImages& GetCacheImages(const cv::Mat& image)
            {
                return mCacheImages.Get(&image);
            }

            void ClearImagesCache()
            {
                mCacheImages.Clear();
            }

        private:
            // Methods
            void UpdateLinkedZooms(const cv::Mat& image)
            {
                auto currentCacheKey = &image;
                auto & currentCache = mCacheParams.Get(&image);
                std::string zoomKey = currentCache.NavigatorParams->ZoomKey;
                if (zoomKey.empty())
                    return;
                ZoomPanTransform::MatrixType newZoom = currentCache.NavigatorParams->ZoomMatrix;
                for (auto& otherCacheKey : mCacheParams.Keys())
                {
                    CachedParams & otherCache = mCacheParams.Get(otherCacheKey);
                    if ((otherCacheKey != currentCacheKey) && (otherCache.NavigatorParams->ZoomKey == zoomKey))
                        otherCache.NavigatorParams->ZoomMatrix = newZoom;
                }
            }
            void UpdateLinkedColorAdjustments(const cv::Mat& image)
            {
                auto currentCacheKey = &image;
                auto & currentCache = mCacheParams.Get(&image);
                std::string colorKey = currentCache.NavigatorParams->ColorAdjustmentsKey;
                if (colorKey.empty())
                    return;
                ColorAdjustmentsValues newColorAdjustments = currentCache.NavigatorParams->ColorAdjustments;
                for (auto& otherCacheKey : mCacheParams.Keys())
                {
                    CachedParams & otherCache = mCacheParams.Get(otherCacheKey);
                    if ((otherCacheKey != currentCacheKey) && (otherCache.NavigatorParams->ColorAdjustmentsKey == colorKey))
                        otherCache.NavigatorParams->ColorAdjustments = newColorAdjustments;
                }
            }

            internal::Cache<const cv::Mat *, CachedParams> mCacheParams;
            double mCachedImagesTimeToLive = 5.;
            internal::ShortLivedCache<const cv::Mat *, CachedImages> mCacheImages { mCachedImagesTimeToLive };
        };
        static ImageNavigatorTextureCache gImageNavigatorTextureCache;
    } // namespace ImageNavigatorUtils

    void ClearNavigatorTextureCache()
    {
        ImageNavigatorUtils::gImageNavigatorTextureCache.ClearImagesCache();
    }

    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        ImageNavigatorParams* params,
        bool refresh)
    {
        using CachedParams = ImageNavigatorUtils::ImageNavigatorTextureCache::CachedParams;
        using CachedImages = ImageNavigatorUtils::ImageNavigatorTextureCache::CachedImages;

        //
        // Lambda / panel Title
        //
        auto fnPanelTitle = [&params, &image]()
        {
            std::string panelTitle;
            {
                if (params->ShowLegendBorder)
                    panelTitle = params->Legend;
                panelTitle += "##ImageNavigator_" + std::to_string((size_t)&image);
            }
            return panelTitle;
        };

        //
        // Lambdas / Watched Pixels
        //
        bool wasWatchedPixelAdded = false;
        auto fnWatchedPixels_Add = [&params, &wasWatchedPixelAdded](const cv::Point2d& pixelDouble)
        {
            cv::Point pixel((int)(pixelDouble.x + .5), (int)(pixelDouble.y + .5));
            params->WatchedPixels.push_back(pixel);

            wasWatchedPixelAdded = true;
            if (! params->ShowOptionsInTooltip)
                params->ShowOptions = true;
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
                    cv::Point watchedPixel = params->WatchedPixels[i];
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
                    ImageNavigatorWidgets::ShowPixelColorWidget(image, watchedPixel, *params);

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

            ImGui::Checkbox("Highlight Watched Pixels", &params->HighlightWatchedPixels);
        };

        //
        // Lambdas / Options & Adjustments
        //
        auto fnOptionsInnerGui = [&params, &image, &fnWatchedPixels_Gui, &wasWatchedPixelAdded](
                CachedParams & cacheParams)
        {
            float optionsWidth = 330.f;
            // Group with fixed width, so that Collapsing headers stop at optionsWidth
            ImGuiImm::BeginGroupFixedWidth(optionsWidth);

            // Adjust float values
            bool hasAdjustFloatValues = true; // ((image.depth() == CV_32F) || (image.depth() == CV_64F));

            if (hasAdjustFloatValues && ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Adjust"))
            {
                ImGui::PushItemWidth(200.);
                ImGuiImm::SliderDouble(
                    "k", &params->ColorAdjustments.Factor,
                    0., 32., "%.3f", ImGuiSliderFlags_Logarithmic);
                ImGui::PushItemWidth(200.);
                ImGuiImm::SliderDouble(
                    "Delta", &params->ColorAdjustments.Delta,
                    -255., 255., "%.3f", ImGuiSliderFlags_Logarithmic);

                if (ImGui::Button("Default"))
                    params->ColorAdjustments = ColorAdjustmentsUtils::ComputeInitialImageAdjustments(image);
                ImGui::SameLine();
                 if (ImGui::Button("No Adjustment"))
                        params->ColorAdjustments = ColorAdjustmentsValues();
            }

            // Watched Pixels
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Watched Pixels", wasWatchedPixelAdded))
                fnWatchedPixels_Gui();

            // Image display options
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Image Display"))
            {
                if (image.type() == CV_8UC3 || image.type() == CV_8UC4)
                {
                    ImGui::Text("Color Order");
                    ImGui::SameLine();
                    int v = params->IsColorOrderBGR ? 0 : 1;
                    ImGui::RadioButton("RGB", &v, 1);
                    ImGui::SameLine();
                    ImGui::RadioButton("BGR", &v, 0);
                    params->IsColorOrderBGR = (v == 0);
                }
                if (image.type() == CV_8UC4)
                    ImGui::Checkbox("Show alpha channel checkerboard", &params->ShowAlphaChannelCheckerboard);
                if (image.channels() > 1)
                {
                    ImGui::Text("Channels: ");
                    ImGui::RadioButton("All", &params->SelectedChannel, -1); ImGui::SameLine();
                    for (int channel_id = 0; channel_id < image.channels(); ++channel_id)
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

            }

            //Navigator display options
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Options"))
            {
                {
                    ImGuiImm::BeginGroupPanel("Navigator display options");
                    ImGui::Checkbox("Show image info", &params->ShowImageInfo);
                    ImGui::Checkbox("Show pixel info", &params->ShowPixelInfo);
                    ImGui::Checkbox("Show zoom buttons", &params->ShowZoomButtons);
                    ImGui::Checkbox("Show legend border", &params->ShowLegendBorder);
                    ImGuiImm::EndGroupPanel();
                }

                ImGui::Checkbox("Pan with mouse", &params->PanWithMouse);
                ImGui::Checkbox("Zoom with mouse wheel", &params->ZoomWithMouseWheel);

                ImGui::Separator();
                if (ImGui::Checkbox("Show Options in tooltip window", &params->ShowOptionsInTooltip))
                {
                    if (!params->ShowOptionsInTooltip) // We were in a tooltip when clicking
                        params->ShowOptions = true;
                }
            }

            // Save Image
            if (ImageNavigatorWidgets::CollapsingHeader_OptionalCacheState("Save"))
            {
                // Use portable_file_dialogs if available
                if (pfd::settings::available())
                {
                    if (ImGui::Button("Save Image"))
                    {
                        pfd::settings::verbose(true);
                        std::string filename = pfd::save_file("Select a file", ".",
                                                              { "Image Files", "*.png *.jpg *.jpeg *.jpg *.bmp *.gif *.exr",
                                                                "All Files", "*" }).result();
                        if (!filename.empty())
                        {
                            try
                            {
                                cv::imwrite(filename, image);
                            }
                            catch(const cv::Exception& e)
                            {
                                std::string errorMessage = std::string("Could not save image\n") + e.err.c_str();
                                pfd::message("Error", errorMessage, pfd::choice::ok, pfd::icon::error);
                            }
                        }
                    }
                }
                else
                {
                    ImGui::Text("File name");
                    char *filename = cacheParams.FilenameEditBuffer.data();
                    ImGui::SetNextItemWidth(200.f);
                    ImGui::InputText("##filename", filename, 1000);
                    //ImGui::SetNextItemWidth(200.f);
                    ImGui::Text("The image will be saved in the current folder");
                    ImGui::Text("with a format corresponding to the extension");
                    if (ImGui::SmallButton("save"))
                        cv::imwrite(filename, image);
                }
            }

            ImGuiImm::EndGroupFixedWidth();

        };
        auto fnToggleShowOptions = [&params]()
        {
            if (params->ShowOptionsInTooltip)
                ImGui::OpenPopup("Options");
            else
                params->ShowOptions = !params->ShowOptions;
        };
        auto fnOptionGui = [&params, &fnOptionsInnerGui](CachedParams & cacheParams)
        {
            if (params->ShowOptionsInTooltip)
            {
                if (ImGui::BeginPopup("Options"))
                {
                    fnOptionsInnerGui(cacheParams);
                    ImGui::EndPopup();
                }
            }
            else if (params->ShowOptions)
            {
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("Options");
                fnOptionsInnerGui(cacheParams);
                ImGui::EndGroup();
            }
        };

        //
        // Lambdas / Handle Zoom
        //
        // Mouse dragging
        auto fnHandleMouseDragging = [&params](CachedParams & cacheParams)
        {
            ZoomPanTransform::MatrixType& zoomMatrix = params->ZoomMatrix;

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
                    cv::Point2d((double)dragDeltaDelta.x, (double)dragDeltaDelta.y),
                    zoomMatrix(0, 0));
                cacheParams.LastDragDelta = dragDelta;
            }
        };
        auto fnHandleMouseWheel = [&params](const cv::Point2d& mouseLocation)
        {
            if (!params->ZoomWithMouseWheel)
                return;
            ImGui::SetItemUsingMouseWheel();

            if ((fabs(ImGui::GetIO().MouseWheel) > 0.f) && (ImGui::IsItemHovered()))
            {
                double zoomRatio = (double)ImGui::GetIO().MouseWheel / 4.;
                params->ZoomMatrix = params->ZoomMatrix * ZoomPanTransform::ComputeZoomMatrix(mouseLocation, exp(zoomRatio));
            }
        };
        auto fnShowZoomButtons = [&params, &image]()
        {
            if (params->ShowZoomButtons)
            {
                ZoomPanTransform::MatrixType& zoomMatrix = params->ZoomMatrix;

                cv::Point2d viewportCenter_originalImage = ZoomPanTransform::Apply(
                    zoomMatrix.inv(),
                    cv::Point2d (
                        (double)params->ImageDisplaySize.width / 2.,
                        (double)params->ImageDisplaySize.height / 2.)
                );

                {
                    cv::Point2d zoomCenter = params->WatchedPixels.empty() ?
                                viewportCenter_originalImage
                            :   cv::Point2d(params->WatchedPixels.back());
                    ImGui::PushButtonRepeat(true);
                    if (Icons::IconButton(Icons::IconType::ZoomPlus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1.1);

                    ImGui::SameLine();

                    if (Icons::IconButton(Icons::IconType::ZoomMinus))
                        zoomMatrix = zoomMatrix * ZoomPanTransform::ComputeZoomMatrix(zoomCenter, 1. / 1.1);

                    ImGui::PopButtonRepeat();
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
            }

        };

        //
        // Lambda / Show image
        //
        auto fnShowImage = [&params](CachedImages & cacheImages)
        {
            cv::Point2d mouseLocation = ImageNavigatorWidgets::DisplayTexture_TrackMouse(
                    *cacheImages.GlTexture,
                    ImVec2((float)params->ImageDisplaySize.width, (float)params->ImageDisplaySize.height));

            cv::Point2d mouseLocation_originalImage =
                ImGui::IsItemHovered() ? ZoomPanTransform::Apply(params->ZoomMatrix.inv(), mouseLocation) : cv::Point2d(-1., -1.);
            return mouseLocation_originalImage;
        };


        //
        // Lambda / Show pixel info
        //
        auto fnShowPixelInfo = [&image, &params](const cv::Point2d& mouseLocation)
        {
            cv::Point mouseLoc =
                mouseLocation.x >= 0. ?
                        cv::Point((int)(mouseLocation.x + 0.5), (int)(mouseLocation.y + 0.5))
                    :   cv::Point(-1, -1)
                    ;
            if (mouseLoc.x >= 0)
            {
                ImGui::Text("(%i,%i)", mouseLoc.x, mouseLoc.y);
                ImGui::SameLine();
            }
            ImageNavigatorWidgets::ShowPixelColorWidget(image, mouseLoc, *params);
        };

        //
        // Lambda / Show full Gui
        //
        auto fnShowFullGui = [&](CachedParams& cacheParams, CachedImages &cacheImages)
        {

            ImGui::BeginGroup();
            // Show image
            cv::Point2d mouseLocation_originalImage = fnShowImage(cacheImages);
            // Add Watched Pixel on double click
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
                fnWatchedPixels_Add(mouseLocation_originalImage);

            // Handle Mouse
            fnHandleMouseDragging(cacheParams);
            fnHandleMouseWheel(mouseLocation_originalImage);

            // Zoom+ / Zoom- buttons
            fnShowZoomButtons();
            // adjust button
            {
                if (!params->ShowZoomButtons)
                    ImGui::NewLine();
                ImGuiImm::SameLineAlignRight(20.f, (float)params->ImageDisplaySize.width);
                if (Icons::IconButton(Icons::IconType::AdjustLevels))
                    fnToggleShowOptions();
            }

            // Show infos
            if (params->ShowImageInfo)
                ImageNavigatorWidgets::ShowImageInfo(image, params->ZoomMatrix(0, 0));
            if (params->ShowPixelInfo)
                fnShowPixelInfo(mouseLocation_originalImage);
            ImGui::EndGroup();

            // Show Options
            fnOptionGui(cacheParams);

            return mouseLocation_originalImage;
        };
        auto fnShowFullGui_WithBorder = [&](CachedParams& cacheParams, CachedImages &cacheImages)
        {
            // BeginGroupPanel
            bool drawBorder = params->ShowLegendBorder;
            ImGuiImm::BeginGroupPanel_FlagBorder(fnPanelTitle().c_str(), drawBorder);
            cv::Point2d mouseLocation_originalImage = fnShowFullGui(cacheParams, cacheImages);
            ImGuiImm::EndGroupPanel_FlagBorder();
            return mouseLocation_originalImage;
        };


        /////////////////////////////////////////////////////////////////////////////////////////
        //
        // Core of the function below (there are only lambdas declarations before)
        //
        /////////////////////////////////////////////////////////////////////////////////////////
        if (image.empty())
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f),
                               "%s -> empty image !!!", params->Legend.c_str());
            return cv::Point2d();
        }

        ImageNavigatorUtils::gImageNavigatorTextureCache.UpdateCache(image, params, refresh);
        auto &cacheParams = ImageNavigatorUtils::gImageNavigatorTextureCache.GetCacheParams(image);
        auto &cacheImages = ImageNavigatorUtils::gImageNavigatorTextureCache.GetCacheImages(image);

        ImGui::PushID("##ImageNavigator"); ImGui::PushID(&image);
        cv::Point2d mouseLocation_originalImage = fnShowFullGui_WithBorder(cacheParams, cacheImages);
        ImGui::PopID(); ImGui::PopID();

        return mouseLocation_originalImage;
    }


    cv::Point2d ImageNavigator(
        const cv::Mat& image,
        const cv::Size& imageDisplaySize,
        const std::string& legend,
        bool refreshImage,
        bool showOptionsWhenAppearing,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey
    )
    {
        static std::map<const cv::Mat *, ImageNavigatorParams> s_Params;
        if (s_Params.find(&image) == s_Params.end())
        {
            ImageNavigatorParams params;
            params.ImageDisplaySize = imageDisplaySize;
            params.Legend = legend;
            params.ShowOptions = showOptionsWhenAppearing;
            params.ZoomKey = zoomKey;
            params.ColorAdjustmentsKey = colorAdjustmentsKey;
            s_Params[&image] = params;
        }

        ImageNavigatorParams* params = & s_Params.at(&image);

        return ImageNavigator(image, params, refreshImage);
    }



    ////////////////////////////////////////////
    // Inspector
    ////////////////////////////////////////////

    struct Inspector_ImageAndParams
    {
        cv::Mat Image;
        ImageNavigatorParams Params;

        const cv::Point2d InitialZoomCenter = cv::Point2d();
        double InitialZoomRatio = 1.;
        bool WasSentToTextureCache = false;
    };

    static std::vector<Inspector_ImageAndParams> s_Inspector_ImagesAndParams;
    static size_t s_Inspector_CurrentIndex = 0;


    void Inspector_AddImage(
        const cv::Mat& image,
        const std::string& legend,
        const std::string& zoomKey,
        const std::string& colorAdjustmentsKey,
        const cv::Point2d & zoomCenter,
        double zoomRatio,
        bool isColorOrderBGR
    )
    {
        ImageNavigatorParams params;
        params.Legend = legend;
        params.IsColorOrderBGR = isColorOrderBGR;
        params.ZoomKey = zoomKey;
        params.ColorAdjustmentsKey = colorAdjustmentsKey;
        params.ShowOptions = true;

        s_Inspector_ImagesAndParams.push_back({image, params, zoomCenter, zoomRatio});
    }

    void priv_Inspector_ShowImagesListbox(float width)
    {
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        if (ImGui::BeginListBox("##ImageNavigatorList",
                                ImVec2(width - 10.f, ImGui::GetContentRegionAvail().y)))
        {
            for (size_t i = 0; i < s_Inspector_ImagesAndParams.size(); ++i)
            {
                const bool is_selected = (s_Inspector_CurrentIndex == i);

                std::string id = s_Inspector_ImagesAndParams[i].Params.Legend + "##_" + std::to_string(i);
                auto &cacheImage = ImageNavigatorUtils::gImageNavigatorTextureCache.GetCacheImages(
                    s_Inspector_ImagesAndParams[i].Image);

                ImVec2 itemSize(width - 10.f, 40.f);
                float imageHeight = itemSize.y - ImGui::GetTextLineHeight();
                ImVec2 pos = ImGui::GetCursorScreenPos();
                if (ImGui::Selectable(id.c_str(), is_selected, 0, itemSize))
                    s_Inspector_CurrentIndex = i;

                float imageRatio = cacheImage.GlTexture->mImageSize.x / cacheImage.GlTexture->mImageSize.y;
                ImVec2 image_tl(pos.x, pos.y + ImGui::GetTextLineHeight());
                ImVec2 image_br(pos.x + imageRatio * imageHeight, image_tl.y + imageHeight);

                ImGuiImmGlImage::GetWindowDrawList_AddImage(cacheImage.GlTexture->mImTextureId, image_tl, image_br);
            }
            ImGui::EndListBox();
        }
    };

    void priv_Inspector_CleanImagesParams(const ImVec2& imageSize)
    {
        for (auto& i :s_Inspector_ImagesAndParams)
        {
            // Force image size
            i.Params.ImageDisplaySize = cv::Size((int)imageSize.x, (int)imageSize.y);

            // Store in texture cache
            if (! i.WasSentToTextureCache)
            {
                if (i.InitialZoomRatio > 0.)
                {
                    i.Params.ZoomMatrix = ZoomPanTransform::MakeZoomMatrix(
                        i.InitialZoomCenter, i.InitialZoomRatio, i.Params.ImageDisplaySize);
                }
                ImageNavigatorUtils::gImageNavigatorTextureCache.UpdateCache(i.Image, &i.Params, true);
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
                v.Params.ShowLegendBorder = currentParams.ShowLegendBorder;
                v.Params.ShowOptions = currentParams.ShowOptions;
                v.Params.ShowOptionsInTooltip = currentParams.ShowOptionsInTooltip;
                v.Params.PanWithMouse = currentParams.PanWithMouse;
                v.Params.ZoomWithMouseWheel = currentParams.ZoomWithMouseWheel;
            }
        }
    };

    ImVec2 priv_Inspector_ImageSize(float listWidth, bool showOptionsColumn)
    {
        ImVec2 imageSize;

        float x_margin = 30.f;
        float y_margin = 5.f;
        float navigator_info_height = 120.f;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto &params = s_Inspector_ImagesAndParams.front().Params;
            if (!params.ShowImageInfo)
                navigator_info_height -= 20.f;
            if (!params.ShowPixelInfo)
                navigator_info_height -= 20.f;
        }
        float navigator_options_width = showOptionsColumn ? 300.f : 0.f;
        ImVec2 winSize = ImGui::GetWindowSize();
        imageSize = ImVec2(
            winSize.x - listWidth - x_margin - navigator_options_width,
            winSize.y - y_margin - navigator_info_height);
        if (imageSize.x < 1.f)
            imageSize.x = 1.f;
        if (imageSize.y < 1.f)
            imageSize.y = 1.f;
        return imageSize;
    };


    void Inspector_Show()
    {
        ImageNavigatorWidgets::s_CollapsingHeader_CacheState_Sync = true;

        bool showOptionsColumn = true;
        if (!s_Inspector_ImagesAndParams.empty())
        {
            const auto& params = s_Inspector_ImagesAndParams.front().Params;
            if ( (params.ShowOptionsInTooltip) || (!params.ShowOptions))
                showOptionsColumn = false;
        }

        static float listWidth = ImGui::GetWindowSize().x / 10.f;

        ImVec2 imageSize = priv_Inspector_ImageSize(listWidth, showOptionsColumn);
        priv_Inspector_CleanImagesParams(imageSize);

        ImGui::Columns(2);

        //
        // First column: image list
        //
        {
            // Set column width
            {
                static bool wasWidthSet = false;
                if (!wasWidthSet)
                {
                    ImGui::SetColumnWidth(0, listWidth);
                    wasWidthSet = true;
                }
                ImGui::Text("Image list");
                listWidth = ImGui::GetColumnWidth(0);
            }
            // Show image list
            priv_Inspector_ShowImagesListbox(listWidth);
        }

        ImGui::NextColumn();

        //
        // Second column : navigator
        //
        {
            if (s_Inspector_ImagesAndParams.empty())
                s_Inspector_CurrentIndex = 0;
            else if (s_Inspector_CurrentIndex >= s_Inspector_ImagesAndParams.size())
                s_Inspector_CurrentIndex = s_Inspector_ImagesAndParams.size() - 1;

            if (s_Inspector_CurrentIndex < s_Inspector_ImagesAndParams.size())
            {
                auto& imageAndParams = s_Inspector_ImagesAndParams[s_Inspector_CurrentIndex];
                ImageNavigator(imageAndParams.Image, &imageAndParams.Params);
            }
        }

        ImGui::Columns(1);

        ImageNavigatorWidgets::s_CollapsingHeader_CacheState_Sync = false;
    }

    void Inspector_ClearImages()
    {
        s_Inspector_ImagesAndParams.clear();
    }

} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/immvision.cpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace immvision
{
    void foo()
    {
        std::cout << "foo()" << std::endl;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/color_adjustment_utils.cpp                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace ColorAdjustmentsUtils
    {
        bool IsNone(const ColorAdjustmentsValues& a)
        {
            return (a.Factor == 1.) && (a.Delta == 0.);
        }

        bool IsEqual(const ColorAdjustmentsValues& v1, const ColorAdjustmentsValues& v2)
        {
            if (fabs(v2.Factor - v1.Factor) > 1E-6)
                return false;
            if (fabs(v2.Delta - v1.Delta) > 1E-6)
                return false;
            return true;
        }

        cv::Mat Adjust(const ColorAdjustmentsValues& a, const cv::Mat &image)
        {
            if (IsNone(a))
                return image;
            else return
                    image * a.Factor + a.Delta;
        }

        ColorAdjustmentsValues ComputeInitialImageAdjustments(const cv::Mat& m)
        {
            ColorAdjustmentsValues r;
            if ((m.depth() == CV_32F) || (m.depth() == CV_64F))
            {
                std::vector<double> minima, maxima;
                std::vector<cv::Mat> channels;
                cv::split(m, channels);
                for (const cv::Mat&channel: channels)
                {
                    double min, max;
                    cv::minMaxLoc(channel, &min, &max);
                    minima.push_back(min);
                    maxima.push_back(max);
                }

                double min = *std::min_element(minima.begin(), minima.end());
                double max = *std::max_element(maxima.begin(), maxima.end());
                r.Factor = 1. / (max - min);
                r.Delta = -min * r.Factor;
            }
            return r;
        }

    } // namespace ImageAdjustmentsUtils

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/cv_drawing_utils.cpp                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        } // namespace MathUtils


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
            auto lines = SplitString(msg, '\n');

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
                                int thickness /*= 1*/)
        {
            if (add_cartouche)
                for (auto x : std::vector<double>{-1., 1.})
                    for (auto y : std::vector<double>{-1., 1.})
                        cross_hole(img, position + cv::Point2d(x, y), _ContrastColor(color), size, size_hole, thickness);

            cross_hole(img, position, color, size, size_hole, thickness);
            double delta_y = size_hole + size + 6.;
            cv::Point2d text_position = {position.x, position.y - delta_y};
            text(img, text_position, name, color, true, add_cartouche);
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl_provider.cpp                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef IMMVISION_BUILDING_PYBIND // see gl_provider_python for the pybind version


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl_provider.h included by src/immvision/internal/gl_provider.cpp//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision_GlProvider
{
    // InitGlProvider must be called after the OpenGl Loader is initialized
    void InitGlProvider();
    // InitGlProvider must be called before the OpenGl Loader is reset
    void ResetGlProvider();

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, unsigned int textureId);
    unsigned int GenTexture();
    void DeleteTexture(unsigned int texture_id);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl_provider.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision_gl_loader/immvision_gl_loader.h included by src/immvision/internal/gl_provider.cpp//
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
//                       src/immvision/internal/gl_provider.cpp continued                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision_GlProvider
{
    void _AssertOpenGlLoaderWorking()
    {
        size_t glGenTexturesAddress = (size_t)glGenTextures;
        size_t glDeleteTexturesAddress = (size_t)glDeleteTextures;

        if ((glGenTexturesAddress == 0) || (glDeleteTexturesAddress == 0))
        {
            const char* err_msg = "glGenTextures/glDeleteTexturesAddress address not initialized. Is your your OpenGL Loader initialized?";
            std::cerr << err_msg;
            assert(false);
            throw std::runtime_error(err_msg);
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
        ImmVision::ClearAllTextureCaches();
    }

    void Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height, unsigned int textureId)
    {
        static int counter = 0;
        ++counter;
        std::cout << "Blit_RGBA_Buffer counter=" << counter << "\n";
        glBindTexture(GL_TEXTURE_2D, textureId);
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

    unsigned int GenTexture()
    {
        std::cout << "GenTexture()\n";
        _AssertOpenGlLoaderWorking();
        GLuint textureId_Gl;
        glGenTextures(1, &textureId_Gl);
        return textureId_Gl;
    }

    void DeleteTexture(unsigned int texture_id)
    {
        std::cout << "DeleteTexture()\n";
        _AssertOpenGlLoaderWorking();
        glDeleteTextures(1, &texture_id);
    }
}

#endif // #ifndef IMMVISION_BUILDING_PYBIND


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/gl_texture.cpp                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    GlTexture::GlTexture()
    {
        unsigned int textureId_Gl = ImmVision_GlProvider::GenTexture();
        this->mImTextureId = textureId_Gl;
    }

    GlTexture::~GlTexture()
    {
        ImmVision_GlProvider::DeleteTexture(mImTextureId);
    }

    void GlTexture::Draw(const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) const
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->mImageSize;
        ImGuiImmGlImage::Image(this->mImTextureId, size_, uv0, uv1, tint_col, border_col);
    }

    bool GlTexture::DrawButton(const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) const
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->mImageSize;
        return ImGuiImmGlImage::ImageButton(this->mImTextureId, size_, uv0, uv1, frame_padding, bg_col, tint_col);
    }

    void GlTexture::Draw_DisableDragWindow(const ImVec2 &size) const
    {
        ImVec2 size_(size);
        if (size.x == 0.f)
            size_ = this->mImageSize;

        ImVec2 imageTl = ImGui::GetCursorScreenPos();
        ImVec2 imageBr(imageTl.x + size.x, imageTl.y + size.y);
        std::stringstream id;
        id << "##" << mImTextureId;
        ImGui::InvisibleButton(id.str().c_str(), size);
        ImGuiImmGlImage::GetWindowDrawList_AddImage(mImTextureId, imageTl, imageBr);
    }

    void GlTexture::Blit_RGBA_Buffer(unsigned char *image_data, int image_width, int image_height)
    {
        ImmVision_GlProvider::Blit_RGBA_Buffer(image_data, image_width, image_height, this->mImTextureId);
        mImageSize = ImVec2((float)image_width, (float) image_height);
    }

    //
    // ImageTextureCv
    //
    GlTextureCv::GlTextureCv(const cv::Mat& mat, bool isBgrOrBgra) : GlTextureCv()
    {
        BlitMat(mat, isBgrOrBgra);
    }

    void GlTextureCv::BlitMat(const cv::Mat& mat, bool isBgrOrBgra)
    {
        if (mat.empty())
            return;
        cv::Mat mat_rgba = CvDrawingUtils::converted_to_rgba_image(mat, isBgrOrBgra);

        Blit_RGBA_Buffer(mat_rgba.data, mat_rgba.cols, mat_rgba.rows);
    }
} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/imgui_imm.cpp                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IMGUI_DEFINE_MATH_OPERATORS

#include <sstream>
#include <stack>

namespace ImGuiImm
{
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
    {
        float vf = (float)*v;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, format, flags);
        if (changed)
            *v = (double)vf;
        return changed;
    }

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
    cv::Size ComputeDisplayImageSize(cv::Size askedImageSize, cv::Size realImageSize)
    {
        auto toSize = [](ImVec2 v) { return cv::Size((int)((double)v.x + 0.5), (int)((double)v.y + 0.5)); };
        auto toImVec2 = [](cv::Size v) { return ImVec2((float)v.width, (float)v.height); };
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
        float xLineStart = IM_FLOOR(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
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
            effectiveSize.x = ImGui::GetWindowContentRegionWidth();
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
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
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
        colors[ImGuiCol_TabActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_TabUnfocused]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
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
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
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
        colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
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
        colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
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
            for (int i = 0; i <= ImGuiCol_COUNT; i++)
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
            for (int i = 0; i <= ImGuiCol_COUNT; i++)
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
//                       src/immvision/internal/imgui_imm_gl_image.cpp                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImGuiImmGlImage
{
//#define IMMVISION_BUILDING_PYBIND // KK
    static void* UintToTextureID(unsigned int id)
    {
        return (void *)(intptr_t)id;
    }

#ifndef IMMVISION_BUILDING_PYBIND
    void  Image(unsigned int user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
    {
        ImGui::Image(UintToTextureID(user_texture_id), size, uv0, uv1, tint_col, border_col);
    }
    bool  ImageButton(unsigned int user_texture_id, const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
    {
        return ImGui::ImageButton(UintToTextureID(user_texture_id), size, uv0, uv1, frame_padding, bg_col, tint_col);
    }
    void  GetWindowDrawList_AddImage(unsigned int user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col)
    {
        ImGui::GetWindowDrawList()->AddImage(UintToTextureID(user_texture_id), p_min, p_max, uv_min, uv_max, col);
    }
#endif
} // namespace ImGuiImmGlImage


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/internal_icons.cpp                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace ImmVision
{
    namespace Icons
    {
        static cv::Size iconsSizeDraw(200, 200);
        auto ScalePoint = [](cv::Point2d p) {
            return cv::Point2d(p.x * (double) iconsSizeDraw.width, p.y * (double) iconsSizeDraw.height);
        };
        auto ScaleDouble = [](double v) {
            return v * (double) iconsSizeDraw.width;
        };
        auto ScaleInt = [](double v) {
            return (int) (v * (double) iconsSizeDraw.width + 0.5);
        };

        auto PointFromOther = [](cv::Point2d o, double angleDegree, double distance) {
            double m_pi = 3.14159265358979323846;
            double angleRadian = -angleDegree / 180. * m_pi;
            cv::Point2d r(o.x + cos(angleRadian) * distance, o.y + sin(angleRadian) * distance);
            return r;
        };


        cv::Mat MakeMagnifierImage(IconType iconType)
        {
            using namespace ImmVision;
            cv::Mat m(iconsSizeDraw, CV_8UC4);


            // Transparent background
            m = cv::Scalar(0, 0, 0, 0);

            cv::Scalar color(255, 255, 255, 255);
            double radius = 0.3;
            cv::Point2d center(1. - radius * 1.3, radius * 1.2);
            // Draw shadow
            {
                cv::Point2d decal(radius * 0.1, radius * 0.1);
                cv::Scalar color_shadow(127, 127, 127, 255);

                CvDrawingUtils::circle(
                    m, //image,
                    ScalePoint(center + decal),
                    ScaleDouble(radius), //radius
                    color_shadow,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 1.7) + decal),
                    ScalePoint(PointFromOther(center, 225., radius * 1.03) + decal),
                    color_shadow,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 2.3) + decal),
                    ScalePoint(PointFromOther(center, 225., radius * 1.5) + decal),
                    color_shadow,
                    ScaleInt(0.14)
                );
            }
            // Draw magnifier
            {
                CvDrawingUtils::circle(
                    m, //image,
                    ScalePoint(center),
                    ScaleDouble(radius), //radius
                    color,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 1.7)),
                    ScalePoint(PointFromOther(center, 225., radius * 1.03)),
                    color,
                    ScaleInt(0.08)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 225., radius * 2.3)),
                    ScalePoint(PointFromOther(center, 225., radius * 1.5)),
                    color,
                    ScaleInt(0.14)
                );
            }

            if (iconType == IconType::ZoomPlus)
            {
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 0., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 180., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 90., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 270., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
            }
            if (iconType == IconType::ZoomMinus)
            {
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(PointFromOther(center, 0., radius * 0.6)),
                    ScalePoint(PointFromOther(center, 180., radius * 0.6)),
                    color,
                    ScaleInt(0.06)
                );
            }
            if (iconType == IconType::ZoomScaleOne)
            {
                cv::Point2d a = PointFromOther(center, -90., radius * 0.45);
                cv::Point2d b = PointFromOther(center, 90., radius * 0.45);
                a.x += radius * 0.05;
                b.x += radius * 0.05;
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(a),
                    ScalePoint(b),
                    color,
                    ScaleInt(0.06)
                );
                cv::Point2d c(b.x - radius * 0.2, b.y + radius * 0.2);
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(b),
                    ScalePoint(c),
                    color,
                    ScaleInt(0.06)
                );
            }

            return m;
        }


        cv::Mat MakeFullViewImage()
        {
            cv::Mat m(iconsSizeDraw, CV_8UC4);
            m = cv::Scalar(0, 0, 0, 0);

            cv::Scalar color(255, 255, 255, 255);
            double decal = 0.1;
            double length_x = 0.3, length_y = 0.3;
            for (int y = 0; y <= 1; ++y)
            {
                for (int x = 0; x <= 1; ++x)
                {
                    cv::Point2d corner;

                    corner.x = (x == 0) ? decal : 1. - decal;
                    corner.y = (y == 0) ? decal : 1. - decal;
                    double moveX = (x == 0) ? length_x : -length_x;
                    double moveY = (y == 0) ? length_y : -length_y;
                    cv::Point2d a = corner;
                    cv::Point2d b(a.x + moveX, a.y);
                    cv::Point2d c(a.x, a.y + moveY);
                    CvDrawingUtils::line(
                        m, //image,
                        ScalePoint(a),
                        ScalePoint(b),
                        color,
                        ScaleInt(0.06)
                    );
                    CvDrawingUtils::line(
                        m, //image,
                        ScalePoint(a),
                        ScalePoint(c),
                        color,
                        ScaleInt(0.06)
                    );

                }
            }
            return m;
        }

        cv::Mat MakeAdjustLevelsImage()
        {
            cv::Mat m(iconsSizeDraw, CV_8UC4);
            m = cv::Scalar(0, 0, 0, 0);
            cv::Scalar color(255, 255, 255, 255);

            double yMin = 0.15, yMax = 0.8;
            int nbBars = 3;
            for (int bar = 0; bar < nbBars; ++bar)
            {
                double xBar = (double)bar / ((double)(nbBars) + 0.17) + 0.2;
                cv::Point2d a(xBar, yMin);
                cv::Point2d b(xBar, yMax);
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(a),
                    ScalePoint(b),
                    color,
                    ScaleInt(0.08)
                );

                double barWidth = 0.1;
                double yBar = 0.7 - 0.2 * (double)bar;
                cv::Point2d c(a.x - barWidth / 2., yBar);
                cv::Point2d d(a.x + barWidth / 2., yBar);
                CvDrawingUtils::line(
                    m, //image,
                    ScalePoint(c),
                    ScalePoint(d),
                    color,
                    ScaleInt(0.16)
                );
            }

            return m;
        }


        static std::map<IconType, std::unique_ptr<GlTextureCv>> sIconsTextureCache;
        static cv::Size gIconSize(20,  20);

        unsigned int GetIcon(IconType iconType)
        {
            if (sIconsTextureCache.find(iconType) == sIconsTextureCache.end())
            {
                cv::Mat m;
                if (iconType == IconType::ZoomFullView)
                    m = MakeFullViewImage();
                else if (iconType == IconType::AdjustLevels)
                    m = MakeAdjustLevelsImage();
                else
                    m = MakeMagnifierImage(iconType);

                cv::Mat resized = m;
                cv::resize(m, resized, cv::Size(gIconSize.width * 2, gIconSize.height * 2), 0., 0., cv::INTER_AREA);
                auto texture = std::make_unique<GlTextureCv>(resized, true);
                sIconsTextureCache[iconType] = std::move(texture);
            }
            return sIconsTextureCache[iconType]->mImTextureId;
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
            std::string spaceLabel = " ";
            while (ImGui::CalcTextSize(spaceLabel.c_str()).x < 14.f)
                spaceLabel += " ";
            bool clicked = ImGui::Button(spaceLabel.c_str());

            ImGuiImmGlImage::GetWindowDrawList_AddImage(
                GetIcon(iconType),
                cursorPos,
                {cursorPos.x + (float)gIconSize.width, cursorPos.y + (float)gIconSize.height},
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
            static cv::Mat mag = MakeMagnifierImage(IconType::ZoomScaleOne);
            static cv::Mat img = MakeAdjustLevelsImage();

            static ImmVision::ImageNavigatorParams imageNavigatorParams1;
            imageNavigatorParams1.ImageDisplaySize = {400, 400};
            ImmVision::ImageNavigator(mag, &imageNavigatorParams1);

            ImGui::SameLine();

            static ImmVision::ImageNavigatorParams imageNavigatorParams2;
            imageNavigatorParams2.ImageDisplaySize = {400, 400};
            ImmVision::ImageNavigator(img, &imageNavigatorParams2);

            ImVec2 iconSize(15.f, 15.f);
            ImGuiImmGlImage::ImageButton(GetIcon(IconType::ZoomScaleOne), iconSize);
            ImGuiImmGlImage::ImageButton(GetIcon(IconType::ZoomPlus), iconSize);
            ImGuiImmGlImage::ImageButton(GetIcon(IconType::ZoomMinus), iconSize);
            ImGuiImmGlImage::ImageButton(GetIcon(IconType::ZoomFullView), iconSize);
            ImGuiImmGlImage::ImageButton(GetIcon(IconType::AdjustLevels), iconSize);
        }

        void ClearIconsTextureCache()
        {
            Icons::sIconsTextureCache.clear();
        }

} // namespace Icons


} // namespace ImmVision


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/matrix_info_utils.cpp                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef CV_16F // for old versions of OpenCV
#define CV_16F 7
#endif

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const cv::Mat& m)
        {
            std::map<int, std::string> depthNames
                {
                    { CV_8U, "CV_8U" },
                    { CV_8S, "CV_8S" },
                    { CV_16U, "CV_16U" },
                    { CV_16S, "CV_16S" },
                    { CV_32S, "CV_32S"},
                    { CV_32F, "CV_32F"},
                    { CV_64F, "CV_64F"},
                    { CV_16F, "CV_16F"}
                };
            return depthNames.at(m.depth()) + "C" + std::to_string(m.channels());
        }

        std::string _MatInfo(const cv::Mat &m)
        {
            return _MatTypeName(m) + " " + std::to_string(m.cols) + "x" + std::to_string(m.rows);
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
        std::vector<double> GrabValuesFromBuffer(const uchar * buffer, int nb)
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

        std::vector<double> MatValuesAt(const cv::Mat& m, int x, int y)
        {
            int depth = m.depth();
            int nb_channels = m.channels();
            const uchar * ptr = m.ptr(y, x);
            if (depth == CV_8U)
                return GrabValuesFromBuffer<uchar>(ptr, nb_channels);
            else if (depth == CV_8S)
                return GrabValuesFromBuffer<uchar>(ptr, nb_channels);
            else if (depth == CV_16U)
                return GrabValuesFromBuffer<uint16_t>(ptr, nb_channels);
            else if (depth == CV_16S)
                return GrabValuesFromBuffer<int16_t>(ptr, nb_channels);
#if CV_MAJOR_VERSION >= 4
                else if (depth == CV_16F)
                return GrabValuesFromBuffer<cv::float16_t>(ptr, nb_channels);
#endif
            else if (depth == CV_32S)
                return GrabValuesFromBuffer<int32_t>(ptr, nb_channels);
            else if (depth == CV_32F)
                return GrabValuesFromBuffer<float>(ptr, nb_channels);
            else if (depth == CV_64F)
                return GrabValuesFromBuffer<double>(ptr, nb_channels);
            else
                throw std::runtime_error("MatValuesAt: unhandled depth");
        }

        std::string MatPixelColorInfo(const cv::Mat & m, int x, int y, char separator, bool add_paren)
        {
            if (!cv::Rect(cv::Point(0, 0), m.size()).contains(cv::Point(x, y)))
                return "";
            std::vector<double> values = MatValuesAt(m, x, y);

            auto formatValue = [](double v, int depth) -> std::string
            {
                bool isFloat = false;
                if ((depth == CV_32F) || (depth == CV_64F))
                    isFloat = true;
#if CV_MAJOR_VERSION >= 4
                if (depth == CV_16F)
                    isFloat = true;
#endif
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
            int depth = m.depth();
            for (double v: values)
                strs.push_back(formatValue(v, depth));

            return JoinStrings(strs, ',');
        }

    } // namespace MatrixInfoUtils

} // namespace ImmVision

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       src/immvision/internal/short_lived_cache.cpp                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
//                       src/immvision/internal/zoom_pan_transform.cpp                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = cv::Matx33d;

        MatrixType Identity()
        {
            return cv::Matx33d::eye();
        }

        MatrixType ComputeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            mat(0, 2) = zoomCenter.x * (1. - zoomRatio);
            mat(1, 2) = zoomCenter.y * (1. - zoomRatio);
            return mat;
        }

        MatrixType ComputePanMatrix(const cv::Point2d& dragDelta, double currentZoom)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 2) = (double)dragDelta.x / currentZoom;
            mat(1, 2) = (double)dragDelta.y / currentZoom;
            return mat;
        }

        MatrixType MakeScaleOne(cv::Size imageSize, cv::Size viewportSize)
        {
            MatrixType r = Identity();
            r(0, 2) = (viewportSize.width / 2 - imageSize.width / 2);
            r(1, 2) = (viewportSize.height / 2 - imageSize.height / 2);
            return r;
        }

        cv::Matx23d ZoomMatrixToM23(const cv::Matx33d &m)
        {
            cv::Matx23d r;
            for (int y = 0; y < 2; y++)
                for (int x = 0; x < 3; x++)
                    r(y, x) = m(y, x);
            return r;
        }

        MatrixType MakeFullView(cv::Size imageSize, cv::Size viewportSize)
        {
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

        cv::Point2d Apply(const MatrixType& zoomMatrix, const cv::Point2d &p)
        {
            cv::Matx31d pMat(p.x, p.y, 1.);
            cv::Matx31d rMat = zoomMatrix * pMat;
            cv::Point2d r(rMat(0, 0), rMat(1, 0));
            return r;
        }

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const cv::Size& oldDisplaySize, const cv::Size& newDisplaySize)
        {
            if (oldDisplaySize.area() == 0 || newDisplaySize.area() == 0)
                return oldZoomMatrix;

            MatrixType zoomMatrix;

            auto fnImageCenter = [](const cv::Size s) {
                return cv::Point2d((double)s.width / 2., (double)s.height / 2.);
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

            cv::Point2d translation;
            {
                cv::Point2d oldDisplayCenter_Zoomed = fnImageCenter(oldDisplaySize);
                cv::Point2d oldDisplayCenter_Image = ZoomPanTransform::Apply(oldZoomMatrix.inv(), oldDisplayCenter_Zoomed);

                cv::Point2d newDisplayCenter_Zoomed_Wanted = fnImageCenter(newDisplaySize);
                cv::Point2d newDisplayCenter_Zoomed_Now = ZoomPanTransform::Apply(zoomMatrix, oldDisplayCenter_Image);
                translation = newDisplayCenter_Zoomed_Wanted - newDisplayCenter_Zoomed_Now;
            }

            zoomMatrix(0, 2) = translation.x;
            zoomMatrix(1, 2) = translation.y;

            return zoomMatrix;
        }

        cv::Matx33d MakeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio,const cv::Size displayedImageSize)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            double dx = (double)displayedImageSize.width / 2. - zoomRatio * zoomCenter.x;
            double dy = (double)displayedImageSize.height / 2. - zoomRatio * zoomCenter.y;
            mat(0, 2) = dx;
            mat(1, 2) = dy;
            return mat;
        }

    } // namespace ZoomPanTransform

    cv::Matx33d MakeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio,const cv::Size displayedImageSize)
    {
        return ZoomPanTransform::MakeZoomMatrix(zoomCenter, zoomRatio, displayedImageSize);
    }

}

