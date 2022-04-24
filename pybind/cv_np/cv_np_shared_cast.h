#pragma once
#include "cv_np_synonyms.h"
#include <opencv2/core/core.hpp>
#include <pybind11/numpy.h>

#include <iostream>
#include <stdexcept>
#include <vector>
//
// Explicit casters between cv::Mat / cv::Matx and numpy.ndarray, with shared memory
// Also see automatic casters in the namespace pybind11:detail below
//
namespace cv_np_shared_cast
{
    //
    // Public interface
    //
    pybind11::array mat_to_nparray(const cv::Mat& m);
    cv::Mat         nparray_to_mat(pybind11::array& a);

        template<typename _Tp, int _rows, int _cols>
    pybind11::array matx_to_nparray(const cv::Matx<_Tp, _rows, _cols>& m);
        template<typename _Tp, int _rows, int _cols>
    void            nparray_to_matx(pybind11::array &a, cv::Matx<_Tp, _rows, _cols>& out_matrix);


    //
    // Private details and implementations below
    //
    namespace detail
    {
        template<typename _Tp, int _rows, int _cols>
        pybind11::capsule make_capsule_matx(const cv::Matx<_Tp, _rows, _cols>& m)
        {
            return pybind11::capsule(new cv::Matx<_Tp, _rows, _cols>(m)
                , [](void *v) { delete reinterpret_cast<cv::Matx<_Tp, _rows, _cols>*>(v); }
            );
        }
    } // namespace detail

    template<typename _Tp, int _rows, int _cols>
    pybind11::array matx_to_nparray(const cv::Matx<_Tp, _rows, _cols>& m)
    {
        return pybind11::array(
            pybind11::dtype::of<_Tp>()
            , std::vector<std::size_t> {_rows, _cols}
            , m.val
            , detail::make_capsule_matx<_Tp, _rows, _cols>(m));
    }

    template<typename _Tp, int _rows, int _cols>
    void nparray_to_matx(pybind11::array &a, cv::Matx<_Tp, _rows, _cols>& out_matrix)
    {
        size_t mat_size = (size_t)(_rows * _cols);
        if (a.size() != mat_size)
            throw std::runtime_error("Bad size");

        _Tp* arrayValues = (_Tp*) a.data(0);
        for (size_t i = 0; i < mat_size; ++i)
            out_matrix.val[i] = arrayValues[i];
    }
} // namespace cv_np_shared_cast



//
// 1. Casts with shared memory between {cv::Mat, cv::Matx, cv::Vec} and numpy.ndarray
//
// 2. Casts without shared memory between {cv::Size, cv::Point, cv::Point3} and python tuples
//
namespace pybind11
{
    namespace detail
    {
        //
        // Cast between cv::Mat and numpy.ndarray
        // The cast between cv::Mat and numpy.ndarray works with shared memory:
        //   any modification to the Matrix size, type, and values is immediately
        //   impacted on both sides.
        //
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
                auto a = reinterpret_borrow<array>(src);
                auto new_mat = cv_np_shared_cast::nparray_to_mat(a);
                value = new_mat;
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
                auto a = cv_np_shared_cast::mat_to_nparray(m);
                return a.release();
            }
        };


        //
        // Cast between cv::Matx<_rows,_cols> (aka Matx33d, Matx21d, etc) + Vec<_rows> (aka Vec1d, Vec2f, etc) and numpy.ndarray
        // The cast between cv::Matx, cv::Vec and numpy.ndarray works with shared memory:
        //   any modification to the Matrix size, type, and values is immediately
        //   impacted on both sides.
        //
        template<typename _Tp, int _rows, int _cols>
        struct type_caster<cv::Matx<_Tp, _rows, _cols> >
        {
            using Matxxx = cv::Matx<_Tp, _rows, _cols>;

        public:
        PYBIND11_TYPE_CASTER(Matxxx, _("numpy.ndarray"));

            // Conversion part 1 (Python->C++)
            bool load(handle src, bool)
            {
                auto a = reinterpret_borrow<array>(src);
                cv_np_shared_cast::nparray_to_matx<_Tp, _rows, _cols>(a, value);
                return true;
            }

            // Conversion part 2 (C++ -> Python)
            static handle cast(const Matxxx &m, return_value_policy, handle defval)
            {
                auto a = cv_np_shared_cast::matx_to_nparray<_Tp, _rows, _cols>(m);
                return a.release();
            }
        };



        //
        // Cast between cv::Size and a simple python tuple.
        // No shared memory, you cannot modify the width or the height without
        // transferring the whole Size from C++, or the tuple from python
        //
        template<typename _Tp>
        struct type_caster<cv::Size_<_Tp>>
        {
            using SizeTp = cv::Size_<_Tp>;

        public:
        PYBIND11_TYPE_CASTER(SizeTp, _("tuple"));

            // Conversion part 1 (Python->C++, i.e tuple -> Size)
            bool load(handle src, bool)
            {
                auto tuple = pybind11::reinterpret_borrow<pybind11::tuple>(src);
                if (tuple.size() != 2)
                    throw std::invalid_argument("Size should be in a tuple of size 2");

                SizeTp r;
                r.width =  tuple[0].cast<_Tp>();
                r.height = tuple[1].cast<_Tp>();

                value = r;
                return true;
            }

            // Conversion part 2 (C++ -> Python, i.e Size -> tuple)
            static handle cast(const SizeTp &value, return_value_policy, handle defval)
            {
                auto result = pybind11::make_tuple(value.width, value.height);
                return result.release();
            }
        };


        //
        // Cast between cv::Point and a simple python tuple
        // No shared memory, you cannot modify x or y without
        // transferring the whole Point from C++, or the tuple from python
        //
        template<typename _Tp>
        struct type_caster<cv::Point_<_Tp>>
        {
            using PointTp = cv::Point_<_Tp>;

        public:
        PYBIND11_TYPE_CASTER(PointTp , _("tuple"));

            // Conversion part 1 (Python->C++)
            bool load(handle src, bool)
            {
                auto tuple = pybind11::reinterpret_borrow<pybind11::tuple>(src);
                if (tuple.size() != 2)
                    throw std::invalid_argument("Point should be in a tuple of size 2");

                PointTp r;
                r.x = tuple[0].cast<_Tp>();
                r.y = tuple[1].cast<_Tp>();

                value = r;
                return true;
            }

            // Conversion part 2 (C++ -> Python)
            static handle cast(const PointTp &value, return_value_policy, handle defval)
            {
                auto result = pybind11::make_tuple(value.x, value.y);
                return result.release();
            }
        };



        //
        // Point3
        // No shared memory
        //
        template<typename _Tp>
        struct type_caster<cv::Point3_<_Tp>>
        {
            using PointTp = cv::Point3_<_Tp>;

        public:
        PYBIND11_TYPE_CASTER(PointTp , _("tuple"));

            // Conversion part 1 (Python->C++)
            bool load(handle src, bool)
            {
                auto tuple = pybind11::reinterpret_borrow<pybind11::tuple>(src);
                if (tuple.size() != 3)
                    throw std::invalid_argument("Point3 should be in a tuple of size 3");

                PointTp r;
                r.x = tuple[0].cast<_Tp>();
                r.y = tuple[1].cast<_Tp>();
                r.z = tuple[2].cast<_Tp>();

                value = r;
                return true;
            }

            // Conversion part 2 (C++ -> Python)
            static handle cast(const PointTp &value, return_value_policy, handle defval)
            {
                auto result = pybind11::make_tuple(value.x, value.y, value.z);
                return result.release();
            }
        };


    }  // namespace detail
}  // namespace pybind11

