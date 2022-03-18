#include "immvision_simple_runner/immvision_simple_runner.h"
#include "mandelbrot.h"
#include "immvision/image_cv.h"

struct AppState
{
    AppState() { updateImage(); }
    void updateImage()
    {
        _mat = MakeMandelbrotImage(_mandelbrotOptions);
        _imgcv = ImmVision::ImageCv(_mat);
    }
    ImmVision::ImageCv& getImageCv() { return _imgcv; }

    MandelbrotOptions _mandelbrotOptions = MandelbrotOptions();

private:
    cv::Mat _mat;
    ImmVision::ImageCv _imgcv;
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
    if (ImGuiExt::SliderDouble(
        "x",
        &gAppState._mandelbrotOptions.StartPoint.x,
        -2.,
        2.,
        "%.1lf",
        1
    ))
        changed = true;
    if (ImGuiExt::SliderDouble(
        "y",
        &gAppState._mandelbrotOptions.StartPoint.y,
        -2.,
        2.,
        "%.1lf",
        1
    ))
        changed = true;
    if (ImGuiExt::SliderDouble(
        "zoom",
        &gAppState._mandelbrotOptions.Zoom,
        0.001,
        10.,
        "%.1lf",
        1
    ))
        changed = true;

    gAppState.getImageCv().Draw();
    if (changed)
        gAppState.updateImage();

}

int main()
{
    auto appParams = ImmVisionSimpleRunner::AppParams();

    ImmVisionSimpleRunner::Run(guiFunction, appParams);
}