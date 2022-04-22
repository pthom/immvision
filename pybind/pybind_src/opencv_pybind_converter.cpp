#include "opencv_pybind_converter.h"
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>

// #define OPENCV_PYBIND_CONVERTER_LOG_ACTIVE

namespace opencv_pybind_converter
{
    struct ScalarTypeSynonyms
    {
        int cv_depth = -1;
        std::string pybind_format;
        std::string cv_depth_name;
    };


    std::vector<ScalarTypeSynonyms> sScalarTypeSynonyms{
        { CV_8U, pybind11::format_descriptor<uint8_t>::format(), "CV_8U" },
        { CV_8S, pybind11::format_descriptor<int8_t>::format(), "CV_8S" },
        { CV_16U, pybind11::format_descriptor<uint16_t>::format(), "CV_16U" },
        { CV_16S, pybind11::format_descriptor<int16_t>::format(), "CV_16S" },
        { CV_32S, pybind11::format_descriptor<int32_t>::format(), "CV_32S" },
        { CV_32F, pybind11::format_descriptor<float>::format(), "CV_32F" },
        { CV_64F, pybind11::format_descriptor<double>::format(), "CV_64F" },
#if (CV_MAJOR_VERSION >= 4)
        { CV_16F, pybind11::format_descriptor<cv::int16_t>::format(), "CV_16F" },
#endif
    };

    int PybindFormat_to_CvDepth(const std::string& format)
    {
        for (const auto& v : sScalarTypeSynonyms)
            if (v.pybind_format == format)
                return v.cv_depth;
        std::stringstream msg;
        msg << "PybindFormat_to_CvDepth: Unsupported type " << format;
        throw std::logic_error(msg.str().c_str());
    }
    std::string PybindFormat_to_CvDepth_string(const std::string& format)
    {
        for (const auto& v : sScalarTypeSynonyms)
            if (v.pybind_format == format)
                return v.cv_depth_name;
        std::stringstream msg;
        msg << "PybindFormat_to_CvDepth_string: Unsupported type " << format;
        throw std::logic_error(msg.str().c_str());
    }
    std::string CvDepth_to_PybindFormat(int cv_depth)
    {
        for (const auto& v : sScalarTypeSynonyms)
            if (v.cv_depth == cv_depth)
                return v.pybind_format;
        std::stringstream msg;
        msg << "CvDepth_to_PybindFormat: Unsupported cv_depth " << cv_depth;
        throw std::logic_error(msg.str().c_str());
    }
    std::string CvDepth_to_string(int cv_depth)
    {
        for (const auto& v : sScalarTypeSynonyms)
            if (v.cv_depth == cv_depth)
                return v.cv_depth_name;
        std::stringstream msg;
        msg << "CvDepth_to_string: Unsupported cv_depth " << cv_depth;
        throw std::logic_error(msg.str().c_str());
    }


    std::string BufferInfoDescription(const pybind11::buffer_info& buffer_info)
    {
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
             format = %s (i.e %s for OpenCV)
             ndim = %i
             shape = %s
             strides = %s
         )",
             buffer_info.ptr,
             (int)buffer_info.itemsize,
             (int)buffer_info.size,
             buffer_info.format.c_str(), PybindFormat_to_CvDepth_string(buffer_info.format).c_str(),
             (int)buffer_info.ndim,
             fnDumpVector(buffer_info.shape).c_str(),
             fnDumpVector(buffer_info.strides).c_str()
        );
        std::string r(msg);
        return r;
    }

    std::string CvMatDescription(const cv::Mat& m)
    {
        char msg[2000];
        snprintf(msg, 2000, R"(
                elemSize1 (aka itemsize): %zu
                size (aka row*cols*channels): %i
                depth (aka format): %s,
                [rows, cols, channels] (aka shape): %i, %i, %i
        )",
                 m.elemSize1(),
                 m.rows * m.cols * m.channels(),
                 CvDepth_to_string(m.depth()).c_str(),
                 m.rows, m.cols, m.channels()
        );
        return msg;
    }

    cv::Mat BufferInfoToCvMat(const pybind11::buffer_info& bufferInfo)
    {
#ifdef OPENCV_PYBIND_CONVERTER_LOG_ACTIVE
        std::cout << "Conversion python-> C++\n";
        std::cout << "========================\n";
        std::cout << "Input bufferInfo \n"
                  << opencv_pybind_converter::BufferInfoDescription(bufferInfo) << std::endl;
#endif

        int nb_channels = bufferInfo.ndim > 2 ? (int)bufferInfo.shape[2] : 1;
        int cv_depth = opencv_pybind_converter::PybindFormat_to_CvDepth(bufferInfo.format);
        int cv_type = CV_MAKETYPE(cv_depth, nb_channels);
        cv::Mat m = cv::Mat(cv::Size((int)bufferInfo.shape[1], (int)bufferInfo.shape[0]), cv_type, bufferInfo.ptr, cv::Mat::AUTO_STEP);

#ifdef OPENCV_PYBIND_CONVERTER_LOG_ACTIVE
        std::cout << "Output cv::Mat \n"
                  << opencv_pybind_converter::CvMatDescription(m) << std::endl;
#endif
        return m;
    }

    pybind11::buffer_info CvMatToBufferInfo(const cv::Mat& m_maybe_discontinuous)
    {
        cv::Mat m = m_maybe_discontinuous;
        if (!m_maybe_discontinuous.isContinuous() || m_maybe_discontinuous.isSubmatrix())
            m = m_maybe_discontinuous.clone();

#ifdef OPENCV_PYBIND_CONVERTER_LOG_ACTIVE
        std::cout << "Conversion C++-> Python\n";
        std::cout << "========================\n";
        std::cout << "Input cv::Mat \n"
                  << opencv_pybind_converter::CvMatDescription(m) << std::endl;
#endif

        std::string format = opencv_pybind_converter::CvDepth_to_PybindFormat(m.depth());
        size_t elemsize = m.elemSize1();
        int ndim = m.channels() > 1 ? 3 : 2;

        std::vector<size_t> bufferdim;
        std::vector<size_t> strides;
        if (ndim == 2)
        {
            bufferdim = {(size_t) m.rows, (size_t) m.cols};
            strides = {elemsize * (size_t) m.cols, elemsize};
        }
        else if (ndim == 3)
        {
            size_t nb_channels = (size_t)m.channels();
            bufferdim = {(size_t) m.rows, (size_t) m.cols, nb_channels};
            strides = {elemsize * m.cols * nb_channels, elemsize * nb_channels, elemsize};
        }

        auto bufferInfo = pybind11::buffer_info(
            m.data,    /* Pointer to buffer */
            elemsize,  /* Size of one scalar */
            format,    /* Python struct-style format descriptor */
            ndim,       /* Number of dimensions */
            bufferdim, /* Buffer dimensions */
            strides    /* Strides (in bytes) for each index */
        );

#ifdef OPENCV_PYBIND_CONVERTER_LOG_ACTIVE
        std::cout << "Output buffer_info \n"
                  << opencv_pybind_converter::BufferInfoDescription(bufferInfo) << std::endl;
#endif
        return bufferInfo;
    }


} // namespace opencv_pybind_converter

