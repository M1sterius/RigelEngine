#pragma once

#include "Core.hpp"
#include "Stopwatch.hpp"

namespace rge
{
    class SleepUtility
    {
    public:
        /**
        * Sleeps the given amount of time with the maximum precision allowed on this platform.
        * @param time Time to sleep
        */
        static void PreciseSleep(const ElapsedTime& time);

        /**
         * Spins in a tight loop until the specified amount of time has elapsed.
         * @param time Time to wait
         */
        static void SpinWait(const ElapsedTime& time);

        /**
        *  Sleeps the amount of time required to cap FPS at the specified targetFPS value.
        * @param elapsed Time in seconds this frame took so far
        * @param targetFPS The limit this function will cap FPS at
        */
        static void LimitFPS(const ElapsedTime& elapsed, const uint32_t targetFPS);
    };
}
