#include "immdebug/immdebug.h"

#include <vector>
#include <opencv2/core.hpp>


cv::Mat MakeRandomImage()
{
    cv::Size imageSize( rand() % 1200 + 10, rand() % 1000 + 10);

    std::vector<int> imageTypes {
        CV_8UC1,
        CV_8UC3,
        CV_8UC4,
//        CV_32FC1,
//        CV_64FC1
    };
    int imageType = imageTypes[rand() % imageTypes.size()];
    cv::Mat m(imageSize, imageType);

    double a0 = rand() % 10, a1 = rand() % 10, a2 = rand() % 10, a3 = rand() % 10, a4 = rand() % 10;
    constexpr double pi = 3.141;

    for (int row = 0; row < imageSize.height; ++row)
    {
        double y = (double) row / (double)imageSize.height * 2. * pi;
        for (int col = 0; col < imageSize.width; ++col)
        {
            double x = (double) col / (double)imageSize.width * 2. * pi;

            cv::Scalar color;

            color[0] = cos(a0 * x) + sin(a1 * y) + cos(x * y * a3);
            color[1] = sin(a0 * y) + sin(a1 * x) + cos(x * (y + 2.) * a4);
            color[2] = cos(a0 * x) + sin(a2 * y) + cos(x * y * a1);
            color[3] = cos(a0 * x) + sin(a1 * y) + cos(x * y * a0);

            color = ((color / 3.) + cv::Scalar(1., 1., 1., 1.)) / 2.;
            color[3] = 1.;

            if ( (imageType == CV_8UC1) || (imageType == CV_8UC3) || (imageType == CV_8UC4))
                color = color * 255.;

            cv::Range xr(col, col + 1);
            cv::Range yr(row, row + 1);
            m(yr, xr) = color;
        }
    }
    return m;
}

int main()
{
    for (int i = 0; i < 10; ++i)
    {
        cv::Mat m = MakeRandomImage();
        ImmVision::ImmDebug(m,"Rand image " + std::to_string(i));
    }
}
