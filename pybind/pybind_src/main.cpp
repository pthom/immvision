#include "immvision/immvision.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "immvision/internal/opencv_pybind_converter.h"
#include "imgui.h"
#include "imgui_internal.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

size_t GetPythonImGuiContextPointer();


namespace py = pybind11;

namespace ImmVision_GlProvider
{
    void InitGlProvider();
    void ResetGlProvider();
}


static void*   MyMallocWrapper(size_t size, void* user_data)    { IM_UNUSED(user_data); return malloc(size); }
static void    MyFreeWrapper(void* ptr, void* user_data)        { IM_UNUSED(user_data); free(ptr); }

#ifndef IMMVISION_NOLINK_APPLE
    #define CREATE_GIMGUI_POINTER
#endif

#ifdef CREATE_GIMGUI_POINTER
    //ImGuiContext*   GImGui = NULL;
#endif

void SetImGuiContext()
{
    if (ImGui::GetCurrentContext() == NULL)
    {
        printf("cpp: SetImGuiContext detected null context!\n");
        size_t contextPointer = GetPythonImGuiContextPointer();
        printf("cpp: SetImGuiContext(%p) (got pointer from Python)\n", (void *)contextPointer);
        ImGui::SetCurrentContext((ImGuiContext*)contextPointer);
        ImGui::SetAllocatorFunctions(MyMallocWrapper, MyFreeWrapper);
        ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
        printf("SetImGuiContext done\n");
    }
}

void Hello()
{
    printf("Hello Version: %s Time:%s\n", IMMVISION_VERSION, IMMVISION_COMPILATION_TIMESTAMP);
}

void Image(
    const cv::Mat& m,
    bool refresh,
    const std::array<int, 2> size = {0, 0},
    bool isBgrOrBgra = true
)
{
    SetImGuiContext();

    cv::Size cv_size(size[0], size[1]);
    ImmVision::Image(m, refresh, cv_size, isBgrOrBgra);
}

void ImageNavigator(const cv::Mat& image)
{
    SetImGuiContext();
    cv::Size cv_size(500, 500);
    ImmVision::ImageNavigator(image, cv_size);
}


// This function will call the 2 casters defined in opencv_pybind_converter
// The unit tests check that the values and types are unmodified
cv::Mat RoundTrip_Mat_To_BufferInfo_To_Mat(const cv::Mat& m)
{
    return m;
}

PYBIND11_MODULE(cpp_immvision, m) {
    m.doc() = R"pbdoc(
        immvision: immediate image debugger and insights
        -----------------------
        https://github.com/pthom/immvision/
    )pbdoc";

    m.def("add", [](int a, int b) { return a + b; },
          R"pbdoc(Add two numbers)pbdoc"
          );

    m.def("Hello", Hello);

    m.def("Image", Image);
    m.def("ImageNavigator", ImageNavigator);

    m.def("InitGlProvider", ImmVision_GlProvider::InitGlProvider);
    m.def("ResetGlProvider", ImmVision_GlProvider::ResetGlProvider);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
