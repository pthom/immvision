#pragma once
#ifdef IMMVISION_PYBIND

#include <opencv2/core/core.hpp>
#include <pybind11/numpy.h>

#include <iostream>
#include <stdexcept>
#include <vector>

namespace opencv_pybind_converter
{
    int PybindFormat_to_CvDepth(const std::string& format);
    std::string PybindFormat_to_CvDepth_string(const std::string& format);
    std::string CvDepth_to_string(int cv_depth);
    std::string CvDepth_to_PybindFormat(int cv_depth);

    std::string BufferInfoDescription(const pybind11::buffer_info& buffer_info);
    std::string CvMatDescription(const cv::Mat& m);

    cv::Mat BufferInfoToCvMat(const pybind11::buffer_info& bufferInfo);
    pybind11::buffer_info CvMatToBufferInfo(const cv::Mat& m);

} // namespace opencv_pybind_converter


namespace pybind11
{
    // Inspired (and heavily modified) from https://github.com/pybind/pybind11/issues/538#issuecomment-273981569
    namespace detail
    {

        template<>
        struct type_caster<cv::Mat>
        {
        public:
        PYBIND11_TYPE_CASTER(cv::Mat, _("numpy.ndarray"));

            /**
             * Conversion part 1 (Python->C++):
             * Return false upon failure.
             * The second argument indicates whether implicit conversions should be applied.
             */
            bool load(handle src, bool)
            {
                auto b = reinterpret_borrow<array>(src);
                buffer_info bufferInfo = b.request();
                value = opencv_pybind_converter::BufferInfoToCvMat(bufferInfo);
                return true;
            }

            /**
             * Conversion part 2 (C++ -> Python):
             * The second and third arguments are used to indicate the return value policy and parent object
             * (for ``return_value_policy::reference_internal``) and are generally
             * ignored by implicit casters.
             */
            static handle cast(const cv::Mat &m, return_value_policy, handle defval)
            {
                auto bufferInfo = opencv_pybind_converter::CvMatToBufferInfo(m);
                return array(bufferInfo).release();
            }
        };


        template<typename _Tp, int _rows, int _cols>
        struct type_caster<cv::Matx<_Tp, _rows, _cols> >
        {
            using Matxxx = cv::Matx<_Tp, _rows, _cols>;

        public:
        PYBIND11_TYPE_CASTER(Matxxx, _("numpy.ndarray"));

            // Conversion part 1 (Python->C++)
            bool load(handle src, bool)
            {
                auto as_array = reinterpret_borrow<array>(src);
                cv::Mat asMat = as_array.cast<cv::Mat>();
                Matxxx asMatx(asMat);
                value = asMatx;
                return true;
            }

            // Conversion part 2 (C++ -> Python)
            static handle cast(const Matxxx &value, return_value_policy, handle defval)
            {
                cv::Mat asMat(value);
                auto result = pybind11::cast(asMat);
                return result.release();
            }
        };

    }  // namespace detail
}  // namespace pybind11

#endif // #ifdef IMMVISION_PYBIND
