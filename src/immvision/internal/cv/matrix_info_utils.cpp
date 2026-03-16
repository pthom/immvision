#include "immvision/internal/cv/matrix_info_utils.h"
#include <stdexcept>
#include <map>

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const ImageBuffer& m)
        {
            return ImageDepthName(m.depth);
        }

        std::string _MatInfo(const ImageBuffer &m)
        {
            return ImageDepthName(m.depth) + " " + std::to_string(m.width) + "x" + std::to_string(m.height);
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
        std::vector<double> GrabValuesFromBuffer(const uint8_t * buffer, int nb)
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

        std::vector<double> MatValuesAt(const ImageBuffer& m, int x, int y)
        {
            int nb_channels = m.channels;
            const uint8_t * ptr = m.ptr<uint8_t>(y) + x * m.elemSizeTotal();
            switch (m.depth)
            {
                case ImageDepth::uint8:
                    return GrabValuesFromBuffer<uint8_t>(ptr, nb_channels);
                case ImageDepth::int8:
                    return GrabValuesFromBuffer<int8_t>(ptr, nb_channels);
                case ImageDepth::uint16:
                    return GrabValuesFromBuffer<uint16_t>(ptr, nb_channels);
                case ImageDepth::int16:
                    return GrabValuesFromBuffer<int16_t>(ptr, nb_channels);
                case ImageDepth::int32:
                    return GrabValuesFromBuffer<int32_t>(ptr, nb_channels);
                case ImageDepth::float32:
                    return GrabValuesFromBuffer<float>(ptr, nb_channels);
                case ImageDepth::float64:
                    return GrabValuesFromBuffer<double>(ptr, nb_channels);
                default:
                    throw std::runtime_error("MatValuesAt: unhandled depth");
            }
        }

        std::string MatPixelColorInfo(const ImageBuffer & m, int x, int y, char separator, bool add_paren)
        {
            if (!Rect(Point(0, 0), m.size()).contains(Point(x, y)))
                return "";
            std::vector<double> values = MatValuesAt(m, x, y);

            auto formatValue = [](double v, ImageDepth depth) -> std::string
            {
                bool isFloat = ImageDepthIsFloat(depth);
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
            for (double v: values)
                strs.push_back(formatValue(v, m.depth));

            std::string r = JoinStrings(strs, separator);
            if (add_paren)
                r = std::string("(") + r + ")";
            return r;
        }

    } // namespace MatrixInfoUtils

} // namespace ImmVision
