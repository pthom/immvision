#pragma once
#include "immvision/image.h"
#include "immvision/internal/gl/gl_texture.h"
#include "immvision/internal/gl/short_lived_cache.h"


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
                cv::Size PreviousImageSize;
                struct ImageParams  PreviousParams;
            };
            struct CachedImageAndTexture
            {
                // These caches are heavy and will be destroyed
                // if not used (after about 5 seconds)
                cv::Mat     ImageRgbaCache;             // Image with applied colormap, alpha grid & paper background
                std::unique_ptr<GlTextureCv> GlTexture;
            };

            // returns true if new entry
            KeyType GetID(const std::string& id_label, bool use_id_stack);

            bool UpdateCache(KeyType id, const cv::Mat& image, ImageParams* params, bool userRefresh);
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