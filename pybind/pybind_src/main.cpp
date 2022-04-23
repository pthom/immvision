#include "immvision/immvision.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cv_np_shared_cast.h"
#include "imgui.h"
#include "imgui_internal.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


void register_cv_np_shared_test_functions(pybind11::module& m);

void pybind_image_register(pybind11::module& m);
void pybind_image_navigator_register(pybind11::module& m);
void pybind_gl_provider_sentinel_register(pybind11::module& m);


namespace py = pybind11;


std::string VersionInfo()
{
    char msg[2000];
    snprintf(msg, 2000, "immvision version %s (date: %s)\n", IMMVISION_VERSION, __TIMESTAMP__);
    return msg;
}


PYBIND11_MODULE(cpp_immvision, m)
{
    m.doc() = R"pbdoc(
        immvision: immediate image debugger and insights
        https://github.com/pthom/immvision/
    )pbdoc";
    m.def("VersionInfo", VersionInfo);

    register_cv_np_shared_test_functions(m);
    pybind_gl_provider_sentinel_register(m);

    pybind_image_register(m);
    pybind_image_navigator_register(m);


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
