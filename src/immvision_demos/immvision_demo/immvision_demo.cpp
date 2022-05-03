#include "hello_imgui/hello_imgui.h"
#include "immvision/immvision.h"
#include "immvision/internal/imgui/imgui_imm.h"

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
    bool ShowImageFiltered = true;
    cv::Mat ImageFiltered;
    cv::Size DisplaySize = cv::Size(0, 400);
    SobelParamsValues SobelParams;
};

static AppState gAppState;


void guiFunction()
{
    if (gAppState.Image.empty())
    {
        gAppState.LoadImages();
        gAppState.UpdateImages();
    }

    bool changed  = false;

    ImGui::Text("FPS: %.1lf", (double)ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::Checkbox("Show filtered image", &gAppState.ShowImageFiltered);

    if (gAppState.ShowImageFiltered)
    {
        changed |= GuiSobelParams(gAppState.SobelParams);
        ImGui::SameLine();
        changed |= GuiDisplaySize(gAppState.DisplaySize);
    }

    if (changed)
    {
        gAppState.UpdateImages();
    }

    {
        static ImmVision::ImageParams originalImageParams {
            .RefreshImage = false,
            .ImageDisplaySize = gAppState.DisplaySize,
            .Legend = "Original Image",
            .ZoomPanMatrix = ImmVision::MakeZoomPanMatrix(
                cv::Point2d(1004., 953.), 40., originalImageParams.ImageDisplaySize),
            .ZoomKey = "i",
        };

        originalImageParams.ImageDisplaySize = gAppState.DisplaySize;
        ImmVision::Image(
            gAppState.Image,
            &originalImageParams);
    }

    if (gAppState.ShowImageFiltered)
    {
        ImGui::SameLine();
        {
            static ImmVision::ImageParams imageParamsFilter {
                .RefreshImage = false,
                .ImageDisplaySize = gAppState.DisplaySize,
                .Legend = "X & Y Gradients (see channels 0 & 1)",
                .ZoomPanMatrix = ImmVision::MakeZoomPanMatrix(
                    cv::Point2d(1004., 953.), 40., imageParamsFilter.ImageDisplaySize),
                .ZoomKey = "i"
            };

            imageParamsFilter.RefreshImage = changed;
            imageParamsFilter.ImageDisplaySize = gAppState.DisplaySize;
            ImmVision::Image(
                gAppState.ImageFiltered,
                &imageParamsFilter);
        }
    }


    {
        static cv::Mat imageTransparent = cv::imread("resources/bear_transparent.png", cv::IMREAD_UNCHANGED);
        ImmVision::ImageDisplay(
            imageTransparent, cv::Size(0, 400),
            false, // refresh
            false // show options button
            );
    }

//    ImGui::Begin("Style");
//    ImGui::ShowStyleEditor(nullptr);
//    ImGui::End();

    //ImGui::ShowMetricsWindow(NULL);
    //ImGui::ShowMetricsWindow(NULL);
    //ImGui::ShowDemoWindow(NULL);
}


void guiImageDisplayOnly()
{
    {
        static cv::Mat image2 = cv::imread("resources/house.jpg", cv::IMREAD_UNCHANGED);
        static ImmVision::ImageParams params;
        params.ImageDisplaySize = cv::Size(0, 300);
        ImmVision::Image(image2, &params);
    }
    ImGui::SameLine();

    ImGui::BeginGroup();

    //static cv::Mat image = cv::imread("resources/house.jpg", cv::IMREAD_UNCHANGED);
    //static cv::Mat image = cv::imread("resources/bear_transparent.png", cv::IMREAD_UNCHANGED);

#ifdef __EMSCRIPTEN__
    static cv::Mat image = cv::imread("resources/house.jpg", cv::IMREAD_UNCHANGED);
#else
    static cv::VideoCapture cap(0);
    static cv::Mat image;
    cap >> image;
    ImGui::SetMaxWaitBeforeNextFrame(1. / 100.);
#endif

    cv::Point2d mousePosition = ImmVision::ImageDisplay(
        image,
        cv::Size(0, 300),
        true, // refresh
        true // show options button
    );
    ImGui::Text("Mouse position %.2lf, %.2lf", mousePosition.x, mousePosition.y);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::EndGroup();
}


int main(int, char* [])
{
    HelloImGui::RunnerParams params;
    params.appWindowParams.windowSize = {1000.f, 800.f};
    params.appWindowParams.windowTitle = "ImmVision Demo";
    //params.callbacks.ShowGui = guiFunction;
    params.callbacks.ShowGui = guiImageDisplayOnly;
    HelloImGui::Run(params);

    return 0;
}

