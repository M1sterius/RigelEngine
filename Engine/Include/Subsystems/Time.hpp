#pragma once

#include "Core.hpp"
#include "Stopwatch.hpp"
#include "RigelSubsystem.hpp"

namespace Rigel
{
    class ProjectSettings;

    class Time final : public RigelSubsystem
    {
    public:
        NODISCARD static float64_t GetDeltaTime();
        NODISCARD static float32_t GetDeltaTimeF();

        NODISCARD static float64_t GetDeltaTimeUnscaled();
        NODISCARD static float32_t GetDeltaTimeUnscaledF();

        // Note that global time is always unscaled
        NODISCARD static float64_t GetGlobalTime();
        NODISCARD static float32_t GetGlobalTimeF();

        NODISCARD static uint64_t GetFrameCount();

        static void SetTargetFPS(const uint64_t fps);
        NODISCARD static uint64_t GetTargetFPS();

        static void SetTimeScale(const float64_t timeScale);
        NODISCARD static float64_t GetTimeScale();

        static constexpr float64_t MAX_DELTA_TIME_SECONDS = 1.0;
        static constexpr float64_t MIN_DELTA_TIME_SECONDS = 0.0001;
        static constexpr float64_t FALLBACK_DELTA_TIME_SECONDS = 0.016;
    INTERNAL:
        Time() = default;
        ~Time() override = default;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;
    private:
        NODISCARD static float64_t CorrectDeltaTime(float64_t deltaTime);

        Stopwatch m_GlobalTimeStopwatch{};
        Stopwatch m_DeltaTimeStopwatch{};

        uint32_t m_TargetFPS{};
        uint32_t m_TargetTickrate{};
        uint64_t m_FrameCounter{};

        float64_t m_DeltaTime{};
        float64_t m_PhysicsTickTime{};
        float64_t m_TimeScale{};

        friend class Engine;
    };
}
