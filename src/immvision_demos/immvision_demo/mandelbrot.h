#pragma once
#include <opencv2/core.hpp>

struct MandelbrotOptions
{
    const unsigned int MaxIteration = 142;
    cv::Size Size = cv::Size(800, 600);
    cv::Point2d StartPoint = cv::Point2d(-0.75, 0.0);
    double Zoom = 0.8;
};

cv::Mat_<cv::Vec3b> MakeMandelbrotImage(const MandelbrotOptions& options);
