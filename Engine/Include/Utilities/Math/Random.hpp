#pragma once

#include <random>

namespace Rigel
{
    class Random
    {
    public:
        /**
         * Generates a random numerical value using std::mt19937_64 generator
         */
        template<typename T>
        static T Numeric(T min, T max)
        {
            thread_local std::random_device rd;
            thread_local std::mt19937_64 generator(rd());

            if (min > max)
                std::swap(min, max);

            if constexpr(std::is_floating_point_v<T>)
            {
                std::uniform_real_distribution<T> distribution(min, max);
                return distribution(generator);
            }
            else
            {
                std::uniform_int_distribution<T> distribution(min, max);
                return distribution(generator);
            }
        }

        /**
         * Generates random UUIDv4 as a std::string using std::mt19937_64 generator
         */
        static std::string UUIDv4();
    };

    inline std::string get_uuid() {
        static std::random_device dev;
        static std::mt19937 rng(dev());

        std::uniform_int_distribution<int> dist(0, 15);

        const char *v = "0123456789abcdef";
        const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

        std::string res;
        for (int i = 0; i < 16; i++) {
            if (dash[i]) res += "-";
            res += v[dist(rng)];
            res += v[dist(rng)];
        }
        return res;
    }
}
