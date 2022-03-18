#include "immvision_simple_runner/immvision_simple_runner.h"
#include "mandelbrot.h"
#include "immvision/internal/gl_texture.h"

#include <memory>

struct AppState
{
    AppState() { updateImage(); }
    void updateImage()
    {
        cv::Mat mat = MakeMandelbrotImage(mMandelbrotOptions);
        mGlTextureCv.BlitMat(mat);
    }

    MandelbrotOptions      mMandelbrotOptions = MandelbrotOptions();
    ImmVision::GlTextureCv mGlTextureCv;
};

std::unique_ptr<AppState> gAppState;

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
    if (!gAppState)
        gAppState = std::make_unique<AppState>();

    bool changed  = false;
    if (ImGuiExt::SliderDouble(
        "x",
        &gAppState->mMandelbrotOptions.StartPoint.x,
        -2.,
        2.,
        "%.3lf",
        1
    ))
        changed = true;
    if (ImGuiExt::SliderDouble(
        "y",
        &gAppState->mMandelbrotOptions.StartPoint.y,
        -2.,
        2.,
        "%.3lf",
        1
    ))
        changed = true;
    if (ImGuiExt::SliderDouble(
        "zoom",
        &gAppState->mMandelbrotOptions.Zoom,
        0.001,
        10.,
        "%.3lf",
        1
    ))
        changed = true;

    gAppState->mGlTextureCv.Draw();
    if (changed)
        gAppState->updateImage();

}

int main()
{
    auto appParams = ImmVisionSimpleRunner::AppParams();

    ImmVisionSimpleRunner::Run(guiFunction, appParams);
}