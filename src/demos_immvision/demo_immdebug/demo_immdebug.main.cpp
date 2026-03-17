#include "immdebug/immdebug.h"
#include "immvision/image.h"

#include <vector>
#include <cstdint>
#include <cstring>
#include <filesystem>

std::string ResourcesDir()
{
    std::filesystem::path this_file(__FILE__);
    return (this_file.parent_path().parent_path() / "resources").string();
}


void ExampleImageProcessingWithDebug()
{
    // Load image using ImmVision::ImRead (returns RGB)
    ImmVision::ImageBuffer image = ImmVision::ImRead(ResourcesDir() + "/house.jpg");
    ImmVision::ImmDebug(image, "original");
    ImmVision::ImmDebugBgr(image, "original as BGR");

    // Extract a sub-region
    ImmVision::ImageBuffer roi = image.subImage({400, 300, 200, 200});
    ImmVision::ImmDebug(roi, "roi");

    // Manual grayscale conversion (luminance formula)
    ImmVision::ImageBuffer gray = ImmVision::ImageBuffer::Zeros(
        roi.width, roi.height, 1, ImmVision::ImageDepth::uint8);
    for (int y = 0; y < roi.height; y++)
    {
        const uint8_t* src = roi.ptr<uint8_t>(y);
        uint8_t* dst = gray.ptr<uint8_t>(y);
        for (int x = 0; x < roi.width; x++)
        {
            int r = src[x * 3 + 0];
            int g = src[x * 3 + 1];
            int b = src[x * 3 + 2];
            dst[x] = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);
        }
    }
    ImmVision::ImmDebug(gray, "grayscale");
}


int main()
{
    ExampleImageProcessingWithDebug();
}
