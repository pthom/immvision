#include "immvision/image.h"

#include "immvision/internal/gl_texture.h"

#include <map>
#include <chrono>

namespace ImmVision
{
    namespace internal
    {
        double HighResTimer()
        {
            using chrono_second = std::chrono::duration<double, std::ratio<1>>;
            using chrono_clock = std::chrono::high_resolution_clock;

            static std::chrono::time_point<chrono_clock> startTime = chrono_clock::now();
            double elapsed = std::chrono::duration_cast<chrono_second>(chrono_clock::now() - startTime).count();
            return elapsed;
        }

        struct CachedTexture
        {
            CachedTexture() : mGlTextureCv()
            {
                mLastUsageTime = HighResTimer();
            }
            void Ping()
            {
                mLastUsageTime = HighResTimer();
            }
            bool IsUnused() const
            {
                double now = HighResTimer();
                return ((now - mLastUsageTime) > 3.);
            }
            GlTextureCv mGlTextureCv;
            double mLastUsageTime;
        };

        // Cache for images that are not refreshed at each frame
        class TextureCache
        {
        public:
            GlTextureCv* GetTexture(const cv::Mat *mat)
            {
                mTexturesMap[mat].Ping();
                ClearNotUsedAnymoreTextures();
                return &mTexturesMap[mat].mGlTextureCv;
            }
            void ClearAllTextures()
            {
                mTexturesMap.clear();
            }
        private:
            void ClearNotUsedAnymoreTextures()
            {
                std::vector<const cv::Mat*> unused;
                for (const auto& kv: mTexturesMap)
                    if (kv.second.IsUnused())
                        unused.push_back(kv.first);

               for (auto& v: unused)
                   mTexturesMap.erase(v);
            }
        private:
            std::map<const cv::Mat *, CachedTexture> mTexturesMap;
        };
        TextureCache gTextureCache;

        ImVec2 ComputeFinalDisplayImageSize(
            ImVec2 askedImageSize,
            ImVec2 realImageSize
            )
        {
            if ((askedImageSize.x == 0.f) && (askedImageSize.y == 0.f))
                return realImageSize;
            else if (askedImageSize.x == 0.f)
                return ImVec2(askedImageSize.y * realImageSize.x / realImageSize.y, askedImageSize.y);
            else if (askedImageSize.y == 0.f)
                return ImVec2(askedImageSize.x, askedImageSize.x * realImageSize.y / realImageSize.x);
            else
                return askedImageSize;
        }

        struct
        {
            cv::Point2d lastDisplayRatio = cv::Point2d(1., 1.);
            ImVec2      lastImageCursorPos = ImVec2(0.f, 0.f);
        } gLastImageInfo;


    } // namespace internal


    void Image(
        const cv::Mat &mat,
        bool refresh,
        const cv::Size& size
    )
    {
        GlTextureCv* glTextureCv = internal::gTextureCache.GetTexture(&mat);

        // Detect if the texture was not transfered to OpenGL yet
        if ((glTextureCv->mImageSize.x == 0.f) && (glTextureCv->mImageSize.y == 0.f))
            refresh = true;

        if (refresh)
            glTextureCv->BlitMat(mat);

        ImVec2 sizeImVec2((float)size.width, (float)size.height);
        ImVec2 displaySize = internal::ComputeFinalDisplayImageSize(sizeImVec2, glTextureCv->mImageSize);

        internal::gLastImageInfo.lastDisplayRatio = cv::Point2d(
            (double)(glTextureCv->mImageSize.x / displaySize.x),
            (double)(glTextureCv->mImageSize.y / displaySize.y)
        );
        internal::gLastImageInfo.lastImageCursorPos = ImGui::GetCursorScreenPos();

        ImGui::Image(glTextureCv->mImTextureId, displaySize);
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

    void ClearTextureCache()
    {
        internal::gTextureCache.ClearAllTextures();
    }
    
} // namespace ImmVision
