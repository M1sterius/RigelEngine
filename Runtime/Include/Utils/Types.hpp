#pragma once

// Standart integer types
#include <cstdint>

// Unique ID type
typedef uint64_t uid_t;

static_assert(sizeof(float) == 4, "Rigel engine does not support systems with sizeof(float) not equal to 32 bits");
static_assert(sizeof(double) == 8, "Rigel engine does not support systems with sizeof(double) not equal to 64 bits");

typedef float float32_t;
typedef double float64_t;
