#include "Time.hpp"
#include "Engine.hpp"

#include <algorithm>

namespace rge
{
    float64_t Time::GetDeltaTime()
    {
        const auto& engine = Engine::Get();
        return engine.GetDeltaTime();
    }

    float32_t Time::GetDeltaTimeF()
    {
        return static_cast<float32_t>(GetDeltaTime());
    }

    float64_t Time::GetGlobalTime()
    {
        const auto& engine = Engine::Get();
        return engine.GetGlobalTime();
    }

    float32_t Time::GetGlobalTimeF()
    {
        return static_cast<float32_t>(GetGlobalTime());
    }

    uint64_t Time::GetFrameCount()
    {
        const auto& engine = Engine::Get();
        return engine.GetFrameCount();
    }

    uint64_t Time::GetTargetFPS()
    {
        const auto& engine = Engine::Get();
        return engine.TargetFps;
    }

    void Time::SetTargetFPS(const uint64_t fps)
    {
        auto& engine = Engine::Get();
        engine.TargetFps = fps;
    }


}
