#pragma once

#include <opencv2/core.hpp>
#include <opencv2/core/cvdef.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <string>

//#define HAS_FLOAT_16

namespace numpy_opencv_convert
{
    template<typename T> int pybind_array_get_cv_depth(const pybind11::array_t<T>&) { return -1; }

//#define CV_8U   0
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<uint8_t>&) { return CV_8U; }
//#define CV_8S   1
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<int8_t>&) { return CV_8S; }
//#define CV_16U  2
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<uint16_t>&) { return CV_16U; }
//#define CV_16S  3
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<int16_t>&) { return CV_16S; }
//#define CV_32S  4
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<int32_t>&) { return CV_32S; }
//#define CV_32F  5
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<float>&) { return CV_32F; }
//#define CV_64F  6
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<double>&) { return CV_64F; }
#ifdef HAS_FLOAT_16
//#define CV_16F  7
    template<> int pybind_array_get_cv_depth(const pybind11::array_t<cv::float16_t>&) { return CV_16F; }
#endif // #ifdef HAS_FLOAT_16

    template<typename T>
    int pybind_array_get_cv_type(const pybind11::array_t<T>& array)
    {
        pybind11::buffer_info buffer_info = array.request();
        int cv_mat_type;
        {
            int nb_channels = 1;
            {
                if (buffer_info.ndim == 3)
                    nb_channels = buffer_info.shape[2];
            }
            int depth = pybind_array_get_cv_depth(array);
            cv_mat_type = CV_MAKETYPE(depth, nb_channels);
        }
        return cv_mat_type;
    }


    std::string cv_depth_name(int depth)
    {
        switch (depth)
        {
            case CV_8U:  return "CV_8U"; break;
            case CV_8S:  return "CV_8S"; break;
            case CV_16U: return "CV_16U"; break;
            case CV_16S: return "CV_16S"; break;
            case CV_32S: return "CV_32S"; break;
            case CV_32F: return "CV_32F"; break;
            case CV_64F: return "CV_64F"; break;
            case CV_16F: return "CV_16F"; break;
            default:
                assert(false);
                throw("unhandled depth!");
                break;
        }
    }


    std::string numeric_type_name(int depth)
    {
        switch (depth)
        {
            case CV_8U:  return "uint8_t"; break;
            case CV_8S:  return "int8_t"; break;
            case CV_16U: return "uint16_t"; break;
            case CV_16S: return "int16_t"; break;
            case CV_32S: return "int32_t"; break;
            case CV_32F: return "float"; break;
            case CV_64F: return "double"; break;
#ifdef HAS_FLOAT_16
            case CV_16F: return "cv::float16_t"; break;
#endif
            default:
                assert(false);
                throw("unhandled depth!");
                break;
        }
    }


    template<typename T>
    std::string get_inner_buffer_info(const pybind11::array_t<T>& array)
    {
        pybind11::buffer_info buffer_info = array.request();
        auto fnDumpVector = [](const std::vector<ssize_t>& xs) {
            std::string r = "[";
            for(ssize_t x : xs)
                r = r + std::to_string(x) + " ";
            r = r + "]";
            return r;
        };
        char msg[2000];
        snprintf(msg, 2000, R"(
         ptr = %p
         itemsize = %i
         size = %i
         format = %s
         ndim = %i
         shape = %s
         strides = %s
    )",
                 buffer_info.ptr,
                 (int)buffer_info.itemsize,
                 (int)buffer_info.size,
                 buffer_info.format.c_str(),
                 (int)buffer_info.ndim,
                 fnDumpVector(buffer_info.shape).c_str(),
                 fnDumpVector(buffer_info.strides).c_str()
        );
        std::string r(msg);
        return r;
    }

    template<typename T>
    std::string np_array_info(const pybind11::array_t<T>& array)
    {
        int cv_depth = pybind_array_get_cv_depth(array);
        std::string r;
        r+= "        Numeric type: " + numeric_type_name(cv_depth);
        r+= "        CV Depth    : " + cv_depth_name(cv_depth);
        r += get_inner_buffer_info(array);
        return r;
    }


    template<typename T>
    cv::Mat np_array_to_cv_mat(const pybind11::array_t<T>& array)
    {
        pybind11::buffer_info buffer_info = array.request();
        assert(buffer_info.ndim >= 2);
        int cv_type = pybind_array_get_cv_type(array);
        cv::Mat m(buffer_info.shape[0], buffer_info.shape[1], cv_type, buffer_info.ptr);
        return m;
    }

} // namespace numpy_opencv_convert




#define MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, depth)\
    module_name.def(function_name_str, function_name<depth>);


#ifdef HAS_FLOAT_16
    #define MODULE_DEF_FUNCTION_ALL_DEPTHS(module_name, function_name_str, function_name) \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, uint8_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, int8_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, uint16_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, int16_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, int32_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, float); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, double);  \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, cv::float16_t);
#else
    #define MODULE_DEF_FUNCTION_ALL_DEPTHS(module_name, function_name_str, function_name) \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, uint8_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, int8_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, uint16_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, int16_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, int32_t); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, float); \
        MODULE_DEF_FUNCTION_FOR_DEPTH(module_name, function_name_str, function_name, double);
#endif