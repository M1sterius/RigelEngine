#pragma once

#include "Core.hpp"
#include "Stopwatch.hpp"
#include "RigelSubsystem.hpp"

namespace Rigel
{
    class Time final : public RigelSubsystem
    {
    public:
        NODISCARD static float64_t GetDeltaTime();
        NODISCARD static float32_t GetDeltaTimeF();

        NODISCARD static float64_t GetGlobalTime();
        NODISCARD static float32_t GetGlobalTimeF();

        NODISCARD static uint64_t GetFrameCount();

        static void SetTargetFPS(const uint64_t fps);
        NODISCARD static uint64_t GetTargetFPS();

        static constexpr float64_t MAX_DELTA_TIME_SECONDS = 1.0;
        static constexpr float64_t MIN_DELTA_TIME_SECONDS = 0.0001;
        static constexpr float64_t FALLBACK_DELTA_TIME_SECONDS = 0.016;
    INTERNAL:
        Time() = default;
        ~Time() override = default;

        int32_t Startup() override;
        int32_t Shutdown() override;
    private:
        NODISCARD static float64_t CorrectDeltaTime(float64_t deltaTime);
    };
}
