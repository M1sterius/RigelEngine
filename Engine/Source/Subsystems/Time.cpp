#include "Time.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "SubsystemGetters.hpp"

#ifdef RIGEL_PLATFORM_WINDOWS
    // this define removes global legacy windows min/max macros that break everything when used with pch
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include "Windows.h"
#endif

namespace Rigel
{
    ErrorCode Time::Startup(const ProjectSettings& settings)
    {
        #ifdef RIGEL_PLATFORM_WINDOWS
            static const UINT periodMin = []
            {
                TIMECAPS tc;
                timeGetDevCaps(&tc, sizeof(TIMECAPS));
                return tc.wPeriodMin;
            }();

            timeBeginPeriod(periodMin);
        #endif

        m_TargetFPS = settings.TargetFPS == 0 ? 1000 : settings.TargetFPS;
        m_TargetTickrate = settings.PhysicsTickrate == 0 ? 1000 : settings.PhysicsTickrate;
        m_TimeScale = settings.TimeScale;

        m_PhysicsTickTime = 1.0 / static_cast<float64_t>(m_TargetTickrate);
        m_DeltaTime = 1.0 / static_cast<float64_t>(m_TargetFPS);

        return ErrorCode::OK;
    }

    ErrorCode Time::Shutdown()
    {
        #ifdef RIGEL_PLATFORM_WINDOWS
            static const UINT periodMin = []
            {
                TIMECAPS tc;
                timeGetDevCaps(&tc, sizeof(TIMECAPS));
                return tc.wPeriodMin;
            }();

            timeEndPeriod(periodMin);
        #endif

        return ErrorCode::OK;
    }

    float64_t Time::GetDeltaTime()
    {

        return CorrectDeltaTime(GetTime()->m_DeltaTime * GetTime()->m_TimeScale);
    }

    float32_t Time::GetDeltaTimeF()
    {
        return static_cast<float32_t>(GetDeltaTime());
    }

    float64_t Time::GetDeltaTimeUnscaled()
    {

        return CorrectDeltaTime(GetTime()->m_DeltaTime);
    }

    float32_t Time::GetDeltaTimeUnscaledF()
    {
        return static_cast<float32_t>(GetDeltaTimeUnscaled());
    }

    float64_t Time::GetGlobalTime()
    {

        return GetTime()->m_GlobalTimeStopwatch.GetElapsed().AsSeconds();
    }

    float32_t Time::GetGlobalTimeF()
    {
        return static_cast<float32_t>(GetGlobalTime());
    }

    uint64_t Time::GetFrameCount()
    {

        return GetTime()->m_FrameCounter;
    }

    uint64_t Time::GetTargetFPS()
    {

        return GetTime()->m_TargetFPS;
    }

    void Time::SetTimeScale(const float64_t timeScale)
    {
        GetTime()->m_TimeScale = timeScale;
    }

    float64_t Time::GetTimeScale()
    {

        return GetTime()->m_TimeScale;
    }

    void Time::SetTargetFPS(const uint64_t fps)
    {
        ASSERT(fps, "Target framerate cannot be equal to 0!");

        GetTime()->m_TargetFPS = fps;
    }

    float64_t Time::CorrectDeltaTime(const float64_t deltaTime)
    {
        const float64_t correctedDeltaTime = (deltaTime >= MAX_DELTA_TIME_SECONDS || deltaTime <= MIN_DELTA_TIME_SECONDS)
                    ? FALLBACK_DELTA_TIME_SECONDS : deltaTime;

        if (abs(correctedDeltaTime - deltaTime) > 1e-9)
            Debug::Warning("Time::DeltaTime was outside the allowed ranged with the value of {}!", deltaTime);

        return correctedDeltaTime;
    }
}
