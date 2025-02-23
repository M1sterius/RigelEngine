#pragma once

#ifdef _WIN32
#include "Windows.h"
#endif

#include <cstdint>
#include <chrono>
#include <thread>

namespace rge
{
     /**
     * Sleeps the given amount of time with the maximum precision allowed on this platform
     * @param milliseconds Time to sleep in milliseconds
     */
    inline void PreciseSleep(const uint64_t milliseconds)
    {
        #ifdef _WIN32
        static const UINT periodMin = []
        {
            TIMECAPS tc;
            timeGetDevCaps(&tc, sizeof(TIMECAPS));
            return tc.wPeriodMin;
        }();

        timeBeginPeriod(periodMin);
        Sleep(milliseconds);
        timeEndPeriod(periodMin);
        #else
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        #endif
    }

    /**
     *  Sleeps the given amount of time with the maximum precision allowed on this platform
     * @param seconds Time to sleep in seconds
     */
    inline void PreciseSleep(const double seconds)
    {
        const auto ms = static_cast<uint64_t>(seconds * 1000);
        PreciseSleep(ms);
    }

    /**
     * Sleeps give amount of time by spinning it a tight loop
     * @param seconds The time in seconds this function will spin
     */
    inline void Spin(const double seconds)
    {
        const auto beginTime = std::chrono::high_resolution_clock::now();

        while (true)
        {
            const std::chrono::duration<double> passed = std::chrono::high_resolution_clock::now() - beginTime;
            if (passed.count() >= seconds) break;
        }
    }

    /**
     *  Sleeps the amount of time required to cap FPS at the specified fpsLimit value
     * @param time Time in seconds this frame took so far
     * @param fpsLimit The limit this function will cap FPS at
     */
    inline void LimitFPS(const double time, const uint64_t fpsLimit)
    {
        const auto frameTime = 1.0 / (double)fpsLimit;
        const auto timeDiff = frameTime - time;
        if (timeDiff > 0) PreciseSleep(timeDiff);
    }
}
