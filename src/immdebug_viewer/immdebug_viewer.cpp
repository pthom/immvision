#include "hello_imgui/hello_imgui.h"
#include "immdebug_viewer/single_instance_app.h"
#include "immdebug/immdebug_internal.h"
#include "immvision/inspector.h"

#include "GLFW/glfw3.h"
#include <algorithm>
#include <cstdint>
#include <iostream>


// Swap R and B channels, returning a new image. Only handles uint8 3/4-channel images.
static ImmVision::ImageBuffer SwapRB(const ImmVision::ImageBuffer& src)
{
    ImmVision::ImageBuffer dst = src.clone();
    int ch = dst.channels;
    for (int y = 0; y < dst.height; y++)
    {
        uint8_t* row = dst.ptr<uint8_t>(y);
        for (int x = 0; x < dst.width; x++)
            std::swap(row[x * ch], row[x * ch + 2]);
    }
    return dst;
}

void AddIncomingImages()
{
    bool foundNewImages = false;
    std::optional<ImmVision::ImmDebug_Internal::ImagePayload> imagePayload;
    do
    {
        imagePayload = ImmVision::ImmDebug_Internal::ReadImagePayload();
        if (imagePayload)
        {
            HelloImGui::Log(HelloImGui::LogLevel::Info, "Received image payload");
            // Convert BGR images to RGB so the viewer always works in RGB mode
            if (imagePayload->isColorOrderBGR
                && imagePayload->Image.depth == ImmVision::ImageDepth::uint8
                && (imagePayload->Image.channels == 3 || imagePayload->Image.channels == 4))
            {
                imagePayload->Image = SwapRB(imagePayload->Image);
            }
            ImmVision::UseRgbColorOrder();
            ImmVision::Inspector_AddImage(
                imagePayload->Image,
                imagePayload->Legend,
                imagePayload->ZoomKey,
                imagePayload->ColorAdjustmentsKey,
                imagePayload->ZoomCenter,
                imagePayload->ZoomRatio
            );
            foundNewImages = true;
        }

    } while(imagePayload);

    if (foundNewImages)
        glfwFocusWindow((GLFWwindow*) HelloImGui::GetRunnerParams()->backendPointers.glfwWindow);
}


void Gui(ImmVision::SingleInstanceApp& singleInstanceApp)
{
    ImmVision::Inspector_Show();
    // HelloImGui::LogGui();

    // check for new images to show (every 10 frames, to reduce disk usage)
    static int idx = 0;
    if (idx % 10 == 0)
        AddIncomingImages();
    idx++;

    if (singleInstanceApp.WasPinged())
    {
        glfwFocusWindow((GLFWwindow*) HelloImGui::GetRunnerParams()->backendPointers.glfwWindow);
        HelloImGui::Log(HelloImGui::LogLevel::Warning, "Pong");
    }

}



int main()
{
    using namespace std::literals;

    std::string appLockId = "immdebug_viewer";
    ImmVision::SingleInstanceApp singleInstanceApp(appLockId);

    if ( ! singleInstanceApp.RunSingleInstance())
    {
        std::cout << "Exit...\n";
        return 0;
    }

    HelloImGui::RunnerParams params;
    params.appWindowParams.windowGeometry.fullScreenMode = HelloImGui::FullScreenMode::FullMonitorWorkArea;
    params.appWindowParams.restorePreviousGeometry = true;
    params.appWindowParams.windowTitle = "ImmVision - immdebug viewer";
    params.callbacks.ShowGui = [&singleInstanceApp]() {
        Gui(singleInstanceApp);
    };

    HelloImGui::Run(params);

    return 0;
}
