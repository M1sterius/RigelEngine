#pragma once

#include <iostream>
#include <random>
#include <cstdint>

/**
 * @brief Generates a random 64-bit unsigned integer in the specified min, max range
 * @param min The lower bound
 * @param max The upper bound
 * @return A random 64-bit unsigned integer
 */
inline uint64_t GenerateRandomUint64(const uint64_t min, const uint64_t max)
{
    if (min > max)
        throw std::invalid_argument("Min cannot be greater than Max.");

    static std::random_device rd;
    std::mt19937_64 generator(rd());

    std::uniform_int_distribution<uint64_t> distribution(min, max);

    return distribution(generator);
}

/**
 * @brief Generates a random 64-bit signed integer in the specified min, max range
 * @param min The lower bound
 * @param max The upper bound
 * @return A random 64-bit signed integer
 */
inline int64_t GenerateRandomInt64(const int64_t min, const int64_t max)
{
    if (min > max)
        throw std::invalid_argument("Min cannot be greater than Max.");

    static std::random_device rd;
    std::mt19937_64 generator(rd());

    std::uniform_int_distribution<int64_t> distribution(min, max);

    return distribution(generator);
}

/**
 * @brief Generates a random 64-bit floating point number in the specified min, max range
 * @param min The lower bound
 * @param max The upper bound
 * @return A random 64-bit floating point number
 */
inline double GenerateRandomDouble(const double min, const double max)
{
    if (min > max)
        throw std::invalid_argument("Min cannot be greater than Max.");

    static std::random_device rd;
    std::mt19937_64 generator(rd());

    std::uniform_real_distribution<double> distribution(min, max);

    return distribution(generator);
}

/**
 * @brief Generates a random 32-bit floating point number in the specified min, max range
 * @param min The lower bound
 * @param max The upper bound
 * @return A random 32-bit floating point number
 */
inline float GenerateRandomFloat(const float min, const float max)
{
    if (min > max)
        throw std::invalid_argument("Min cannot be greater than Max.");

    static std::random_device rd;
    std::mt19937_64 generator(rd());

    std::uniform_real_distribution<float> distribution(min, max);

    return distribution(generator);
}
