#include "Time.hpp"

#include <algorithm>

namespace rge
{
    Stopwatch Time::DeltaTimeStopwatch = Stopwatch();
    Stopwatch Time::GlobalTimeStopwatch = Stopwatch();
    double Time::DeltaTime = MIN_DELTA_TIME_SECONDS;
    uint64_t Time::FrameCount = 0;
    uint64_t Time::TargetFPS = 1000;

    void Time::SetDeltaTime(const double deltaTime)
    {
//        DeltaTime = std::clamp(deltaTime, MIN_DELTA_TIME_SECONDS, MAX_DELTA_TIME_SECONDS);

        DeltaTime = (deltaTime > MAX_DELTA_TIME_SECONDS || deltaTime < MIN_DELTA_TIME_SECONDS)
                    ? FALLBACK_DELTA_TIME_SECONDS
                    : deltaTime;

        if (deltaTime != DeltaTime)
            rge::Logger::Warning("Time::DeltaTime was out of allowed range with the value of: " + std::to_string(deltaTime));
    }
}
