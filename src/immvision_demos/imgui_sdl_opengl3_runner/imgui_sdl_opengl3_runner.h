#include "imgui.h"
#include <string>

struct AppParams
{
    std::string Title = "immvision";
    int Width = 1280;
    int Height = 720;
    ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

using GuiFunctionPtr = void (*)();

int imgui_sdl_opengl3_run(
    GuiFunctionPtr guiFunction, 
    const AppParams& appParams = AppParams());
