#include "cv_np/cv_np_synonyms.h"
#include <string>
namespace cv_np
{
    std::vector<TypeSynonyms> sTypeSynonyms
    {
        { CV_8U,  "CV_8U", pybind11::format_descriptor<uint8_t>::format() },
        { CV_8S,  "CV_8S", pybind11::format_descriptor<int8_t>::format() },
        { CV_16U, "CV_16U", pybind11::format_descriptor<uint16_t>::format() },
        { CV_16S, "CV_16S", pybind11::format_descriptor<int16_t>::format() },
        { CV_32S, "CV_32S", pybind11::format_descriptor<int32_t>::format() },
        { CV_32F, "CV_32F", pybind11::format_descriptor<float>::format() },
        { CV_64F, "CV_64F", pybind11::format_descriptor<double>::format() },

        // Note: this format needs adaptations
        //#if (CV_MAJOR_VERSION >= 4)
        //        { CV_16F, "CV_16F", pybind11::format_descriptor<cv::float16_t>::format() },
        //#endif
    };


    static int sColumnWidth = 12;
    static std::string AlignCenter(const std::string& s)
    {
        int nb_spaces = s.size() < sColumnWidth ? sColumnWidth - s.size() : 0;
        int nb_spaces_left = nb_spaces / 2;
        int nb_spaces_right = sColumnWidth - s.size() - nb_spaces_left;
        if (nb_spaces_right < 0)
            nb_spaces_right = 0;
        return std::string((size_t)nb_spaces_left, ' ') + s + std::string( (size_t)nb_spaces_right, ' ');
    }
    static std::string AlignCenter(const int v)
    {
        return AlignCenter(std::to_string(v));
    }

    std::string ListTypeSynonyms()
    {
        std::string title = AlignCenter("cv_depth") + AlignCenter("cv_depth_name") + AlignCenter("np_format");

        std::string r;
        r = title + "\n";
        for (const auto& format: sTypeSynonyms)
            r = r + format.str() + "\n";
        return r;
    }

    std::string TypeSynonyms::str() const
    {
        return AlignCenter(cv_depth) + AlignCenter(cv_depth_name) + AlignCenter(np_format);
    }
}