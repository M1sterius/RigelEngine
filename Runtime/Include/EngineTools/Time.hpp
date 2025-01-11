#pragma once

#include "Utils/NODISCARD.hpp"
#include "Utils/Stopwatch.hpp"

namespace rge
{
    class Engine;

    class Time
    {
    public:
        friend class Engine;

        NODISCARD static inline double GetDeltaTime() { return DeltaTime; }
        NODISCARD static inline float GetDeltaTimeF() { return static_cast<float>(DeltaTime); }

        NODISCARD static inline double GetGlobalTime() { return GlobalTimeStopwatch.GetElapsed().AsSeconds(); }
        NODISCARD static inline double GetGlobalTimeF() { return static_cast<float>(GlobalTimeStopwatch.GetElapsed().AsSeconds()); }

        NODISCARD static inline uint64_t GetFrameCount() { return FrameCount; }

        static inline void SetTargetFPS(const uint64_t fps) { TargetFPS = fps; }
        NODISCARD static inline uint64_t GetTargetFPS() { return TargetFPS; }

        static constexpr double MAX_DELTA_TIME_SECONDS = 1.0;
        static constexpr double MIN_DELTA_TIME_SECONDS = 0.000001;
        static constexpr double FALLBACK_DELTA_TIME_SECONDS = 0.016;
    private:
        static void SetDeltaTime(const double deltaTime);

        static Stopwatch DeltaTimeStopwatch;
        static Stopwatch GlobalTimeStopwatch;
        static double DeltaTime;
        static uint64_t FrameCount;
        static uint64_t TargetFPS;
    };
}
