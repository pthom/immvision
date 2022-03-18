#include "imgui.h"
#include <string>

namespace ImmVisionSimpleRunner
{
    struct AppParams
    {
        std::string Title = "immvision";
        int Width = 1280;
        int Height = 720;
        ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    };

    using GuiFunctionPtr = void (*)();

    void Run(
        GuiFunctionPtr guiFunction,
        const AppParams& appParams = AppParams());

} // namespace ImmVisionSimpleRunner
