#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/misc/math_utils.h"

#include <cassert>
#include <cmath>

namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = Matrix33d;

        MatrixType Identity()
        {
            return Matrix33d::eye();
        }

        MatrixType ComputeZoomMatrix(const Point2d & zoomCenter, double zoomRatio)
        {
            auto mat = Matrix33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            mat(0, 2) = zoomCenter.x * (1. - zoomRatio);
            mat(1, 2) = zoomCenter.y * (1. - zoomRatio);
            return mat;
        }

        MatrixType ComputePanMatrix(const Point2d& dragDelta, double currentZoom)
        {
            auto mat = Matrix33d::eye();
            mat(0, 2) = (double)dragDelta.x / currentZoom;
            mat(1, 2) = (double)dragDelta.y / currentZoom;
            return mat;
        }

        MatrixType MakeScaleOne(Size imageSize, Size viewportSize)
        {
            MatrixType r = Identity();
            r(0, 2) = (viewportSize.width / 2 - imageSize.width / 2);
            r(1, 2) = (viewportSize.height / 2 - imageSize.height / 2);
            return r;
        }

        MatrixType MakeFullView(Size imageSize, Size viewportSize)
        {
            assert(imageSize.area() > 0);
            assert(viewportSize.area() >0);

            MatrixType r = Identity();

            double zoom;
            {
                double k_image = (double)imageSize.width / (double)imageSize.height;
                double k_viewport = (double)viewportSize.width / (double)viewportSize.height;
                if (k_image > k_viewport)
                    zoom = (double)viewportSize.width / (double)imageSize.width;
                else
                    zoom = (double)viewportSize.height / (double)imageSize.height;
            }

            r(0, 0) = zoom;
            r(1, 1) = zoom;

            return r;
        }

        bool IsEqual(const MatrixType & v1, const MatrixType & v2)
        {
            for (int j = 0; j < 3; j++)
                for (int i = 0; i < 3; i++)
                    if (fabs(v2(j, i) - v1(j, i)) > 1E-6)
                        return false;
            return true;
        }

        Point2d Apply(const MatrixType& zoomMatrix, const Point2d &p)
        {
            return zoomMatrix.apply(p);
        }

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const Size& oldDisplaySize, const Size& newDisplaySize)
        {
            if (oldDisplaySize.area() == 0 || newDisplaySize.area() == 0)
                return oldZoomMatrix;

            MatrixType zoomMatrix;

            auto fnImageCenter = [](const Size s) {
                return Point2d((double)s.width / 2., (double)s.height / 2.);
            };

            double newZoomFactor;
            {
                double oldZoomFactor = oldZoomMatrix(0, 0);
                double kx = (double)newDisplaySize.width / (double)oldDisplaySize.width;
                double ky = (double)newDisplaySize.height / (double)oldDisplaySize.height;
                double k = (kx + ky) / 2.;
                newZoomFactor = oldZoomFactor * k;
            }

            zoomMatrix = MatrixType::eye();
            zoomMatrix(0, 0) = zoomMatrix(1, 1) = newZoomFactor;

            Point2d translation;
            {
                Point2d oldDisplayCenter_Zoomed = fnImageCenter(oldDisplaySize);
                Point2d oldDisplayCenter_Image = ZoomPanTransform::Apply(oldZoomMatrix.inv(), oldDisplayCenter_Zoomed);

                Point2d newDisplayCenter_Zoomed_Wanted = fnImageCenter(newDisplaySize);
                Point2d newDisplayCenter_Zoomed_Now = ZoomPanTransform::Apply(zoomMatrix, oldDisplayCenter_Image);
                translation = newDisplayCenter_Zoomed_Wanted - newDisplayCenter_Zoomed_Now;
            }

            zoomMatrix(0, 2) = translation.x;
            zoomMatrix(1, 2) = translation.y;

            return zoomMatrix;
        }

        Matrix33d MakeZoomMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize)
        {
            auto mat = Matrix33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            double dx = (double)displayedImageSize.width / 2. - zoomRatio * zoomCenter.x;
            double dy = (double)displayedImageSize.height / 2. - zoomRatio * zoomCenter.y;
            mat(0, 2) = dx;
            mat(1, 2) = dy;
            return mat;
        }

        Rect VisibleRoi(const MatrixType & zoomMatrix, Size imageDisplaySize, Size originalImageSize)
        {
            Rect roi;
            {
                Point2d tl = ZoomPanTransform::Apply(zoomMatrix.inv(), Point2d(0., 0.));
                Point tli(MathUtils::RoundInt(tl.x), MathUtils::RoundInt(tl.y));
                tli.x = std::clamp(tli.x, 0, originalImageSize.width - 1);
                tli.y = std::clamp(tli.y, 0, originalImageSize.height - 1);

                Point2d br = ZoomPanTransform::Apply(zoomMatrix.inv(), Point2d(
                    (double)imageDisplaySize.width, (double)imageDisplaySize.height));
                Point bri(MathUtils::RoundInt(br.x), MathUtils::RoundInt(br.y));
                bri.x = std::clamp(bri.x, 0, originalImageSize.width);
                bri.y = std::clamp(bri.y, 0, originalImageSize.height);

                roi = Rect(tli, bri);
            }
            return roi;
        }

        UvFromZoomPanResult UvFromZoomPan(
            const MatrixType& zoomPanMatrix,
            Size imageSize,
            Size displaySize)
        {
            double scale = zoomPanMatrix(0, 0);
            double tx    = zoomPanMatrix(0, 2);
            double ty    = zoomPanMatrix(1, 2);
            double imgW  = (double)imageSize.width;
            double imgH  = (double)imageSize.height;
            double dispW = (double)displaySize.width;
            double dispH = (double)displaySize.height;

            // Unclamped UVs: what portion of the image is visible in the display area
            // display_pixel = scale * image_pixel + t
            // image_pixel = (display_pixel - t) / scale
            // uv = image_pixel / image_size
            double uv0x = -tx / (scale * imgW);
            double uv0y = -ty / (scale * imgH);
            double uv1x = (dispW - tx) / (scale * imgW);
            double uv1y = (dispH - ty) / (scale * imgH);

            double uvRangeX = uv1x - uv0x;
            double uvRangeY = uv1y - uv0y;

            // Clamp UVs to [0,1] and compute widget offset/size
            double offsetX = 0., offsetY = 0.;
            double widgetW = dispW, widgetH = dispH;

            if (uv0x < 0.) { offsetX = (-uv0x / uvRangeX) * dispW; uv0x = 0.; }
            if (uv0y < 0.) { offsetY = (-uv0y / uvRangeY) * dispH; uv0y = 0.; }
            if (uv1x > 1.) { uv1x = 1.; }
            if (uv1y > 1.) { uv1y = 1.; }

            widgetW = (uv1x - uv0x) / uvRangeX * dispW;
            widgetH = (uv1y - uv0y) / uvRangeY * dispH;

            UvFromZoomPanResult result;
            result.uv0 = Point2d(uv0x, uv0y);
            result.uv1 = Point2d(uv1x, uv1y);
            result.widgetOffset = Point2d(offsetX, offsetY);
            result.widgetSize = Size2d(widgetW, widgetH);
            return result;
        }

    } // namespace ZoomPanTransform

    Matrix33d MakeZoomPanMatrix(const Point2d & zoomCenter, double zoomRatio, const Size displayedImageSize)
    {
        return ZoomPanTransform::MakeZoomMatrix(zoomCenter, zoomRatio, displayedImageSize);
    }

    Matrix33d MakeZoomPanMatrix_ScaleOne(
        Size imageSize,
        const Size displayedImageSize)
    {
        return ZoomPanTransform::MakeScaleOne(imageSize, displayedImageSize);
    }

    Matrix33d MakeZoomPanMatrix_FullView(
        Size imageSize,
        const Size displayedImageSize)
    {
        return ZoomPanTransform::MakeFullView(imageSize, displayedImageSize);
    }
}
