#include "imgui_sdl_opengl3_runner.h"

void guiFunction()
{
    ImGui::Text("Hello");
}

int main()
{
    auto appParams = AppParams();

    return imgui_sdl_opengl3_run(guiFunction, appParams);
}