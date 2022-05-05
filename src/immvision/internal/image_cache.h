#pragma once
#include "immvision/image.h"
#include "immvision/internal/gl/gl_texture.h"
#include "immvision/internal/gl/short_lived_cache.h"


namespace ImmVision
{
    namespace ImageCache
    {
        using KeyType = std::size_t;

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


            // returns true if new entry
            bool UpdateCache(const std::string& id_label, const cv::Mat& image, ImageParams* params, bool userRefresh);
            KeyType GetID(const std::string& id_label);
            CachedParams& GetCacheParams(const std::string& id_label);
            CachedImages& GetCacheImages(const std::string& id_label);
            void ClearImagesCache();

        private:
            // Methods
            void UpdateLinkedZooms(const std::string& id_label);
            void UpdateLinkedColorAdjustments(const std::string& id_label);

            internal::Cache<KeyType, CachedParams> mCacheParams;
            double mCachedImagesTimeToLive = 5.;
            internal::ShortLivedCache<KeyType, CachedImages> mCacheImages { mCachedImagesTimeToLive };
        };

        extern ImageTextureCache gImageTextureCache;

    } // namespace ImageUtils


} // namespace ImmVision