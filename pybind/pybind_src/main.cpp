#include "immvision/immvision.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cv_np_shared_cast.h"
#include "imgui.h"
#include "imgui_internal.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

size_t GetPythonImGuiContextPointer();
void register_cv_np_shared_test_functions(pybind11::module& m);


namespace py = pybind11;

namespace ImmVision_GlProvider
{
    void InitGlProvider();
    void ResetGlProvider();
}


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

void SetImGuiContextFrom_pyimgui_Context()
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

std::string VersionInfo()
{
    char msg[2000];
    snprintf(msg, 2000, "immvision version %s (date: %s)\n", IMMVISION_VERSION, __TIMESTAMP__);
    return msg;
}

void Image(
    const cv::Mat& m,
    bool refresh,
    const cv::Size size = {0, 0},
    bool isBgrOrBgra = true
)
{
    SetImGuiContextFrom_pyimgui_Context();
    cv::Size cv_size(size.width, size.height);
    ImmVision::Image(m, refresh, cv_size, isBgrOrBgra);
}


void ImageNavigator(const cv::Mat& image)
{
    SetImGuiContextFrom_pyimgui_Context();
    cv::Size cv_size(500, 500);
    ImmVision::ImageNavigator(image, cv_size);
}


PYBIND11_MODULE(cpp_immvision, m) {
    m.doc() = R"pbdoc(
        immvision: immediate image debugger and insights
        -----------------------
        https://github.com/pthom/immvision/
    )pbdoc";

    register_cv_np_shared_test_functions(m);

    m.def("Image", Image, "Image doc",
        py::arg("image"),
        py::arg("refresh") = false,
        py::arg("size") = pybind11::make_tuple(0, 0),
        py::arg("isBgrOrBgra") = true
    );
    m.def("ImageNavigator", ImageNavigator);

    m.def("InitGlProvider", ImmVision_GlProvider::InitGlProvider);
    m.def("ResetGlProvider", ImmVision_GlProvider::ResetGlProvider);

    m.def("VersionInfo", VersionInfo);

    using namespace ImmVision;
    py::class_<ImageNavigatorParams>(m, "ImageNavigatorParams")
        .def(py::init<>())
        .def_readwrite("ImageDisplaySize", &ImageNavigatorParams::ImageDisplaySize)
        .def_readwrite("Legend", &ImageNavigatorParams::Legend)
        .def_readwrite("ZoomMatrix", &ImageNavigatorParams::ZoomMatrix)
        .def_readwrite("WatchedPixels", &ImageNavigatorParams::WatchedPixels)
        ;

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
