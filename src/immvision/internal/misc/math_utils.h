#pragma once
#include <vector>


namespace ImmVision
{
    namespace MathUtils
    {
        int RoundInt(double v);
        double MaximumValue(const std::vector<double> &v);
        double MinimumValue(const std::vector<double> &v);

        inline double Lerp(double a, double b, double x) noexcept
        {
            return (a + (b - a) * x);
        }
        inline double UnLerp(double a, double b, double x) noexcept
        {
            return (x - a) / (b - a);
        }

        inline std::vector<double> arange(double a, double b, double step)
        {
            std::vector<double> r;
            double v = a;
            while (v < b)
            {
                r.push_back(v);
                v += step;
            }
            return r;
        }
    } // namespace MathUtils


} // namespace ImmVision