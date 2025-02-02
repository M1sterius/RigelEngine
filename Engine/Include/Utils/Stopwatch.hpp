#pragma once

#include "Utils/HeaderUtils/HeaderUtils.hpp"

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

        NODISCARD inline std::chrono::high_resolution_clock::duration DurationRaw() const
        {
            return m_Duration;
        }

        NODISCARD inline float64_t AsSeconds() const
        {
            const float64_t seconds = std::chrono::duration<float64_t>(m_Duration).count();
            return seconds;
        }

        NODISCARD inline float AsSecondsF() const
        {
            const float seconds = std::chrono::duration<float32_t>(m_Duration).count();
            return seconds;
        }

        NODISCARD inline uint64_t AsMilliseconds() const
        {
            const uint64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration).count();
            return milliseconds;
        }

        NODISCARD inline float64_t AsMillisecondsD() const
        {
            const float64_t milliseconds = std::chrono::duration<float64_t, std::milli>(m_Duration).count();
            return milliseconds;
        }

        NODISCARD inline float32_t AsMillisecondsF() const
        {
            const float32_t milliseconds = std::chrono::duration<float32_t, std::milli>(m_Duration).count();
            return milliseconds;
        }

        NODISCARD inline uint64_t AsMicroseconds() const
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

        NODISCARD inline ElapsedTime GetElapsed() const
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
