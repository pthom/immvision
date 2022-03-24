#include "immvision_simple_runner/immvision_simple_runner.h"
#include "mandelbrot.h"
#include "immvision/image.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

struct CannyParams
{
    double  	threshold1 = 1.;
    double  	threshold2 = 2.;
    int  	apertureSize = 3;
    bool  	L2gradient = false;
};

struct AppState
{
    AppState() {}
    void LoadImages()
    {
        mImage = cv::imread("resources/tennis.jpg");
    }
    void UpdateImages()
    {
        cv::Mat grey;
        cv::cvtColor(mImage, grey, cv::COLOR_BGR2GRAY);
        cv::Canny(grey, mImageFiltered,
            mCannyParams.threshold1, mCannyParams.threshold2,
            mCannyParams.apertureSize,
            mCannyParams.L2gradient
            );
        int  k = 6;
    }

    cv::Mat mImage, mImageFiltered;
    int mDisplayWidth = 0, mDisplayHeight = 0;
    CannyParams mCannyParams;
};

AppState gAppState;

namespace ImGuiExt
{
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format)
    {
        float vf = *v;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, format);
        if (changed)
            *v = vf;
        return changed;
    }
}

void guiFunction()
{
    if (gAppState.mImage.empty())
        gAppState.LoadImages();

    bool changed  = false;
    ImGui::SetNextItemWidth(200.f);
    if (ImGuiExt::SliderDouble("thr1", &gAppState.mCannyParams.threshold1, 0., 40550., "%.3lf"))
        changed = true;
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    if (ImGuiExt::SliderDouble("thr2", &gAppState.mCannyParams.threshold2, 0., 40550., "%.3lf"))
        changed = true;
    ImGui::Text("Aperture size"); ImGui::SameLine();
    if (ImGui::RadioButton("3", &gAppState.mCannyParams.apertureSize, 3))
        changed = true;
    ImGui::SameLine();
    if (ImGui::RadioButton("5", &gAppState.mCannyParams.apertureSize, 5))
        changed = true;
    ImGui::SameLine();
    if (ImGui::RadioButton("7", &gAppState.mCannyParams.apertureSize, 7))
        changed = true;
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    if (ImGui::Checkbox("L2gradient", &gAppState.mCannyParams.L2gradient))
        changed = true;

    ImGui::SetNextItemWidth(200.f);
    ImGui::SliderInt("Display Width", &gAppState.mDisplayWidth, 0, 1000);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    ImGui::SliderInt("Display Height", &gAppState.mDisplayHeight, 0, 1000);

    if (changed)
        gAppState.UpdateImages();
    ImGui::BeginGroup();
        ImmVision::Image(gAppState.mImage, changed, cv::Size(gAppState.mDisplayWidth, gAppState.mDisplayHeight));
        auto pos = ImmVision::GetImageMousePos();
        ImGui::Text("mouse %.1f %.1f hovered:%i", pos.x, pos.y, (int)ImGui::IsItemHovered());
    ImGui::EndGroup();
    ImGui::SameLine();
    ImmVision::Image(gAppState.mImageFiltered, changed, cv::Size(gAppState.mDisplayWidth, gAppState.mDisplayHeight));

    ImGui::ShowMetricsWindow(NULL);
    //ImGui::ShowDemoWindow(NULL);
}

int main()
{
    auto appParams = ImmVisionSimpleRunner::AppParams();

    ImmVisionSimpleRunner::Run(guiFunction, appParams);
}