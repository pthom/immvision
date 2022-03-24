#include "immvision_simple_runner/immvision_simple_runner.h"
#include "mandelbrot.h"
#include "immvision/image.h"

#include <memory>

struct AppState
{
    AppState() { updateImage(); }
    void updateImage()
    {
        mMat = MakeMandelbrotImage(mMandelbrotOptions);
    }

    MandelbrotOptions      mMandelbrotOptions = MandelbrotOptions();
    cv::Mat mMat;
    int mDisplayWidth = 0, mDisplayHeight = 0;
    bool mShow = true;
};

AppState gAppState;

namespace ImGuiExt
{
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, double power)
    {
        float vf = *v;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, format, (float)power);
        if (changed)
            *v = vf;
        return changed;
    }
}

void guiFunction()
{
    bool changed  = false;
    ImGui::SetNextItemWidth(200.f);
    if (ImGuiExt::SliderDouble(
        "x",
        &gAppState.mMandelbrotOptions.StartPoint.x,
        -2.,
        2.,
        "%.3lf",
        1
    ))
        changed = true;
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    if (ImGuiExt::SliderDouble(
        "y",
        &gAppState.mMandelbrotOptions.StartPoint.y,
        -2.,
        2.,
        "%.3lf",
        1
    ))
        changed = true;
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    if (ImGuiExt::SliderDouble(
        "zoom",
        &gAppState.mMandelbrotOptions.Zoom,
        0.001,
        10.,
        "%.3lf",
        1
    ))
        changed = true;

    ImGui::SetNextItemWidth(200.f);
    ImGui::SliderInt("Display Width", &gAppState.mDisplayWidth, 0, 1000);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    ImGui::SliderInt("Display Height", &gAppState.mDisplayHeight, 0, 1000);

    ImGui::Checkbox("Show", &gAppState.mShow);

    if (changed)
        gAppState.updateImage();
    if (gAppState.mShow)
    {
        ImmVision::Image(gAppState.mMat, changed, cv::Size(gAppState.mDisplayWidth, gAppState.mDisplayHeight));
        auto pos = ImmVision::GetImageMousePos();
        ImGui::Text("mouse %.1f %.1f hovered:%i", pos.x, pos.y, (int)ImGui::IsItemHovered());
    }

    static cv::Mat red(cv::Size(100, 100), CV_8UC3);
    ImmVision::Image(red, false);

    ImGui::ShowDemoWindow(NULL);
}

int main()
{
    auto appParams = ImmVisionSimpleRunner::AppParams();

    ImmVisionSimpleRunner::Run(guiFunction, appParams);
}