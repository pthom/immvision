#pragma once

#include <opencv2/core.hpp>

namespace ImmVision
{
    namespace ZoomPanTransform
    {
        using MatrixType = cv::Matx33d;

        MatrixType Identity();

        MatrixType ComputeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio);
        MatrixType ComputePanMatrix(const cv::Point2d& dragDelta, double currentZoom);
        MatrixType MakeScaleOne(cv::Size imageSize, cv::Size viewportSize);
        MatrixType MakeFullView(cv::Size imageSize, cv::Size viewportSize);
        cv::Matx33d MakeZoomMatrix(const cv::Point2d & zoomCenter, double zoomRatio,const cv::Size displayedImageSize);

        bool IsEqual(const MatrixType & v1, const MatrixType & v2);

        cv::Point2d Apply(const MatrixType& zoomMatrix, const cv::Point2d &p);

        cv::Matx23d ZoomMatrixToM23(const cv::Matx33d &m);

        MatrixType UpdateZoomMatrix_DisplaySizeChanged(
            const MatrixType& oldZoomMatrix,
            const cv::Size& oldDisplaySize, const cv::Size& newDisplaySize);

        cv::Rect VisibleRoi(const MatrixType & zoomMatrix,
                            cv::Size imageDisplaySize,
                            cv::Size originalImageSize
                            );

    } // namespace ZoomPanTransform

    cv::Matx33d MakeZoomPanMatrix(const cv::Point2d & zoomCenter, double zoomRatio, const cv::Size displayedImageSize);

}
