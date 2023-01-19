#include "hello_imgui/hello_imgui.h"
#include "immdebug_viewer/single_instance_app.h"
#include "immdebug/immdebug_internal.h"
#include "immvision/inspector.h"

#include "GLFW/glfw3.h"
#include <iostream>


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

            ImmVision::Inspector_AddImage(
                imagePayload->Image,
                imagePayload->Legend,
                imagePayload->ZoomKey,
                imagePayload->ColorAdjustmentsKey,
                imagePayload->ZoomCenter,
                imagePayload->ZoomRatio,
                imagePayload->isColorOrderBGR
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
    HelloImGui::LogGui(ImVec2(0.f, 300.f));


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
