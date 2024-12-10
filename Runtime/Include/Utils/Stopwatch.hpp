#pragma once

#include <chrono>
#include <cstdint>

namespace rge
{
    struct ElapsedTime
    {
    public:
        ElapsedTime(const std::chrono::high_resolution_clock::duration& duration)
            : m_Duration(duration)
        {

        }

        ~ElapsedTime() = default;

        inline std::chrono::high_resolution_clock::duration DurationRaw() const
        {
            return m_Duration;
        }

        inline double AsSeconds() const
        {
            const double seconds = std::chrono::duration<double>(m_Duration).count();
            return seconds;
        }

        inline float AsSecondsF() const
        {
            const float seconds = std::chrono::duration<float>(m_Duration).count();
            return seconds;
        }

        inline uint64_t AsMilliseconds() const
        {
            const uint64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration).count();
            return milliseconds;
        }

        inline double AsMillisecondsD() const
        {
            const double milliseconds = std::chrono::duration<double, std::milli>(m_Duration).count();
            return milliseconds;
        }

        inline float AsMillisecondsF() const
        {
            const float milliseconds = std::chrono::duration<float, std::milli>(m_Duration).count();
            return milliseconds;
        }

        inline uint64_t AsMicroseconds() const
        {
            const uint64_t microseconds = std::chrono::duration_cast<std::chrono::microseconds>(m_Duration).count();
            return microseconds;
        }
    private:
        std::chrono::high_resolution_clock::duration m_Duration;
    };

    class Stopwatch
    {
    public:
        Stopwatch() = default;
        ~Stopwatch() = default;

        inline static Stopwatch StartNew()
        {
            auto sw = Stopwatch();
            sw.Start();
            return sw;
        }

        inline void Start()
        {
            m_Start = std::chrono::high_resolution_clock::now();
            m_Running = true;
        }

        inline ElapsedTime Stop()
        {
            m_End = std::chrono::high_resolution_clock::now();
            m_Running = false;
            return {m_End - m_Start};
        }

        inline ElapsedTime Restart()
        {
            m_End = std::chrono::high_resolution_clock::now();
            const auto elapsed = m_End - m_Start;
            m_Start = std::chrono::high_resolution_clock::now();
            m_Running = true;
            return {elapsed};
        }

        inline ElapsedTime GetElapsed() const
        {
            if (m_Running)
                return {std::chrono::high_resolution_clock::now() - m_Start};
            else
                return {m_End - m_Start};
        }
    private:
        bool m_Running = false;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_End;
    };
}
