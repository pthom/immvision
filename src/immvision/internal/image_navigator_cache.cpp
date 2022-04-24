#include "immvision/internal/image_navigator_cache.h"
#include "immvision/internal/cv/color_adjustment_utils.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/imgui_imm.h"
#include "immvision/internal/image_navigator_drawing.h"

namespace ImmVision
{
    namespace ImageNavigatorCache
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


        //
        // ImageNavigatorTextureCache impl below
        //

        void ImageNavigatorTextureCache::UpdateCache(const cv::Mat& image, ImageNavigatorParams* params, bool refresh)
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

        ImageNavigatorTextureCache::CachedParams& ImageNavigatorTextureCache::GetCacheParams(const cv::Mat& image)
        {
            return mCacheParams.Get(&image);
        }
        ImageNavigatorTextureCache::CachedImages& ImageNavigatorTextureCache::GetCacheImages(const cv::Mat& image)
        {
            return mCacheImages.Get(&image);
        }

        void ImageNavigatorTextureCache::ClearImagesCache()
        {
            mCacheImages.Clear();
        }

        void ImageNavigatorTextureCache::UpdateLinkedZooms(const cv::Mat& image)
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
        void ImageNavigatorTextureCache::UpdateLinkedColorAdjustments(const cv::Mat& image)
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


    ImageNavigatorTextureCache gImageNavigatorTextureCache;
    } // namespace ImageNavigatorUtils


} // namespace ImmVision