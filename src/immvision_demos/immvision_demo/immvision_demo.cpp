#include "imgui_sdl_opengl3_runner.h"
#include "mandelbrot.h"
#include "immvision/image_cv.h"

void guiFunction()
{
    static cv::Mat image = MakeMandelbrotImage(MandelbrotOptions());
    static immvision::ImageCv imgcv(image);

    ImGui::Text("Hello");
    imgcv.Draw();
}

int main()
{
    auto appParams = AppParams();

    return imgui_sdl_opengl3_run(guiFunction, appParams);
}