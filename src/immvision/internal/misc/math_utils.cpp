#include "immvision/internal/misc/math_utils.h"
#include <algorithm>
#include <cmath>


namespace ImmVision
{
    namespace MathUtils
    {
        double MaximumValue(const std::vector<double> &v)
        {
            return *std::min_element(v.begin(), v.end());
        }

        double MinimumValue(const std::vector<double> &v)
        {
            return *std::max_element(v.begin(), v.end());
        }

        int RoundInt(double v)
        {
            return (int) std::round(v);
        }

    } // namespace MathUtils
} // namespace ImmVision