#pragma once
#include <opencv2/core.hpp>
#include <pybind11/numpy.h>
#include <string>
#include <vector>


namespace cv_np
{
    struct TypeSynonyms
    {
        int         cv_depth = -1;
        std::string cv_depth_name;
        std::string np_format;

        pybind11::dtype dtype() { return pybind11::dtype(np_format); }
        std::string str() const;
    };

    extern std::vector<TypeSynonyms> sTypeSynonyms;

    std::string ListTypeSynonyms();
}