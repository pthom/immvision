#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cv_np/cv_np_shared_cast.h"

void pydef_cv_np(pybind11::module& m);
void pydef_cv_np_shared_test(pybind11::module& m);


PYBIND11_MODULE(cv_np, m)
{
    m.doc() = "...blablabla";

    pydef_cv_np_shared_test(m);
    pydef_cv_np(m);
}
