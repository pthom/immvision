#include "immvision/image.h"

#include "immvision/internal/gl/gl_texture.h"
#include "immvision/internal/imgui_imm.h"
#include "immvision/immvision.h"
#include "immvision/internal/internal_icons.h"
#include "immvision/internal/short_lived_cache.h"

#include <map>
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
