#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace ImmVision_GlProvider
{
    void InitGlProvider();
    void ResetGlProvider();
}

void pydef_gl_provider_sentinel(pybind11::module& m)
{
    m.def("InitGlProvider", ImmVision_GlProvider::InitGlProvider);
    m.def("ResetGlProvider", ImmVision_GlProvider::ResetGlProvider);
}