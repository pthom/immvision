#include "imgui.h"

#include <cstdio>
#include <cstdlib>

size_t GetPythonImGuiContextPointer();

static void*   MyMallocWrapper(size_t size, void* user_data)    { IM_UNUSED(user_data); return malloc(size); }
static void    MyFreeWrapper(void* ptr, void* user_data)        { IM_UNUSED(user_data); free(ptr); }

//
// If we were using ImGui as a DLL, we would need to instantiate ourselves this global variable for ImGui
// See note inside pybind_cmake.cmake for more explanation about the link with ImGui.
// (we are using a static link at the moment)
//
#ifdef IMMVISION_IMGUI_DLL
ImGuiContext*   GImGui = NULL;
#endif


void transfer_imgui_context()
{
    if (ImGui::GetCurrentContext() == NULL)
    {
        printf("cpp: SetImGuiContextFrom_pyimgui_Context detected null context!\n");
        size_t contextPointer = GetPythonImGuiContextPointer();
        printf("cpp: SetImGuiContextFrom_pyimgui_Context(%p) (got pointer from Python)\n", (void *)contextPointer);
        ImGui::SetCurrentContext((ImGuiContext*)contextPointer);
        //ImGui::SetAllocatorFunctions(MyMallocWrapper, MyFreeWrapper);
        ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
        printf("SetImGuiContextFrom_pyimgui_Context done\n");
    }
}
