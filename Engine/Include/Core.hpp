#pragma once

#include "Types.hpp"
#include "Internal.hpp"
#include "Assert.hpp"

#define NODISCARD [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]

#ifdef RIGEL_INTERNAL
    #define RIGEL_API __declspec(dllexport)
#else
    #define RIGEL_API __declspec(dllimport)
#endif
