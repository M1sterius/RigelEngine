#pragma once

#include "Internal.hpp"
#include "NODISCARD.hpp"
#include "Stopwatch.hpp"

namespace rge
{
    class Core;

    class Time
    {
    public:
        friend class rge::Core;

        NODISCARD static double inline GetDeltaTime() { return DeltaTime; }
        NODISCARD static float inline GetDeltaTimeF() { return static_cast<float>(DeltaTime); }

        NODISCARD static double inline GetGlobalTime() { return GlobalTimeStopwatch.GetElapsed().AsSeconds(); }
        NODISCARD static double inline GetGlobalTimeF() { return static_cast<float>(GlobalTimeStopwatch.GetElapsed().AsSeconds()); }
    private:
        static Stopwatch DeltaTimeStopwatch;
        static Stopwatch GlobalTimeStopwatch;
        static double DeltaTime;
    };
}
