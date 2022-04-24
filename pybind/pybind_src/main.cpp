#include "immvision/immvision.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cv_np/cv_np_shared_cast.h"
#include "imgui.h"
#include "imgui_internal.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


void pydef_cv_np(pybind11::module& m);
void pydef_cv_np_shared_test(pybind11::module& m);
void pydef_image(pybind11::module& m);
void pydef_image_navigator(pybind11::module& m);
void pydef_gl_provider_sentinel(pybind11::module& m);


std::string VersionInfo()
{
    char msg[2000];
    snprintf(msg, 2000, "immvision version %s (date: %s)\n", IMMVISION_VERSION, __TIMESTAMP__);
    return msg;
}


PYBIND11_MODULE(cpp_immvision, m)
{
    m.doc() = "immvision: immediate image debugger and insights";
    m.def("VersionInfo", VersionInfo);

    pydef_cv_np_shared_test(m);
    pydef_gl_provider_sentinel(m);
    pydef_cv_np(m);

    pydef_image(m);
    pydef_image_navigator(m);


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
