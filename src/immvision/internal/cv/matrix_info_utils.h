#pragma once
#include <opencv2/core.hpp>
#include <string>

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const cv::Mat& m);
        std::string _MatInfo(const cv::Mat &m);
        std::vector<double> MatValuesAt(const cv::Mat& m, int x, int y);
        std::string MatPixelColorInfo(const cv::Mat & m, int x, int y, char separator = ',', bool add_paren = true);

    } // namespace MatrixInfoUtils

} // namespace ImmVision