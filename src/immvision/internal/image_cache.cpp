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


        //
        // ImageTextureCache impl below
        //

        bool ImageTextureCache::UpdateCache(const std::string& id_label, const cv::Mat& image, ImageParams* params, bool userRefresh)
        {
            auto cacheKey = GetID(id_label);
            params->ImageDisplaySize = ImGuiImm::ComputeDisplayImageSize(params->ImageDisplaySize, image.size());

            bool needsRefreshTexture = userRefresh;
            bool shallRefreshRgbaCache = false;

            bool isNewEntry = false;
            if (! mCacheParams.Contains(cacheKey))
            {
                isNewEntry = true;
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
            cachedParams.Params = params;

            ImageParams oldParams = cachedParams.PreviousParams;
            *cachedParams.Params = *params;

            if (cachedImages.GlTexture->mImageSize.x == 0.f)
                needsRefreshTexture = true;
            if (ShallRefreshTexture(oldParams, *params))
                needsRefreshTexture = true;
            if (!(oldParams.ImageDisplaySize.area() == 0) && (oldParams.ImageDisplaySize != params->ImageDisplaySize))
                params->ZoomPanMatrix = ZoomPanTransform::UpdateZoomMatrix_DisplaySizeChanged(
                    oldParams.ZoomPanMatrix, oldParams.ImageDisplaySize, params->ImageDisplaySize);
            if (needsRefreshTexture)
            {
                if (ShallRefreshRgbaCache(oldParams, *params) || userRefresh)
                    shallRefreshRgbaCache = true;
                ImageDrawing::BlitImageTexture(
                    *params, image, cachedImages.ImageRgbaCache, shallRefreshRgbaCache, cachedImages.GlTexture.get());
            }

            if (! ZoomPanTransform::IsEqual(oldParams.ZoomPanMatrix, params->ZoomPanMatrix))
                UpdateLinkedZooms(id_label);
            if (! Colormap::IsEqual(oldParams.ColormapSettings, params->ColormapSettings))
                UpdateLinkedColorAdjustments(id_label);

            cachedParams.PreviousParams = *params;

            mCacheImages.ClearOldEntries();

            return isNewEntry;
        }

        KeyType ImageTextureCache::GetID(const std::string& id_label)
        {
            return hash_str(id_label);
        }

        ImageTextureCache::CachedParams& ImageTextureCache::GetCacheParams(const std::string& id_label)
        {
            return mCacheParams.Get(GetID(id_label));
        }
        ImageTextureCache::CachedImages& ImageTextureCache::GetCacheImages(const std::string& id_label)
        {
            return mCacheImages.Get(GetID(id_label));
        }

        void ImageTextureCache::ClearImagesCache()
        {
            mCacheImages.Clear();
        }

        void ImageTextureCache::UpdateLinkedZooms(const std::string& id_label)
        {
            auto currentCacheKey = GetID(id_label);
            auto & currentCache = mCacheParams.Get(currentCacheKey);
            std::string zoomKey = currentCache.Params->ZoomKey;
            if (zoomKey.empty())
                return;
            ZoomPanTransform::MatrixType newZoom = currentCache.Params->ZoomPanMatrix;
            for (auto& otherCacheKey : mCacheParams.Keys())
            {
                CachedParams & otherCache = mCacheParams.Get(otherCacheKey);
                if ((otherCacheKey != currentCacheKey) && (otherCache.Params->ZoomKey == zoomKey))
                    otherCache.Params->ZoomPanMatrix = newZoom;
            }
        }
        void ImageTextureCache::UpdateLinkedColorAdjustments(const std::string& id_label)
        {
            auto currentCacheKey = GetID(id_label);
            auto & currentCache = mCacheParams.Get(currentCacheKey);
            std::string colormapKey = currentCache.Params->ColormapKey;
            if (colormapKey.empty())
                return;
            ColormapSettingsData newColorAdjustments = currentCache.Params->ColormapSettings;
            for (auto& otherCacheKey : mCacheParams.Keys())
            {
                CachedParams & otherCache = mCacheParams.Get(otherCacheKey);
                if ((otherCacheKey != currentCacheKey) && (otherCache.Params->ColormapKey == colormapKey))
                    otherCache.Params->ColormapSettings = newColorAdjustments;
            }
        }


    ImageTextureCache gImageTextureCache;
    } // namespace ImageUtils


} // namespace ImmVision