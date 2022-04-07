#include "immvision_simple_runner/immvision_simple_runner.h"
#include "mandelbrot.h"
#include "immvision/image.h"
#include "immvision/image_navigator.h"
#include "immvision/internal/imgui_imm.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <array>

namespace ImGuiExt
{
    bool SliderDoubleLogarithmic(const char* label, double* v, double v_min, double v_max)
    {
        float vf = (float)*v;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, "%.3f", ImGuiSliderFlags_Logarithmic);
        if (changed)
            *v = (double)vf;
        return changed;
    }
}


struct SobelParamsValues
{
    double GaussianBlurSize = 1.25;
    int  	DerivativeOrder = 1; // order of the derivative
    int  	KSize = 5; // size of the extended Sobel kernel; it must be 1, 3, 5, or 7 (or -1 for Scharr)
};

cv::Mat ComputeSobelDerivatives(const cv::Mat&image, const SobelParamsValues& params)
{
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::Mat img_float;
    gray.convertTo(img_float, CV_32FC1);
    img_float = img_float / 255.;

    cv::Mat blurred;
    cv::GaussianBlur(img_float, blurred, cv::Size(), params.GaussianBlurSize, params.GaussianBlurSize);

    std::vector<cv::Mat> r(3);
    int ddepth = CV_64F;

    double good_scale = 1. / pow(2., (double)(params.KSize - 2 * params.DerivativeOrder - 2));
    //params.Scale = good_scale;
    cv::Sobel(blurred, r[0], ddepth, params.DerivativeOrder, 0, params.KSize, good_scale);
    cv::Sobel(blurred, r[1], ddepth, 0, params.DerivativeOrder, params.KSize, good_scale);
    r[2] = (cv::abs(r[0]) + cv::abs(r[1])) / 2.;

    cv::Mat m2;
    cv::merge(r, m2);
    return m2;
}

bool GuiSobelParams(SobelParamsValues& params)
{
    ImGuiImm::BeginGroupPanel("Sobel Params");
    ImGui::BeginTable("Sobel Params", 2, ImGuiTableFlags_SizingFixedFit);
    bool changed = false;
    // ImGui::Columns()

    ImGui::TableNextRow(); ImGui::TableNextColumn();
    ImGui::Text("Blur size");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(200.f);
    changed |= ImGuiExt::SliderDoubleLogarithmic("##Blur Size", &params.GaussianBlurSize, 0.5, 50.);
    ImGui::TableNextRow(); ImGui::TableNextColumn();

    {
        ImGui::Text("Derivative Order");
        ImGui::TableNextColumn();
        changed |= ImGui::RadioButton("1##DerivOrder", &params.DerivativeOrder, 1); ImGui::SameLine();
        changed |= ImGui::RadioButton("2##DerivOrder", &params.DerivativeOrder, 2); ImGui::SameLine();
        changed |= ImGui::RadioButton("3##DerivOrder", &params.DerivativeOrder, 3); ImGui::SameLine();
        changed |= ImGui::RadioButton("4##DerivOrder", &params.DerivativeOrder, 4); ImGui::SameLine();
        changed |= ImGui::RadioButton("5##DerivOrder", &params.DerivativeOrder, 5); ImGui::SameLine();
        changed |= ImGui::RadioButton("6##DerivOrder", &params.DerivativeOrder, 6); ImGui::SameLine();
        ImGui::TableNextRow(); ImGui::TableNextColumn();
    }

    // KSize
    {
        ImGui::Text("K Size");
        ImGui::TableNextColumn();
        changed |= ImGui::RadioButton("1##KSize", &params.KSize, 1); ImGui::SameLine();
        changed |= ImGui::RadioButton("3##KSize", &params.KSize, 3); ImGui::SameLine();
        changed |= ImGui::RadioButton("5##KSize", &params.KSize, 5); ImGui::SameLine();
        changed |= ImGui::RadioButton("7##KSize", &params.KSize, 7); ImGui::SameLine();
        ImGui::TableNextRow(); ImGui::TableNextColumn();
    }

    if (params.KSize <= params.DerivativeOrder)
        params.KSize = 7;

    ImGui::EndTable();
    ImGuiImm::EndGroupPanel();
    return changed;
}

