#pragma once
#ifdef IMMVISION_PYBIND

#include <opencv2/core/core.hpp>
#include <pybind11/numpy.h>

#include <iostream>
#include <stdexcept>

// Taken from
// https://github.com/pybind/pybind11/issues/538#issuecomment-273981569

namespace pybind11
{
namespace detail
{
  template <>
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
      /* Try a default converting into a Python */
      // array b(src, true);
      auto b = reinterpret_borrow<array>(src);  // true);
      buffer_info info = b.request();

      int ndims = static_cast<int>(info.ndim);

      decltype(CV_32F) dtype;
      size_t elemsize;
      if (info.format == format_descriptor<float>::format())
      {
        if (ndims == 3)
        {
          dtype = CV_32FC3;
        }
        else
        {
          dtype = CV_32FC1;
        }
        elemsize = sizeof(float);
      }
      else if (info.format == format_descriptor<double>::format())
      {
        if (ndims == 3)
        {
          dtype = CV_64FC3;
        }
        else
        {
          dtype = CV_64FC1;
        }
        elemsize = sizeof(double);
      }
      else if (info.format == format_descriptor<unsigned char>::format())
      {
        if (ndims == 3)
        {
          dtype = CV_8UC3;
        }
        else
        {
          dtype = CV_8UC1;
        }
        elemsize = sizeof(unsigned char);
      }
      else
      {
        throw std::logic_error("Unsupported type");
        return false;
      }
      (void)elemsize;
      std::vector<int> shape = {static_cast<int>(info.shape[0]), static_cast<int>(info.shape[1])};

      value = cv::Mat(cv::Size(shape[1], shape[0]), dtype, info.ptr, cv::Mat::AUTO_STEP);
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
      // std::cout << "m.cols : " << m.cols << std::endl;
      // std::cout << "m.rows : " << m.rows << std::endl;
      std::string format = format_descriptor<unsigned char>::format();
      size_t elemsize = sizeof(unsigned char);
      int dim;
      switch (m.type())
      {
        case CV_8U:
          format = format_descriptor<unsigned char>::format();
          elemsize = sizeof(unsigned char);
          dim = 2;
          break;
        case CV_8UC3:
          format = format_descriptor<unsigned char>::format();
          elemsize = sizeof(unsigned char);
          dim = 3;
          break;
        case CV_32F:
          format = format_descriptor<float>::format();
          elemsize = sizeof(float);
          dim = 2;
          break;
        case CV_64F:
          format = format_descriptor<double>::format();
          elemsize = sizeof(double);
          dim = 2;
          break;
        default:
          throw std::logic_error("Unsupported type");
      }

      std::vector<size_t> bufferdim;
      std::vector<size_t> strides;
      if (dim == 2)
      {
        bufferdim = {(size_t)m.rows, (size_t)m.cols};
        strides = {elemsize * (size_t)m.cols, elemsize};
      }
      else if (dim == 3)
      {
        bufferdim = {(size_t)m.rows, (size_t)m.cols, (size_t)3};
        strides = {(size_t)elemsize * m.cols * 3, (size_t)elemsize * 3, (size_t)elemsize};
      }
      return array(buffer_info(m.data,    /* Pointer to buffer */
                               elemsize,  /* Size of one scalar */
                               format,    /* Python struct-style format descriptor */
                               dim,       /* Number of dimensions */
                               bufferdim, /* Buffer dimensions */
                               strides    /* Strides (in bytes) for each index */
                               ))
          .release();
    }
  };


  template <typename _Tp, int _rows, int _cols>
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
