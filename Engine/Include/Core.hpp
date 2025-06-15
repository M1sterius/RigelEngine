#pragma once

#include "Types.hpp"
#include "Internal.hpp"
#include "Assert.hpp"
#include "Exceptions.hpp"
#include "ErrorCodes.hpp"
#include "Result.hpp"
#include "TypeUtility.hpp"

#define RIGEL_ENGINE_NAME "RigelEngine"

#define RIGEL_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define RGE_API_CURRENT_VERSION RIGEL_MAKE_VERSION(0, 1, 0)

#define NODISCARD [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]

#define SET_BIT(n) (1 << n)

#define KB(n) (n * 1024)
#define MB(n) (n * 1024 * 1024)
#define GB(n) (n * 1024 * 1024)

#ifdef RIGEL_INTERNAL
    #define RIGEL_API __declspec(dllexport)
#else
    #define RIGEL_API __declspec(dllimport)
#endif
