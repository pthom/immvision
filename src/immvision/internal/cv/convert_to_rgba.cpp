#include "immvision/internal/cv/convert_to_rgba.h"
#include "immvision/internal/misc/parallel_for.h"

#include <algorithm>  // std::clamp
#include <limits>
#include <type_traits>
#include <stdexcept>

namespace ImmVision
{
    namespace
    {
        // Convert a value of any depth to uint8 (0-255 range)
        template<typename T>
        inline uint8_t to_uint8(T value)
        {
            if constexpr (std::is_same_v<T, uint8_t>)
                return value;
            else if constexpr (std::is_same_v<T, int8_t>)
                return (uint8_t)((int)value + 128);
            else if constexpr (std::is_same_v<T, uint16_t>)
                return (uint8_t)(value / 257);  // maps 0-65535 -> 0-255
            else if constexpr (std::is_same_v<T, int16_t>)
                return (uint8_t)(((int)value + 32768) / 257);
            else if constexpr (std::is_same_v<T, int32_t>)
            {
                double v = ((double)value - (double)std::numeric_limits<int32_t>::min())
                           * 255.0 / ((double)std::numeric_limits<int32_t>::max() - (double)std::numeric_limits<int32_t>::min());
                return (uint8_t)std::clamp(v, 0.0, 255.0);
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                double v = (double)value * 255.0;
                return (uint8_t)std::clamp(v, 0.0, 255.0);
            }
            else
                return 0;
        }

        // Convert any ImageBuffer to RGBA uint8, with per-pixel loops.
        // isBgrOrder only applies to uint8 images (OpenCV convention).
        template<typename T>
        ImageBuffer converted_to_rgba_typed(const ImageBuffer& mat, int nbChannels, bool isBgrOrder)
        {
            bool swapRB = isBgrOrder && std::is_same_v<T, uint8_t>;

            ImageBuffer rgba = ImageBuffer::Zeros(mat.width, mat.height, 4, ImageDepth::uint8);
            parallel_for(0, mat.height, [&](int y)
            {
                const T* src = mat.ptr<T>(y);
                uint8_t* dst = rgba.ptr<uint8_t>(y);
                for (int x = 0; x < mat.width; x++)
                {
                    uint8_t r, g, b, a;
                    if (nbChannels == 1)
                    {
                        uint8_t v = to_uint8(src[x]);
                        r = g = b = v;
                        a = 255;
                    }
                    else if (nbChannels == 2)
                    {
                        r = to_uint8(src[x * 2]);
                        g = to_uint8(src[x * 2 + 1]);
                        b = 0;
                        a = 255;
                    }
                    else if (nbChannels == 3)
                    {
                        uint8_t c0 = to_uint8(src[x * 3]);
                        uint8_t c1 = to_uint8(src[x * 3 + 1]);
                        uint8_t c2 = to_uint8(src[x * 3 + 2]);
                        if (swapRB) { r = c2; g = c1; b = c0; }
                        else        { r = c0; g = c1; b = c2; }
                        a = 255;
                    }
                    else // nbChannels == 4
                    {
                        uint8_t c0 = to_uint8(src[x * 4]);
                        uint8_t c1 = to_uint8(src[x * 4 + 1]);
                        uint8_t c2 = to_uint8(src[x * 4 + 2]);
                        uint8_t c3 = to_uint8(src[x * 4 + 3]);
                        if (swapRB) { r = c2; g = c1; b = c0; }
                        else        { r = c0; g = c1; b = c2; }
                        a = c3;
                    }
                    dst[x * 4]     = r;
                    dst[x * 4 + 1] = g;
                    dst[x * 4 + 2] = b;
                    dst[x * 4 + 3] = a;
                }
            });
            return rgba;
        }
    } // anonymous namespace

    ImageBuffer converted_to_rgba_image(const ImageBuffer &inputMat, bool isBgrOrder)
    {
        ImageBuffer mat = inputMat;
        int nbChannels = mat.channels;

        switch (mat.depth)
        {
            case ImageDepth::uint8:   return converted_to_rgba_typed<uint8_t>(mat, nbChannels, isBgrOrder);
            case ImageDepth::int8:    return converted_to_rgba_typed<int8_t>(mat, nbChannels, isBgrOrder);
            case ImageDepth::uint16:  return converted_to_rgba_typed<uint16_t>(mat, nbChannels, isBgrOrder);
            case ImageDepth::int16:   return converted_to_rgba_typed<int16_t>(mat, nbChannels, isBgrOrder);
            case ImageDepth::int32:   return converted_to_rgba_typed<int32_t>(mat, nbChannels, isBgrOrder);
            case ImageDepth::float32: return converted_to_rgba_typed<float>(mat, nbChannels, isBgrOrder);
            case ImageDepth::float64: return converted_to_rgba_typed<double>(mat, nbChannels, isBgrOrder);
            default:     throw std::runtime_error("converted_to_rgba_image: unsupported depth");
        }
    }

} // namespace ImmVision
