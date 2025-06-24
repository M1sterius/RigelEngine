#pragma once

// Standard integer types
#include <cstdint>

// Unique ID type
#ifdef RIGEL_USE_64_BIT_ID_TYPE
    typedef uint64_t uid_t;
#else
    typedef uint32_t uid_t;
#endif

#define NULL_ID 0

typedef float float32_t;
typedef double float64_t;

static_assert(sizeof(float32_t) == 4, "sizeof(float32_t) is not equal to 4");
static_assert(sizeof(float64_t) == 8, "sizeof(float64_t) is not equal to 8");
