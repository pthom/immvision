#include "immvision/internal/drawing/colormap.h"

#include "tinycolormap.hpp"

namespace ImmVision
{
    namespace Colormap
    {
        void foo()
        {
            // Define a target value. This value should be in [0, 1]; otherwise, it will be cropped to 0 or 1.
            const double value = 0.5;

            // Get the mapped color. Here, Viridis is specified as the colormap.
            const tinycolormap::Color color = tinycolormap::GetColor(value, tinycolormap::ColormapType::Viridis);
            (void)color;
        }



    } // namespace Colormap
} // namespace ImmVision