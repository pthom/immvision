#include "imgui.h"
#include "immvision/image.h"
#include <pybind11/pybind11.h>

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

void transfer_imgui_context_python_to_cpp()
{
    if (ImGui::GetCurrentContext() == NULL)
    {
        printf("cpp: SetImGuiContextFrom_pyimgui_Context detected null context!\n");
        size_t contextPointer = GetPythonImGuiContextPointer();
        printf("cpp: SetImGuiContextFrom_pyimgui_Context(%p) (got pointer from Python)\n", (void *)contextPointer);
        ImGui::SetCurrentContext((ImGuiContext*)contextPointer);
        //ImGui::SetAllocatorFunctions(MyMallocWrapper, MyFreeWrapper);
    }
    else
    {
        size_t pythonContextPointer = GetPythonImGuiContextPointer();
        size_t cppContextPointer = (size_t)ImGui::GetCurrentContext();
        if (pythonContextPointer != cppContextPointer)
        {
            printf("pythonContextPointer differs from ImGui::GetCurrentContext()! The graphical app was perhaps restarted...\n");
            ImGui::SetCurrentContext((ImGuiContext*)pythonContextPointer);
            ImmVision::ClearTextureCache();
        }
    }
}


void pydef_transfer_imgui_context_python_to_cpp(pybind11::module& m)
{
    m.def("transfer_imgui_context_python_to_cpp", transfer_imgui_context_python_to_cpp,
          "Pass ImGui context from python to C++");
}
