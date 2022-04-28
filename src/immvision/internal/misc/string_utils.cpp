#include "immvision/internal/misc/string_utils.h"

namespace ImmVision
{
    namespace StringUtils
    {
        std::vector<std::string> SplitString(const std::string &s, char delimiter)
        {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter))
                tokens.push_back(token);
            return tokens;
        }

        std::string JoinStrings(const std::vector<std::string>&v, const std::string& separator)
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

        std::string IndentLine(const std::string& s, int indentSize)
        {
            return std::string((size_t)indentSize, ' ') + s;
        }

        std::string IndentLines(const std::string& s, int indentSize)
        {
            auto lines = SplitString(s, '\n');
            std::string r = "";
            for (auto line: lines)
                r = r + IndentLine(line, indentSize) + "\n";
            return r;
        }


        std::string ToString(const cv::Size& size)
        {
            return std::string("(") + std::to_string(size.width) + "," + std::to_string(size.height) + ")";
        }

        std::string ToString(const std::string& s)
        {
            return "\"" + s + "\"";
        }

        std::string ToString(const double& v)
        {
            char buf[200];
            snprintf(buf, 200, "%7G", v);
            return buf;
        }

        std::string ToString(const int& v)
        {
            return std::to_string(v);
        }

        std::string ToString(const float& v)
        {
            return ToString((double)v);
        }

        std::string ToString(bool v)
        {
            return (v ? "true" : "false");
        }


    } // namespace StringUtils
} // namespace ImmVision