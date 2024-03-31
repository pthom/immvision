#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/misc/math_utils.h"

#include <opencv2/imgproc.hpp>

namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = cv::Matx33d;

        MatrixType Identity()
        {
            return cv::Matx33d::eye();
        }

        MatrixType ComputeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            mat(0, 2) = zoomCenter.x * (1. - zoomRatio);
            mat(1, 2) = zoomCenter.y * (1. - zoomRatio);
            return mat;
        }

        MatrixType ComputePanMatrix(const cv::Point2d& dragDelta, double currentZoom)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 2) = (double)dragDelta.x / currentZoom;
            mat(1, 2) = (double)dragDelta.y / currentZoom;
            return mat;
        }

        MatrixType MakeScaleOne(cv::Size imageSize, cv::Size viewportSize)
        {
            MatrixType r = Identity();
            r(0, 2) = (viewportSize.width / 2 - imageSize.width / 2);
            r(1, 2) = (viewportSize.height / 2 - imageSize.height / 2);
            return r;
        }

        cv::Matx23d ZoomMatrixToM23(const cv::Matx33d &m)
        {
            cv::Matx23d r;
            for (int y = 0; y < 2; y++)
                for (int x = 0; x < 3; x++)
                    r(y, x) = m(y, x);
            return r;
        }

        MatrixType MakeFullView(cv::Size imageSize, cv::Size viewportSize)
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

        cv::Point2d Apply(const MatrixType& zoomMatrix, const cv::Point2d &p)
        {
            cv::Matx31d pMat(p.x, p.y, 1.);
            cv::Matx31d rMat = zoomMatrix * pMat;
            cv::Point2d r(rMat(0, 0), rMat(1, 0));
            return r;
        }

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const cv::Size& oldDisplaySize, const cv::Size& newDisplaySize)
        {
            if (oldDisplaySize.area() == 0 || newDisplaySize.area() == 0)
                return oldZoomMatrix;

            MatrixType zoomMatrix;

            auto fnImageCenter = [](const cv::Size s) {
                return cv::Point2d((double)s.width / 2., (double)s.height / 2.);
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

            cv::Point2d translation;
            {
                cv::Point2d oldDisplayCenter_Zoomed = fnImageCenter(oldDisplaySize);
                cv::Point2d oldDisplayCenter_Image = ZoomPanTransform::Apply(oldZoomMatrix.inv(), oldDisplayCenter_Zoomed);

                cv::Point2d newDisplayCenter_Zoomed_Wanted = fnImageCenter(newDisplaySize);
                cv::Point2d newDisplayCenter_Zoomed_Now = ZoomPanTransform::Apply(zoomMatrix, oldDisplayCenter_Image);
                translation = newDisplayCenter_Zoomed_Wanted - newDisplayCenter_Zoomed_Now;
            }

            zoomMatrix(0, 2) = translation.x;
            zoomMatrix(1, 2) = translation.y;

            return zoomMatrix;
        }

        cv::Matx33d MakeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio,const cv::Size displayedImageSize)
        {
            auto mat = cv::Matx33d::eye();
            mat(0, 0) = zoomRatio;
            mat(1, 1) = zoomRatio;
            double dx = (double)displayedImageSize.width / 2. - zoomRatio * zoomCenter.x;
            double dy = (double)displayedImageSize.height / 2. - zoomRatio * zoomCenter.y;
            mat(0, 2) = dx;
            mat(1, 2) = dy;
            return mat;
        }

        cv::Rect VisibleRoi(const MatrixType & zoomMatrix, cv::Size imageDisplaySize, cv::Size originalImageSize)
        {
            cv::Rect roi;
            {
                cv::Point2d tl = ZoomPanTransform::Apply(zoomMatrix.inv(), cv::Point2d(0., 0.));
                cv::Point tli(MathUtils::RoundInt(tl.x), MathUtils::RoundInt(tl.y));
                tli.x = std::clamp(tli.x, 0, originalImageSize.width - 1);
                tli.y = std::clamp(tli.y, 0, originalImageSize.height - 1);

                cv::Point2d br = ZoomPanTransform::Apply(zoomMatrix.inv(), cv::Point2d(
                    (double)imageDisplaySize.width, (double)imageDisplaySize.height));
                cv::Point bri(MathUtils::RoundInt(br.x), MathUtils::RoundInt(br.y));
                bri.x = std::clamp(bri.x, 0, originalImageSize.width);
                bri.y = std::clamp(bri.y, 0, originalImageSize.height);

                //                bri.x += 1;
//                bri.y += 1;
                roi = cv::Rect(tli, bri);
            }
            return roi;
        }

        // Custom version of cv::warpAffine for small sizes, since cv::warpAffine happily ignores cv::INTER_AREA
        // cf https://github.com/pthom/immvision/issues/6 and
        // cf https://github.com/opencv/opencv/blob/4.x/modules/imgproc/src/imgwarp.cpp#L2826-L2827
        void _WarpAffineInterAreaForSmallSizes(const cv::Mat& src, cv::Mat& dst, const cv::Matx33d& m)
        {
            // Since in our case, we are only dealing with transformations that do not modify
            // the orientation of the vertical arrow and horizontal axes, we take the easy route:
            // first resize the image and then place it at the correct location in the final image.


            // first, compute the resized image size by using the transformation matrix.
            cv::Point2d tl = ZoomPanTransform::Apply(m, cv::Point2d(0., 0.));
            cv::Point2d br = ZoomPanTransform::Apply(m, cv::Point2d((double)src.cols, (double)src.rows));
            cv::Size resizedSize(MathUtils::RoundInt(br.x - tl.x), MathUtils::RoundInt(br.y - tl.y));

            // then, resize the image
            cv::Mat resized;
            cv::resize(src, resized, resizedSize, 0, 0, cv::INTER_AREA);

            // then, place the resized image at the correct location in the final image.
            cv::Matx23d translation = cv::Matx23d::eye();
            translation(0, 2) = tl.x;
            translation(1, 2) = tl.y;

            // copy resized
            cv::warpAffine(resized, dst, translation, dst.size(), cv::INTER_NEAREST, cv::BORDER_TRANSPARENT);
        }


    } // namespace ZoomPanTransform

    cv::Matx33d MakeZoomPanMatrix(const cv::Point2d & zoomCenter, double zoomRatio, const cv::Size displayedImageSize)
    {
        return ZoomPanTransform::MakeZoomMatrix(zoomCenter, zoomRatio, displayedImageSize);
    }

    cv::Matx33d MakeZoomPanMatrix_ScaleOne(
        cv::Size imageSize,
        const cv::Size displayedImageSize)
    {
        return ZoomPanTransform::MakeScaleOne(imageSize, displayedImageSize);
    }

    cv::Matx33d MakeZoomPanMatrix_FullView(
        cv::Size imageSize,
        const cv::Size displayedImageSize)
    {
        return ZoomPanTransform::MakeFullView(imageSize, displayedImageSize);
    }
}
