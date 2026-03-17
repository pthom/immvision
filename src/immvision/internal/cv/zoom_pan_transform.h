#pragma once

#include "immvision/immvision_types.h"

namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = Matrix33d;

        MatrixType Identity();

        MatrixType ComputeZoomMatrix(const Point2d & zoomCenter, double zoomRatio);
        MatrixType ComputePanMatrix(const Point2d& dragDelta, double currentZoom);
        MatrixType MakeScaleOne(Size imageSize, Size viewportSize);
        MatrixType MakeFullView(Size imageSize, Size viewportSize);
        Matrix33d MakeZoomMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize);

        bool IsEqual(const MatrixType & v1, const MatrixType & v2);

        Point2d Apply(const MatrixType& zoomMatrix, const Point2d &p);

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const Size& oldDisplaySize, const Size& newDisplaySize);

        Rect VisibleRoi(const MatrixType & zoomMatrix,
                            Size imageDisplaySize,
                            Size originalImageSize
                            );

        // Custom warp for scale+translate transforms (replaces cv::warpAffine)
        enum class WarpInterp { Nearest, Bilinear, Area };
        void WarpAffineScaleTranslate(const ImageBuffer& src, ImageBuffer& dst, const Matrix33d& m, WarpInterp interp);
        void _WarpAffineInterAreaForSmallSizes(const ImageBuffer& src, ImageBuffer& dst, const Matrix33d& m);

        // Compute UV coordinates and widget placement for GPU-based rendering.
        // Given the zoom/pan matrix, image size, and display size, computes:
        //   - uv0, uv1: texture coordinates (clamped to [0,1])
        //   - widgetOffset: pixel offset within the display area for the image widget
        //   - widgetSize: size of the image widget in display pixels
        struct UvFromZoomPanResult
        {
            Point2d uv0, uv1;         // Texture coordinates
            Point2d widgetOffset;      // Offset within display area
            Size2d  widgetSize;        // Size of the image widget
        };
        UvFromZoomPanResult UvFromZoomPan(
            const MatrixType& zoomPanMatrix,
            Size imageSize,
            Size displaySize);

} // namespace ZoomPanTransform

    Matrix33d MakeZoomPanMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize);

}
