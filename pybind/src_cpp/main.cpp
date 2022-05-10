#include "immvision/immvision.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cvnp/cvnp.h"
#include "imgui.h"
#include "imgui_internal.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


void pydef_cvnp(pybind11::module& m);
void pydef_cvnp_test(pybind11::module& m);
void pydef_image(pybind11::module& m);
void pydef_gl_provider_sentinel(pybind11::module& m);
void pydef_transfer_imgui_context_python_to_cpp(pybind11::module& m);


//PYBIND11_MODULE(_module2, m)
//{
//}

PYBIND11_MODULE(_cpp_immvision, m)
{
    m.doc() = "immvision: immediate image debugger and insights";

    pydef_cvnp_test(m);
    pydef_gl_provider_sentinel(m);
    pydef_cvnp(m);
    pydef_transfer_imgui_context_python_to_cpp(m);
    pydef_image(m);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif

    // immvision._cpp_immvision.module2
    auto module2 = m.def_submodule("module2");
    module2.def("add", [](int a, int b) { return a + b;});

//    auto module3 = pybind11::module("module3");
//    module3.def("sub", [](int a, int b) { return a - b;});
//    pybind11::create_extension_module("aa", "", module3);
}

