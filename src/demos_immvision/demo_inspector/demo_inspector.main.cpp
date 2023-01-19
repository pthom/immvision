#include "immvision/immvision.h"
#include "hello_imgui/hello_imgui.h"
#include "datestr.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


#include <filesystem>
std::string ResourcesDir()
{
    std::filesystem::path this_file(__FILE__);
    return (this_file.parent_path().parent_path() / "resources").string();
}


void FillInspector()
{
    std::string zoomKey = "zk";
    auto image = cv::imread(ResourcesDir() + "/house.jpg");
     ImmVision::Inspector_AddImage(image, "Original", zoomKey);

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    ImmVision::Inspector_AddImage(gray, "Gray", zoomKey);

    cv::Mat blur;
    cv::GaussianBlur(gray, blur, cv::Size(), 7.);
    ImmVision::Inspector_AddImage(blur, "Blur", zoomKey);

    cv::Mat floatMat;
    blur.convertTo(floatMat, CV_64FC1);
    floatMat = floatMat / 255.f;
    ImmVision::Inspector_AddImage(floatMat, "FloatMat", zoomKey);

    cv::Mat place = cv::imread(ResourcesDir() + "/reddit_place_2022.png");
    cv::resize(place, place, cv::Size(2000, 2000));
    ImmVision::Inspector_AddImage(place, "Place", "", "", cv::Point2d(130., 1617.), 1.5);
}


void gui()
{
    ImmVision::Inspector_Show();

    static bool inited = false;
    if (!inited)
    {
        FillInspector();
        inited = true;
    }
}

int main(int , char *[])
{
    HelloImGui::Run(gui);
    return 0;
}
