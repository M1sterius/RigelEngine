#include "SleepUtility.hpp"

#ifdef RGE_PLATFORM_WINDOWS
#include "Windows.h"
#else
#include <chrono>
#include <thread>
#endif

namespace Rigel
{
    void SleepUtility::PreciseSleep(const ElapsedTime& time)
    {
        #ifdef RGE_PLATFORM_WINDOWS
        static const UINT periodMin = []
        {
            TIMECAPS tc;
            timeGetDevCaps(&tc, sizeof(TIMECAPS));
            return tc.wPeriodMin;
        }();

        timeBeginPeriod(periodMin);
        Sleep(time.AsMilliseconds());
        timeEndPeriod(periodMin);

        #else
        std::this_thread::sleep_for(std::chrono::milliseconds(time.AsMilliseconds()));
        #endif
    }

    void SleepUtility::SpinWait(const ElapsedTime& time)
    {
        const auto beginTime = Stopwatch::StartNew();

        while (true)
        {
            const auto passed = beginTime.GetElapsed();
            if (passed.AsMicroseconds() > time.AsMicroseconds()) break;
        }
    }

    void SleepUtility::LimitFPS(const ElapsedTime& elapsed, const uint32_t targetFPS)
    {
        const auto targetFrametime = 1.0 / static_cast<double>(targetFPS);
        const auto timeDiff = ElapsedTime(targetFrametime) - elapsed;
        if (timeDiff.AsSeconds() > 0) PreciseSleep(timeDiff);
    }
}
