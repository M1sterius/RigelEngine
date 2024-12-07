#pragma once

#include <cstdint>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint64 unique_id;

static_assert(sizeof(float) == 4, "Rigel engine does not support systems with float type size not equal to 32 bits");
static_assert(sizeof(double) == 8, "Rigel engine does not support systems with double type size not equal to 64 bits");

typedef float float32;
typedef double float64;
