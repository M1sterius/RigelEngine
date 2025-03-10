#pragma once

// Standard integer types
#include <cstdint>

// Unique ID type
#ifdef RGE_USE_64_BIT_ID_TYPE
    typedef uint64_t uid_t;
#else
    typedef uint32_t uid_t;
#endif

#define NULL_ID 0

static_assert(sizeof(float) == 4, "Rigel engine does not support systems with sizeof(float) not equal to 32 bits");
static_assert(sizeof(double) == 8, "Rigel engine does not support systems with sizeof(double) not equal to 64 bits");

typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;
