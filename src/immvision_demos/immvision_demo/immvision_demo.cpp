#include "immvision_simple_runner/immvision_simple_runner.h"
#include "mandelbrot.h"
#include "immvision/image.h"
#include "immvision/image_navigator.h"
#include "immvision/internal/imgui_ext.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <array>

namespace ImGuiExt
{
    bool SliderDoubleLogarithmic(const char* label, double* v, double v_min, double v_max)
    {
        float vf = *v;
        bool changed = ImGui::SliderFloat(label, &vf, (float)v_min, (float)v_max, "%.3f", ImGuiSliderFlags_Logarithmic);
        if (changed)
            *v = vf;
        return changed;
    }
}


struct SobelParams
{
    double GaussianBlurSize = 1.25;
    int  	DerivativeOrder = 1; // order of the derivative
    int  	KSize = 5; // size of the extended Sobel kernel; it must be 1, 3, 5, or 7 (or -1 for Scharr)
};

std::array<cv::Mat, 2> ComputeSobelDerivatives(const cv::Mat&image, const SobelParams& params)
{
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::Mat img_float;
    gray.convertTo(img_float, CV_32FC1);
    img_float = img_float / 255.;

    cv::Mat blurred;
    cv::GaussianBlur(img_float, blurred, cv::Size(), params.GaussianBlurSize, params.GaussianBlurSize);

    std::array<cv::Mat, 2> r;
    int ddepth = CV_64F;

    double good_scale = 1. / pow(2., (double)(params.KSize - 2 * params.DerivativeOrder - 2));
    //params.Scale = good_scale;
    cv::Sobel(blurred, r[0], ddepth, params.DerivativeOrder, 0, params.KSize, good_scale);
    cv::Sobel(blurred, r[1], ddepth, 0, params.DerivativeOrder, params.KSize, good_scale);

    return r;
}

bool GuiSobelParams(SobelParams& params)
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
        ImageTransparent = cv::imread("resources/bear_transparent.png", cv::IMREAD_UNCHANGED);
    }
    void UpdateImages()
    {
        ImageFiltered = ComputeSobelDerivatives(Image, SobelParams);
    }

    cv::Mat Image;
    cv::Mat ImageTransparent;
    std::array<cv::Mat, 2> ImageFiltered;
    cv::Size DisplaySize = cv::Size(0, 220);
    SobelParams SobelParams;
};

AppState gAppState;


void guiFunction()
{
    if (gAppState.Image.empty())
    {
        gAppState.LoadImages();
        gAppState.UpdateImages();
    }

    bool changed  = false;

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

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

    static ImmVision::ImageNavigatorParams imageNavigatorParamsFilter[2];
    for (size_t i = 0; i < 2; ++i)
    {
        if (i > 0)
            ImGui::SameLine();
        imageNavigatorParamsFilter[i].ImageDisplaySize = gAppState.DisplaySize;
        imageNavigatorParamsFilter[i].ZoomKey = "i";
        imageNavigatorParamsFilter[i].ColorAdjustmentsKey = "c";
        imageNavigatorParamsFilter[i].Legend = (i == 0) ? "X Gradient" : "Y Gradient";
        cv::Point2d mousePosition = ImmVision::ImageNavigator(
            gAppState.ImageFiltered[i],
            &imageNavigatorParamsFilter[i],
            changed);
    }

    if (changed)
        gAppState.UpdateImages();


    {
        static ImmVision::ImageNavigatorParams imageNavigatorParamsTransparent;
        imageNavigatorParamsTransparent.Legend = "Transparent image";
        imageNavigatorParamsTransparent.ImageDisplaySize = gAppState.DisplaySize;
        cv::Point2d mousePosition = ImmVision::ImageNavigator(
            gAppState.ImageTransparent,
            &imageNavigatorParamsTransparent,
            changed);
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

