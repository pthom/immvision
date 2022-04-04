#include "immdebug/immdebug.h"
#include "immdebug/immdebug_internal.h"

#include <iostream>

int main()
{
    cv::Mat m(cv::Size(123, 456), CV_8UC3);
    ImmVision::ImmDebug(
        m,
        "Hello",
        cv::Point2d(12.3, 45.6),
        7.89,
        "zoomKey",
        "colorAdjustmentsKey",
        true
        );

    auto v = ImmVision::ImmDebug_Internal::ReadImagePayload();
    if (v)
    {
        std::cout << "Found one payload size = " << v->Image.size() << std::endl;
    }
}
