#pragma once
#include "immvision/image.h"
#include "immvision/internal/gl/gl_texture.h"
#include "immvision/internal/gl/short_lived_cache.h"


namespace ImmVision
{
    namespace ImageCache
    {
        class ImageTextureCache
        {
        public:
            // members
            struct CachedParams
            {
                // This caches are small and will persist during the application lifetime
                ImageParams* Params = nullptr;
                ImVec2 LastDragDelta;
                std::vector<char> FilenameEditBuffer = std::vector<char>(1000, '\0');
                bool   IsMouseDragging = false;
                struct ImageParams  PreviousParams;
            };
            struct CachedImages
            {
                // This caches are heavy and will be destroyed
                // if not used (after about 5 seconds)
                cv::Mat     ImageRgbaCache;
                std::unique_ptr<GlTextureCv> GlTexture;
            };

            void UpdateCache(const cv::Mat& image, ImageParams* params, bool refresh);

            CachedParams& GetCacheParams(const cv::Mat& image);
            CachedImages& GetCacheImages(const cv::Mat& image);
            void ClearImagesCache();

        private:
            // Methods
            void UpdateLinkedZooms(const cv::Mat& image);
            void UpdateLinkedColorAdjustments(const cv::Mat& image);

            internal::Cache<const cv::Mat *, CachedParams> mCacheParams;
            double mCachedImagesTimeToLive = 5.;
            internal::ShortLivedCache<const cv::Mat *, CachedImages> mCacheImages { mCachedImagesTimeToLive };
        };

        extern ImageTextureCache gImageNavigatorTextureCache;

    } // namespace ImageNavigatorUtils


} // namespace ImmVision