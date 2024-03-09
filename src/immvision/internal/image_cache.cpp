#include "immvision/internal/image_cache.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/imgui_imm.h"
#include "immvision/internal/drawing/image_drawing.h"


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

        void InitializeMissingParams(ImageParams* params, const cv::Mat& image)
        {
            if (Colormap::IsNone(params->ColormapSettings))
                params->ColormapSettings = Colormap::ComputeInitialColormapSettings(image);
            {
                cv::Rect fullRoi(cv::Point2d(), image.size());
                Colormap::InitStatsOnNewImage(image, fullRoi, &params->ColormapSettings);
            }
            if (params->ZoomPanMatrix == cv::Matx33d::eye())
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
            if (v1.IsColorOrderBGR != v2.IsColorOrderBGR)
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
                mCacheImages.Get(key).GlTexture = std::make_unique<GlTextureCv>();
            }
            return isNewEntry;
        }


        bool ImageTextureCache::UpdateCache(KeyType id, const cv::Mat& image, ImageParams* params, bool userRefresh)
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
                        || (cachedImage.GlTexture->mImageSize.x == 0.f)
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
                    *params, image, cachedImage.ImageRgbaCache, shallRefreshRgbaCache, cachedImage.GlTexture.get());
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

            ZoomPanTransform::MatrixType newZoom = currentCache.ParamsPtr->ZoomPanMatrix;
            double currentZoomRatio = newZoom(0, 0);

            cv::Size displayedImageSize = currentCache.ParamsPtr->ImageDisplaySize;
            cv::Point2d visibleImageCenter_ImageCoords;
            {
                cv::Point2d visibleCenter_Viewport(
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
                    cv::Size otherDisplayedImageSize = otherCache.ParamsPtr->ImageDisplaySize;

                    double sizeRatioOtherImage = (double)otherDisplayedImageSize.width / (double)displayedImageSize.width;
                    double zoomRatioOtherImage = currentZoomRatio * sizeRatioOtherImage;
                    auto zoomMatrixOtherImage = ZoomPanTransform::MakeZoomMatrix(
                        visibleImageCenter_ImageCoords, zoomRatioOtherImage, otherDisplayedImageSize);
                    otherCache.ParamsPtr->ZoomPanMatrix = zoomMatrixOtherImage;
                    otherCache.PreviousParams.ZoomPanMatrix = zoomMatrixOtherImage;
                    // otherCache.ParamsPtr->RefreshImage = true;
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