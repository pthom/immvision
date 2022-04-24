#include "immvision/internal/gl/short_lived_cache.h"

#include <chrono>

namespace ImmVision
{
    namespace internal
    {
        double TimerSeconds()
        {
            using chrono_second = std::chrono::duration<double, std::ratio<1>>;
            using chrono_clock = std::chrono::steady_clock;

            static std::chrono::time_point<chrono_clock> startTime = chrono_clock::now();
            double elapsed = std::chrono::duration_cast<chrono_second>(chrono_clock::now() - startTime).count();
            return elapsed;
        }

    } // namespace internal
} // namespace ImmVision
