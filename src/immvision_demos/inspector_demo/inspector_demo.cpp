#include "immvision/image_navigator.h"
#include "hello_imgui/hello_imgui.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


int main()
{
    std::string zoomKey = "zk";
    auto image = cv::imread("resources/house.jpg");
    //ImmVision::AddInspectedImage(image, "Original");
    //ImmVision::Explorer_AddImage(image, "Original");
    //ImmVision::Inspector_AddImage(image, "Original");
    ImmVision::Inspector_AddImage(image, "Original", zoomKey);

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    ImmVision::Inspector_AddImage(gray, "Gray", zoomKey, "colkey");

    cv::Mat blur;
    cv::GaussianBlur(gray, blur, cv::Size(), 7.);
    ImmVision::Inspector_AddImage(blur, "Blur", zoomKey, "colkey");

    auto forcePowerSave = []{
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_EnablePowerSavingMode;
    };

    auto gui = []()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImVec2 viewportSize = viewport->Size;
        ImGui::SetNextWindowSize(viewportSize);

        bool open = true;
        ImmVision::Inspector_ShowWindow(&open);
    };

    HelloImGui::RunnerParams params;
    params.appWindowParams.windowSize = {1200.f, 800.f};
    params.callbacks.ShowGui = gui;
    params.callbacks.PostInit = forcePowerSave;
    HelloImGui::Run(params);

    //No: ImmVision::ShowInspectorWindow();
    //ImmVision::Inspector_Run();
    //ImmVisionSimpleRunner::Run(Gui);
}

