#pragma once

#include "Core.hpp"

#include <chrono>
#include <cstdint>

namespace Rigel
{
    struct ElapsedTime
    {
    public:
        ElapsedTime() noexcept
            : m_Duration(std::chrono::high_resolution_clock::duration::zero())
        {
        }

        explicit ElapsedTime(const double seconds) noexcept
            : m_Duration(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                  std::chrono::duration<double>(seconds)))
        {
        }

        explicit ElapsedTime(const std::chrono::milliseconds milliseconds) noexcept
            : m_Duration(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(milliseconds))
        {
        }

        explicit ElapsedTime(const std::chrono::high_resolution_clock::duration& duration) noexcept
            : m_Duration(duration)
        {
        }

        ElapsedTime(const ElapsedTime& other) noexcept = default;
        ElapsedTime(ElapsedTime&& other) noexcept = default;
        ElapsedTime& operator=(const ElapsedTime& other) noexcept = default;
        ElapsedTime& operator=(ElapsedTime&& other) noexcept = default;

        ~ElapsedTime() = default;

        inline ElapsedTime operator + (const ElapsedTime& other) const noexcept
        {
            return ElapsedTime(m_Duration + other.m_Duration);
        }

        // Addition assignment operator
        ElapsedTime& operator += (const ElapsedTime& other) noexcept
        {
            m_Duration += other.m_Duration;
            return *this;
        }

        // Subtraction operator
        ElapsedTime operator - (const ElapsedTime& other) const noexcept
        {
            return ElapsedTime(m_Duration - other.m_Duration);
        }

        // Subtraction assignment operator
        ElapsedTime& operator -= (const ElapsedTime& other) noexcept
        {
            m_Duration -= other.m_Duration;
            return *this;
        }

        // Multiplication operator with scalar
        template<std::floating_point T>
        ElapsedTime operator * (const T scalar) const noexcept
        {
            return ElapsedTime(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::duration<T>(AsSeconds() * scalar)));
        }

        // Multiplication assignment operator with scalar
        template<std::floating_point T>
        ElapsedTime& operator *= (const T scalar) noexcept
        {
            m_Duration = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::duration<T>(AsSeconds() * scalar));
            return *this;
        }

        // Division operator with scalar
        template<std::floating_point T>
        ElapsedTime operator / (const T scalar) const
        {
            if (std::abs(scalar) < std::numeric_limits<T>::epsilon())
            {
                throw std::invalid_argument("Division by zero or near-zero value");
            }
            return ElapsedTime(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::duration<T>(AsSeconds() / scalar)));
        }

        // Division assignment operator with scalar
        template<std::floating_point T>
        ElapsedTime& operator /= (const T scalar)
        {
            if (std::abs(scalar) < std::numeric_limits<T>::epsilon())
            {
                throw std::invalid_argument("Division by zero or near-zero value");
            }
            m_Duration = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::duration<T>(AsSeconds() / scalar));
            return *this;
        }

        // Division operator with another ElapsedTime (returns a scalar)
        double operator / (const ElapsedTime& other) const
        {
            if (other.m_Duration.count() == 0)
            {
                throw std::invalid_argument("Division by zero duration");
            }
            return static_cast<double>(AsSeconds()) / static_cast<double>(other.AsSeconds());
        }

        bool operator == (const ElapsedTime& other) const noexcept
        {
            return m_Duration == other.m_Duration;
        }

        bool operator != (const ElapsedTime& other) const noexcept
        {
            return m_Duration != other.m_Duration;
        }

        bool operator < (const ElapsedTime& other) const noexcept
        {
            return m_Duration < other.m_Duration;
        }

        bool operator <= (const ElapsedTime& other) const noexcept
        {
            return m_Duration <= other.m_Duration;
        }

        bool operator > (const ElapsedTime& other) const noexcept
        {
            return m_Duration > other.m_Duration;
        }

        bool operator >= (const ElapsedTime& other) const noexcept
        {
            return m_Duration >= other.m_Duration;
        }

        // Conversion methods
        NODISCARD inline std::chrono::high_resolution_clock::duration DurationRaw() const noexcept
        {
            return m_Duration;
        }

        NODISCARD inline float64_t AsSeconds() const noexcept
        {
            const float64_t seconds = std::chrono::duration<float64_t>(m_Duration).count();
            return seconds;
        }

        NODISCARD inline float32_t AsSecondsF() const noexcept
        {
            const float32_t seconds = std::chrono::duration<float32_t>(m_Duration).count();
            return seconds;
        }

        NODISCARD inline uint64_t AsMilliseconds() const noexcept
        {
            const uint64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration).count();
            return milliseconds;
        }

        NODISCARD inline float64_t AsMillisecondsD() const noexcept
        {
            const float64_t milliseconds = std::chrono::duration<float64_t, std::milli>(m_Duration).count();
            return milliseconds;
        }

        NODISCARD inline float32_t AsMillisecondsF() const noexcept
        {
            const float32_t milliseconds = std::chrono::duration<float32_t, std::milli>(m_Duration).count();
            return milliseconds;
        }

        NODISCARD inline uint64_t AsMicroseconds() const noexcept
        {
            const uint64_t microseconds = std::chrono::duration_cast<std::chrono::microseconds>(m_Duration).count();
            return microseconds;
        }

        NODISCARD inline float64_t AsMicrosecondsD() const noexcept
        {
            const float64_t microseconds = std::chrono::duration<float64_t, std::micro>(m_Duration).count();
            return microseconds;
        }

        NODISCARD static inline ElapsedTime FromSeconds(float64_t seconds) noexcept
        {
            return ElapsedTime(seconds);
        }

        NODISCARD static inline ElapsedTime FromMilliseconds(uint64_t milliseconds) noexcept
        {
            return ElapsedTime(std::chrono::milliseconds(milliseconds));
        }

        NODISCARD static inline ElapsedTime FromMicroseconds(uint64_t microseconds) noexcept
        {
            return ElapsedTime(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::microseconds(microseconds)));
        }

        NODISCARD static inline ElapsedTime Zero() noexcept
        {
            return ElapsedTime();
        }

        // Stream insertion operator
        friend std::ostream& operator<<(std::ostream& os, const ElapsedTime& time)
        {
            os << time.AsSeconds() << "s";
            return os;
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
            return ElapsedTime( m_End - m_Start);
        }

        inline ElapsedTime Restart()
        {
            m_End = std::chrono::high_resolution_clock::now();
            const auto elapsed = m_End - m_Start;
            m_Start = std::chrono::high_resolution_clock::now();
            m_Running = true;
            return ElapsedTime(elapsed);
        }

        NODISCARD inline ElapsedTime GetElapsed() const
        {
            if (m_Running)
                return ElapsedTime(std::chrono::high_resolution_clock::now() - m_Start);
            else
                return ElapsedTime(m_End - m_Start);
        }
    private:
        bool m_Running = false;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_End;
    };
}