bool GuiDisplaySize(cv::Size& displaySize)
{
    ImGuiImm::BeginGroupPanel("Display size");
    bool changed = false;
    ImGui::SetNextItemWidth(70.f);
    changed |= ImGui::SliderInt("Width", &displaySize.width, 0, 1000);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(70.f);
    changed |= ImGui::SliderInt("Height", &displaySize.height, 0, 1000);
    ImGuiImm::EndGroupPanel();
    return changed;
}

struct AppState
{
    AppState() {}
    void LoadImages()
    {
        // Image = cv::imread("resources/tennis.jpg");
        // Image = cv::imread("resources/rainbow-lorikeet-1188535.jpg");
        // Image = cv::imread("resources/winter-in-holland-1396288.jpg");
        Image = cv::imread("resources/house.jpg");
        cv::cvtColor(Image, Image, cv::COLOR_BGR2RGB);
    }
    void UpdateImages()
    {
        ImageFiltered = ComputeSobelDerivatives(Image, SobelParams);
    }

    cv::Mat Image;
    cv::Mat ImageFiltered;
    cv::Size DisplaySize = cv::Size(0, 400);
    SobelParamsValues SobelParams;
};

static AppState gAppState;


static void guiFunction()
{
    if (gAppState.Image.empty())
    {
        gAppState.LoadImages();
        gAppState.UpdateImages();
    }

    bool changed  = false;

    ImGui::Text("FPS: %.1lf", (double)ImGui::GetIO().Framerate);

    changed |= GuiSobelParams(gAppState.SobelParams);
    ImGui::SameLine();
    changed |= GuiDisplaySize(gAppState.DisplaySize);

    {
        ImmVision::ImageNavigator(
            gAppState.Image,
            gAppState.DisplaySize,
            "Original",
            false, //bool refreshImage = false,
            true, //bool showOptionsWhenAppearing = false,
            "i", //const std::string& zoomKey = "",
            "" //const std::string& colorAdjustmentsKey = ""
        );

    }
    ImGui::SameLine();

    {
        static ImmVision::ImageNavigatorParams imageNavigatorParamsFilter;
        imageNavigatorParamsFilter.ImageDisplaySize = gAppState.DisplaySize;
        imageNavigatorParamsFilter.ZoomKey = "i";
        imageNavigatorParamsFilter.ColorAdjustmentsKey = "c";
        imageNavigatorParamsFilter.Legend = "X & Y Gradients (see channels 0 & 1)";

        ImmVision::ImageNavigator(
            gAppState.ImageFiltered,
            &imageNavigatorParamsFilter,
            changed);

        static bool zoomApplied = false;
        if (!zoomApplied)
        {
            imageNavigatorParamsFilter.ZoomMatrix = ImmVision::MakeZoomMatrix(
                cv::Point2d(1004., 953.), 40., imageNavigatorParamsFilter.ImageDisplaySize);

            zoomApplied = true;
        }

    }

    if (changed)
        gAppState.UpdateImages();


    {
        static cv::Mat imageTransparent = cv::imread("resources/bear_transparent.png", cv::IMREAD_UNCHANGED);
        ImmVision::ImageNavigator(imageTransparent, cv::Size(0, 400));
    }

//    ImGui::Begin("Style");
//    ImGui::ShowStyleEditor(nullptr);
//    ImGui::End();

    //ImGui::ShowMetricsWindow(NULL);
    //ImGui::ShowMetricsWindow(NULL);
    //ImGui::ShowDemoWindow(NULL);
}


#include "immvision/internal/internal_icons.h"
int main()
{
    auto appParams = ImmVisionSimpleRunner::AppParams();

    ImmVisionSimpleRunner::Run(guiFunction, appParams);
    //ImmVisionSimpleRunner::Run(ImmVision::Icons::DevelPlaygroundGui, appParams);
}

