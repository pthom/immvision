#pragma once
#include "immvision/image.h"
#include "immvision/gl_texture.h"

namespace ImmVision
{
    namespace ImageDrawing
    {
        ImageBuffer DrawWatchedPixels(const ImageBuffer& image, const ImageParams& params);

        void DrawGrid(ImageBuffer& inOutImageRgba, const ImageParams& params);

        ImageBuffer DrawValuesOnZoomedPixels(const ImageBuffer& drawingImage, const ImageBuffer& valuesImage,
                                         const ImageParams& params, bool drawPixelCoords);

        ImageBuffer MakeSchoolPaperBackground(Size s);

        // Old pipeline: CPU warp + annotations → fills GlTexture (to be removed)
        void BlitImageTexture(
            const ImageParams& params,
            const ImageBuffer& image,
            ImageBuffer& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTexture* outTexture
        );

        // New pipeline: RGBA conversion + texture upload only (no warp, no annotations).
        // Annotations are drawn via DrawList by the caller.
        void UpdateImageTexture(
            const ImageParams& params,
            const ImageBuffer& image,
            ImageBuffer& in_out_rgba_image_cache,
            bool shall_refresh_rgba,
            GlTexture* outTexture
        );

        bool HasColormapParam(const ImageParams& params);

    } // namespace ImageDrawing

} // namespace ImmVision
