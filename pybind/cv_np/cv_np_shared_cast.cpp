#include "cv_np/cv_np_shared_cast.h"

// Thanks to Dan Mašek who gave me some inspiration here:
// https://stackoverflow.com/questions/60949451/how-to-send-a-cvmat-to-python-over-shared-memory

namespace cv_np_shared_cast
{

    namespace detail
    {
        namespace py = pybind11;
        
        py::dtype determine_np_dtype(int cv_depth)
        {
            for (auto format_synonym : cv_np::sTypeSynonyms)
                if (format_synonym.cv_depth == cv_depth)
                    return format_synonym.dtype();

            std::string msg = "numpy does not support this OpenCV depth: " + std::to_string(cv_depth) +  " (in determine_np_dtype)";
            throw std::invalid_argument(msg.c_str());
        }

        int determine_cv_depth(pybind11::dtype dt)
        {
            for (auto format_synonym : cv_np::sTypeSynonyms)
                if (format_synonym.dtype().is(dt))
                    return format_synonym.cv_depth;

            std::string msg = std::string("OpenCV does not support this numpy format: ") + dt.char_() +  " (in determine_np_dtype)";
            throw std::invalid_argument(msg.c_str());
        }

        int determine_cv_type(const py::array& a, int depth)
        {
            if (a.ndim() < 2)
                throw std::invalid_argument("determine_cv_type needs at least two dimensions");
            if (a.ndim() > 3)
                throw std::invalid_argument("determine_cv_type needs at most three dimensions");
            if (a.ndim() == 2)
                return CV_MAKETYPE(depth, 1);
            //We now know that shape.size() == 3
            return CV_MAKETYPE(depth, a.shape()[2]);
        }

        cv::Size determine_cv_size(const py::array& a)
        {
            if (a.ndim() < 2)
                throw std::invalid_argument("determine_cv_size needs at least two dimensions");
            return cv::Size(static_cast<int>(a.shape()[1]), static_cast<int>(a.shape()[0]));
        }

        std::vector<std::size_t> determine_shape(const cv::Mat& m)
        {
            if (m.channels() == 1) {
                return {
                    static_cast<size_t>(m.rows)
                    , static_cast<size_t>(m.cols)
                };
            }
            return {
                static_cast<size_t>(m.rows)
                , static_cast<size_t>(m.cols)
                , static_cast<size_t>(m.channels())
            };
        }

        py::capsule make_capsule_mat(const cv::Mat& m)
        {
            return py::capsule(new cv::Mat(m)
                , [](void *v) { delete reinterpret_cast<cv::Mat*>(v); }
            );
        }


    } // namespace detail

    pybind11::array mat_to_nparray(const cv::Mat& m)
    {
        if (!m.isContinuous())
            throw std::invalid_argument("Only continuous Mats supported.");
        return pybind11::array(detail::determine_np_dtype(m.depth())
            , detail::determine_shape(m)
            , m.data
            , detail::make_capsule_mat(m));
    }

    cv::Mat nparray_to_mat(pybind11::array& a)
    {
        int depth = detail::determine_cv_depth(a.dtype());
        int type = detail::determine_cv_type(a, depth);
        cv::Size size = detail::determine_cv_size(a);
        cv::Mat m(size, type, a.mutable_data(0));
        return m;
    }

} // namespace cv_np_shared_cast
