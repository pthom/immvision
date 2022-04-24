#include "immvision/internal/zoom_pan_transform.h"

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

    } // namespace ZoomPanTransform

    cv::Matx33d MakeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio,const cv::Size displayedImageSize)
    {
        return ZoomPanTransform::MakeZoomMatrix(zoomCenter, zoomRatio, displayedImageSize);
    }

}
