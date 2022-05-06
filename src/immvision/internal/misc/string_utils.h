#pragma once
#include <opencv2/core.hpp>
#include <string>

namespace ImmVision
{
    namespace StringUtils
    {
        std::string LowerString(const std::string& s);
        std::string JoinStrings(const std::vector<std::string>&v, const std::string& separator);
        std::vector<std::string> SplitString(const std::string& s, const char separator);
        std::string IndentLine(const std::string& s, int indentSize);
        std::string IndentLines(const std::string& s, int indentSize);


        std::string ToString(const std::string& s);
        std::string ToString(const double& v);
        std::string ToString(const float& v);
        std::string ToString(const int& v);
        std::string ToString(bool v);

        template<typename _Tp>
        std::string ToString(const cv::Point_<_Tp>& v)
        {
            return std::string("(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        }
        template<typename _Tp>
        std::string ToString(const cv::Size_<_Tp>& v)
        {
            return std::string("(") + std::to_string(v.width) + " x " + std::to_string(v.height) + ")";
        }

        template<typename _Tp>
        std::string ToString(const std::vector<_Tp>& v)
        {
            std::vector<std::string> strs;
            for (const auto& x : v)
                strs.push_back(ToString(x));
            std::string r = "[" + JoinStrings(strs, ", ") + "]";
            return r;
        }

        template<typename _Tp, int _rows,int _cols>
        std::string ToString(const cv::Matx<_Tp, _rows, _cols>& m)
        {
            std::vector<std::string> lines;
            for (int i = 0; i < _rows; ++i)
            {
                std::vector<_Tp> lineValues;
                for (int j = 0; j < _cols; ++j)
                    lineValues.push_back(m(i, j));

                std::string lineString = ToString(lineValues);
                if (i != 0)
                    lineString = std::string("   ") + lineString;
                lines.push_back(lineString);
            }
            std::string r = "\n  [";
            r += JoinStrings(lines, ",\n");
            r += "]";
            return r;
        }

    } // namespace StringUtils
} // namespace ImmVision