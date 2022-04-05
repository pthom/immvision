#include "immdebug_viewer/single_instance_app.h"
#include "immdebug/immdebug_internal.h"
#include <thread>
#include <iostream>


void AddIncomingImages()
{
    std::optional<ImmVision::ImmDebug_Internal::ImagePayload> imagePayload;
    do
    {
        imagePayload = ImmVision::ImmDebug_Internal::ReadImagePayload();
        if (imagePayload)
        {
            std::cout << "Should display payload of size " << imagePayload->Image.size() << std::endl;
        }
//        ImGuiCv::ImageExplorers_AddImage(
//            imagePayload.Label,
//            imagePayload.Image,
//            imagePayload.InitialImageAdjustments,
//            imagePayload.AdditionalLegend
//        );
    } while(imagePayload);
}
//
//cv::Mat MakeWindowIcon()
//{
//    cv::Mat icon(cv::Size(48, 48), CV_8UC3);
//    icon = cv::Scalar(100, 180, 120);
//    auto fontFace = cv::FONT_HERSHEY_SIMPLEX;
//    double fontScale = 0.33;
//    cv::putText(icon, "ImViewer", cv::Point(3, 30), fontFace, fontScale, cv::Scalar(200, 50, 50));
//    return icon;
//}
//
//void RunViewer(MakeSingleInstanceApp &singleInstanceApp)
//{
////    ImGuiExt::ImGuiRunner::ImGuiParams params;
////    params.windowed_full_screen = true;
////    params.win_width = 1024;
////    params.win_height = 768;
////    params.win_title = "imdebugui - Viewer";
////    params.clear_color = ImColor(114, 144, 154);
////
////    params.icon = MakeWindowIcon();
////
////    ImGuiExt::ImGuiRunner::run([&]() {
////        ApplySkin();
////        ImGuiCv::ImageExplorers_ShowAll();
////
////        // check for new images to show (every 10 frames, to reduce disk usage)
////        static int idx = 0;
////        if (idx % 10 == 0)
////            AddIncomingImages();
////        idx++;
////
////        if (singleInstanceApp.WasPingedFromOtherInstance())
////            ImGuiExt::ImGuiRunner::BringMainWindowToTop();
////
////    }, params);
//
//
//    while(true)
//    {
//        if (singleInstanceApp.WasPingedFromOtherInstance())
//            std::cout << "Received ping !" << std::endl;
//
//        using namespace std::literals;
//        std::this_thread::sleep_for(1s);
//    }
//}
//
//#ifdef _WIN32
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
//#else
//
//int main()
//#endif
//{
//    //MakeSingleInstanceApp::RemoveAtom(); return 0;
//
//    if (MakeSingleInstanceApp::IsOtherInstanceLaunched_PingIfSo())
//    {
//        std::cout << "Found other instance. It was pinged!" << std::endl;
//        return 1;
//    }
//
//    MakeSingleInstanceApp singleInstanceApp;
//    singleInstanceApp.CreateSingleInstanceAppAtomId();
//    RunViewer(singleInstanceApp);
//
//    return 0;
//}


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

    for (int i = 0; i < 200; ++i)
    {

        if (singleInstanceApp.WasPinged())
            std::cout << "Ping in main\n";
        std::this_thread::sleep_for(50ms);
    }
    return 0;
}
