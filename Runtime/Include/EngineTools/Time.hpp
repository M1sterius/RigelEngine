#pragma once

#include "Utils/NODISCARD.hpp"
#include "Utils/Stopwatch.hpp"
#include "Utils/Types.hpp"

namespace rge
{
    class Engine;

    class Time
    {
    public:
        friend class Engine;

        NODISCARD static inline float64_t GetDeltaTime() { return DeltaTime; }
        NODISCARD static inline float32_t GetDeltaTimeF() { return static_cast<float32_t>(DeltaTime); }

        NODISCARD static inline float64_t GetGlobalTime() { return GlobalTimeStopwatch.GetElapsed().AsSeconds(); }
        NODISCARD static inline float64_t GetGlobalTimeF() { return static_cast<float32_t>(GlobalTimeStopwatch.GetElapsed().AsSeconds()); }

        NODISCARD static inline uint64_t GetFrameCount() { return FrameCount; }

        static inline void SetTargetFPS(const uint64_t fps) { TargetFPS = fps; }
        NODISCARD static inline uint64_t GetTargetFPS() { return TargetFPS; }

        static constexpr float64_t MAX_DELTA_TIME_SECONDS = 1.0;
        static constexpr float64_t MIN_DELTA_TIME_SECONDS = 0.000001;
        static constexpr float64_t FALLBACK_DELTA_TIME_SECONDS = 0.016;
    private:
        static void SetDeltaTime(const float64_t deltaTime);

        static Stopwatch DeltaTimeStopwatch;
        static Stopwatch GlobalTimeStopwatch;
        static float64_t DeltaTime;
        static uint64_t FrameCount;
        static uint64_t TargetFPS;
    };
}
