#include "immdebug/immdebug.h"

#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <filesystem>
std::string ResourcesDir()
{
    std::filesystem::path this_file(__FILE__);
    return (this_file.parent_path().parent_path() / "resources").string();
}


void ExampleImageProcessingWithDebug()
{
    cv::Mat image = cv::imread(ResourcesDir() + "/house.jpg");
    ImmVision::ImmDebug(image, "original");

    cv::Mat roi = image(cv::Rect(800, 600, 400, 400));
    ImmVision::ImmDebug(roi, "roi");

    cv::Mat black;
    cv::cvtColor(roi, black, cv::COLOR_RGB2GRAY);
    ImmVision::ImmDebug(black, "black");

    cv::Mat blur;
    cv::blur(black, blur, cv::Size(5, 5));
    ImmVision::ImmDebug(blur, "blur");

    cv::Mat canny;
    cv::Canny(blur, canny, 80., 120.);
    ImmVision::ImmDebug(canny, "canny");

    cv::Mat floatImage;
    canny.convertTo(floatImage, CV_64FC1);
    ImmVision::ImmDebug(floatImage, "floatImage");

}



int main()
{
    ExampleImageProcessingWithDebug();
}
