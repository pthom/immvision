#include "immvision/internal/cv/color_adjustment_utils.h"

namespace ImmVision
{
    namespace ColorAdjustmentsUtils
    {
        bool IsNone(const ColorAdjustmentsValues& a)
        {
            return (a.Factor == 1.) && (a.Delta == 0.);
        }

        bool IsEqual(const ColorAdjustmentsValues& v1, const ColorAdjustmentsValues& v2)
        {
            if (fabs(v2.Factor - v1.Factor) > 1E-6)
                return false;
            if (fabs(v2.Delta - v1.Delta) > 1E-6)
                return false;
            if (v1.Colormap != v2.Colormap)
                return false;
            return true;
        }

        cv::Mat Adjust(const ColorAdjustmentsValues& a, const cv::Mat &image)
        {
            if (IsNone(a))
                return image;
            else return
                    image * a.Factor + a.Delta;
        }

        ColorAdjustmentsValues ComputeInitialImageAdjustments(const cv::Mat& m)
        {
            ColorAdjustmentsValues r;
            if ((m.depth() == CV_32F) || (m.depth() == CV_64F))
            {
                std::vector<double> minima, maxima;
                std::vector<cv::Mat> channels;
                cv::split(m, channels);
                for (const cv::Mat&channel: channels)
                {
                    double min, max;
                    cv::minMaxLoc(channel, &min, &max);
                    minima.push_back(min);
                    maxima.push_back(max);
                }

                double min = *std::min_element(minima.begin(), minima.end());
                double max = *std::max_element(maxima.begin(), maxima.end());
                r.Factor = 1. / (max - min);
                r.Delta = -min * r.Factor;
            }
            return r;
        }

    } // namespace ImageAdjustmentsUtils

} // namespace ImmVision