#pragma once

#include "Types.hpp"
#include "Internal.hpp"
#include "Assert.hpp"
#include "Exceptions.hpp"

#define RIGEL_ENGINE_NAME "RigelEngine"

#define RGE_API_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define RGE_API_VERSION_MAJOR(version) ((uint32_t)(version) >> 22U)
#define RGE_API_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
#define RGE_API_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)

#define RGE_API_CURRENT_VERSION RGE_API_MAKE_VERSION(0, 1, 0)

#define NODISCARD [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]

#define SET_BIT(n) (1 << n)
#define TYPE_EQUAL(T1, T2) (typeid(T1) == typeid(T2))
#define TYPE_INDEX(T) (std::type_index(typeid(T)))
#define TYPE_NAME(T) (typeid(T).name())

#ifdef RIGEL_INTERNAL
    #define RIGEL_API __declspec(dllexport)
#else
    #define RIGEL_API __declspec(dllimport)
#endif
