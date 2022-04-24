#include "immvision/internal/cv/matrix_info_utils.h"

#include <map>

#ifndef CV_16F // for old versions of OpenCV
#define CV_16F 7
#endif

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const cv::Mat& m)
        {
            std::map<int, std::string> depthNames
                {
                    { CV_8U, "CV_8U" },
                    { CV_8S, "CV_8S" },
                    { CV_16U, "CV_16U" },
                    { CV_16S, "CV_16S" },
                    { CV_32S, "CV_32S"},
                    { CV_32F, "CV_32F"},
                    { CV_64F, "CV_64F"},
                    { CV_16F, "CV_16F"}
                };
            return depthNames.at(m.depth()) + "C" + std::to_string(m.channels());
        }

        std::string _MatInfo(const cv::Mat &m)
        {
            return _MatTypeName(m) + " " + std::to_string(m.cols) + "x" + std::to_string(m.rows);
        }

        std::string JoinStrings(const std::vector<std::string>&v, char separator)
        {
            std::string r;
            for (size_t i = 0; i < v.size(); ++ i)
            {
                r += v[i];
                if (i < v.size() - 1)
                    r += separator;
            }
            return r;
        }

        template<typename _Tp>
        std::vector<double> GrabValuesFromBuffer(const uchar * buffer, int nb)
        {
            std::vector<double> r;
            auto buffer_typed =  reinterpret_cast<const _Tp *>(buffer);
            for (int i = 0; i < nb; ++i)
            {
                r.push_back(static_cast<double>(*buffer_typed));
                ++buffer_typed;
            }
            return r;
        }

        std::vector<double> MatValuesAt(const cv::Mat& m, int x, int y)
        {
            int depth = m.depth();
            int nb_channels = m.channels();
            const uchar * ptr = m.ptr(y, x);
            if (depth == CV_8U)
                return GrabValuesFromBuffer<uchar>(ptr, nb_channels);
            else if (depth == CV_8S)
                return GrabValuesFromBuffer<uchar>(ptr, nb_channels);
            else if (depth == CV_16U)
                return GrabValuesFromBuffer<uint16_t>(ptr, nb_channels);
            else if (depth == CV_16S)
                return GrabValuesFromBuffer<int16_t>(ptr, nb_channels);
#if CV_MAJOR_VERSION >= 4
                else if (depth == CV_16F)
                return GrabValuesFromBuffer<cv::float16_t>(ptr, nb_channels);
#endif
            else if (depth == CV_32S)
                return GrabValuesFromBuffer<int32_t>(ptr, nb_channels);
            else if (depth == CV_32F)
                return GrabValuesFromBuffer<float>(ptr, nb_channels);
            else if (depth == CV_64F)
                return GrabValuesFromBuffer<double>(ptr, nb_channels);
            else
                throw std::runtime_error("MatValuesAt: unhandled depth");
        }

        std::string MatPixelColorInfo(const cv::Mat & m, int x, int y, char separator, bool add_paren)
        {
            if (!cv::Rect(cv::Point(0, 0), m.size()).contains(cv::Point(x, y)))
                return "";
            std::vector<double> values = MatValuesAt(m, x, y);

            auto formatValue = [](double v, int depth) -> std::string
            {
                bool isFloat = false;
                if ((depth == CV_32F) || (depth == CV_64F))
                    isFloat = true;
#if CV_MAJOR_VERSION >= 4
                if (depth == CV_16F)
                    isFloat = true;
#endif
                if (isFloat)
                {
                    char buffer_color[300];
                    snprintf(buffer_color, 300, "%.5G", (double) v);
                    return std::string(buffer_color);
                }
                else
                {
                    char buffer_color[300];
                    snprintf(buffer_color, 300, "%lld", (long long) v);
                    return std::string(buffer_color);
                }
            };

            std::vector<std::string> strs;
            int depth = m.depth();
            for (double v: values)
                strs.push_back(formatValue(v, depth));

            std::string r = JoinStrings(strs, separator);
            if (add_paren)
                r = std::string("(") + r + ")";
            return r;
        }

    } // namespace MatrixInfoUtils

} // namespace ImmVision