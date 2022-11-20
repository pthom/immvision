#include "hello_imgui/hello_imgui.h"
#include "immvision/immvision.h"
#include "immvision/imgui_imm.h"
#include "immvision/internal/cv/colormap.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <array>

struct SobelParamsValues
{
    double GaussianBlurSize = 1.25;
    int  	DerivativeOrder = 1; // order of the derivative
    int  	KSize = 5; // size of the extended Sobel kernel; it must be 1, 3, 5, or 7 (or -1 for Scharr)
};

std::array<cv::Mat,2> ComputeSobelDerivatives(const cv::Mat&image, const SobelParamsValues& params)
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

//    cv::Mat m2;
//    cv::merge(r, m2);
//    return m2;
    return {r[0], r[1]};
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
    changed |= ImGuiImm::SliderAnyFloat("##Blur Size", &params.GaussianBlurSize, 0.5, 50.);
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
    std::array<cv::Mat,2> ImageFiltered;
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

    if (gAppState.ShowImageFiltered)
    {
        {
            static std::array<ImmVision::ImageParams, 2> imageParamsFilter;

            for (int i = 0; i < 2; ++i)
            {
                imageParamsFilter[i].RefreshImage = changed;
                imageParamsFilter[i].ImageDisplaySize =  gAppState.DisplaySize;
                imageParamsFilter[i].ColormapKey = "c";
                imageParamsFilter[i].ZoomKey = "i";
                imageParamsFilter[i].ShowOptionsPanel = true;
                ImmVision::Image(
                    "Gradients" + std::to_string(i),
                    gAppState.ImageFiltered[i],
                    &imageParamsFilter[i]);
                ImGui::SameLine();
            }
            ImGui::NewLine();
        }
    }

    {
        static ImmVision::ImageParams originalImageParams {
            .RefreshImage = false,
            .ImageDisplaySize = gAppState.DisplaySize,
            //.ZoomPanMatrix = ImmVision::MakeZoomPanMatrix(cv::Point2d(1004., 953.), 40., originalImageParams.ImageDisplaySize),
            .ZoomKey = "i",
        };

        originalImageParams.ImageDisplaySize = gAppState.DisplaySize;
        ImmVision::Image(
            "Original Image",
            gAppState.Image,
            &originalImageParams);
    }

    ImGui::SameLine();

    {
        static cv::Mat imageTransparent = cv::imread("resources/bear_transparent.png", cv::IMREAD_UNCHANGED);
        ImmVision::ImageDisplay(
            "Transparent image",
            imageTransparent, cv::Size(0, 400),
            false, // refresh
            true // show options button
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
        ImmVision::Image("House", image2, &params);
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
#endif

    cv::Point2d mousePosition = ImmVision::ImageDisplay(
        "image",
        image,
        cv::Size(0, 300),
        true, // refresh
        true // show options button
    );
    ImGui::Text("Mouse position %.2lf, %.2lf", mousePosition.x, mousePosition.y);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::EndGroup();
}

void Foo()
{
    auto colormaps = ImmVision::Colormap::AvailableColormaps();
    //auto ts = ImmVision::Colormap::ColorMapsTextures();

//    auto ims = ImmVision::Colormap::ColorMapsImages();
//    for (const auto& kv: ims)
//    {
//        cv::imshow(kv.first, kv.second);
//        cv::waitKey();
//    }
}

int main(int, char* [])
{
    //Foo(); return 0;

    HelloImGui::RunnerParams params;
    params.appWindowParams.windowGeometry.size = {1400, 1000};
    params.appWindowParams.windowTitle = "ImmVision Demo";
    params.callbacks.ShowGui = guiFunction;
    //params.callbacks.ShowGui = guiImageDisplayOnly;
    HelloImGui::Run(params);

    return 0;
}

