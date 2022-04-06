#include "immvision/image_navigator.h"
#include "hello_imgui/hello_imgui.h"
#include "datestr.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

 int main(int , char *[])
{
    std::cout << datestr << std::endl;

    std::string zoomKey = "zk";

    auto image = cv::imread(HelloImGui::assetFileFullPath("house.jpg"));
    ImmVision::Inspector_AddImage(image, "Original", zoomKey);

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    ImmVision::Inspector_AddImage(gray, "Gray", zoomKey, "colkey");

    cv::Mat blur;
    cv::GaussianBlur(gray, blur, cv::Size(), 7.);
    ImmVision::Inspector_AddImage(blur, "Blur", zoomKey, "colkey");

    cv::Mat place = cv::imread(HelloImGui::assetFileFullPath("reddit_place_2022.png"));
    cv::resize(place, place, cv::Size(2000, 2000));
    ImmVision::Inspector_AddImage(place, "Place");

    auto gui = []()
    {
        ImGui::Text("%s FPS:%.1f", datestr, ImGui::GetIO().Framerate);
        ImmVision::Inspector_Show();
    };

    HelloImGui::RunnerParams params;
    params.appWindowParams.windowSize = {1200.f, 800.f};
    params.callbacks.ShowGui = gui;
    params.callbacks.LoadAdditionalFonts = []{};
    HelloImGui::Run(params);

    return 0;
}
