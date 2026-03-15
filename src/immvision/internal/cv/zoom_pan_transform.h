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

} // namespace ZoomPanTransform

    Matrix33d MakeZoomPanMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize);

}
