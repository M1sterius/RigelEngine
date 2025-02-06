#pragma once

#include "Core.hpp"

namespace rge
{
    class ComponentTypeRegistry final
    {
    public:
        ComponentTypeRegistry(const ComponentTypeRegistry&) = delete;
        ComponentTypeRegistry operator = (const ComponentTypeRegistry&) = delete;
    };
}