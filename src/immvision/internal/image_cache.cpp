#include "immvision/internal/image_cache.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/gl/gl_provider.h"
#include "immvision/imgui_imm.h"
#include "immvision/internal/drawing/draw_list_annotate.h"


namespace ImmVision
{
    namespace ImageCache
    {
        void UpdateImageTexture(
            const ImageParams& params,
            const ImageBuffer& image,
            ImageBuffer& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTexture* outTexture
        )
        {
            if (image.empty())
                return;

            if (shall_refresh_rgba)
            {
                ImageBuffer finalImage = image.clone();

                if (!params.ColormapKey.empty() && Colormap::CanColormap(image))
                {
                    finalImage = Colormap::ApplyColormap(finalImage, params.ColormapSettings);
                    // ApplyColormap returns RGBA; upload directly
                    in_out_rgba_image_cache = finalImage;
                    outTexture->UpdateFromImage(finalImage, false);
                }
                else
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

                    // Let UpdateFromImage handle RGBA conversion (including BGR swap)
                    bool is_color_order_bgr = IsUsingBgrColorOrder();
                    outTexture->UpdateFromImage(finalImage, is_color_order_bgr);
                    in_out_rgba_image_cache = finalImage;
                }
                assert(!in_out_rgba_image_cache.empty());
            }
        }

        static KeyType hash_str(const std::string& str)
        {
            const std::hash<std::string> hasher;
            size_t hashResult = hasher(str);
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
            // ShowAlphaChannelCheckerboard and ShowSchoolPaperBackground no longer
            // trigger texture re-upload — they are drawn via DrawList.
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
                {
                    InitializeMissingParams(params, image);

                    // If this new entry shares a ZoomKey with an existing entry,
                    // propagate that entry's zoom to all linked images (including this new one).
                    if (!params->ZoomKey.empty())
                    {
                        for (auto& otherKey : mCacheParams.Keys())
                        {
                            if (otherKey == id)
                                continue;
                            CachedParams& otherCache = mCacheParams.Get(otherKey);
                            if (otherCache.ParamsPtr && otherCache.ParamsPtr->ZoomKey == params->ZoomKey)
                            {
                                UpdateLinkedZooms(otherKey);
                                break;
                            }
                        }
                    }
                }

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

            // In the new GPU pipeline, the texture only needs re-upload when the
            // RGBA content changes (colormap, channel, image data). Zoom/pan, grid,
            // annotations are handled by UV coordinates and DrawList.
            bool shallRefreshRgba =
                    userRefresh
                    || isNewEntry
                    || (cachedImage.mGlTexture->ImageSize.empty())
                    || ShallRefreshRgbaCache(oldParams, *params);

            if (shallRefreshRgba)
            {
                UpdateImageTexture(
                    *params, image, cachedImage.mImageRgbaCache, true, cachedImage.mGlTexture.get());
            }

            // Set texture filtering based on zoom level
            {
                double zoom = params->ZoomPanMatrix(0, 0);
                using TF = ImmVision_GlProvider::TextureFilter;
                TF minF, magF;
                if (zoom >= 12.0)      { minF = TF::Nearest; magF = TF::Nearest; }
                else if (zoom >= 1.0)  { minF = TF::Linear;  magF = TF::Linear;  }
                else                   { minF = TF::LinearMipmapLinear; magF = TF::Linear; }
                ImmVision_GlProvider::SetTextureFiltering(cachedImage.mGlTexture->TextureId, minF, magF);
            }

            // Only propagate zoom when the user directly interacted (drag, wheel, buttons).
            if (cachedParams.UserInteractedWithZoom)
            {
                UpdateLinkedZooms(id);
                cachedParams.UserInteractedWithZoom = false;
            }
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
