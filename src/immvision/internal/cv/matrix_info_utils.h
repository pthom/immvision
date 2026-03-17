#pragma once
#include "immvision/immvision_types.h"
#include <string>

namespace ImmVision
{
    namespace MatrixInfoUtils
    {
        std::string _MatTypeName(const ImageBuffer& m);
        std::string _MatInfo(const ImageBuffer &m);
        std::vector<double> MatValuesAt(const ImageBuffer& m, int x, int y);
        std::string MatPixelColorInfo(const ImageBuffer & m, int x, int y, char separator = ',', bool add_paren = true);

    } // namespace MatrixInfoUtils

} // namespace ImmVision
