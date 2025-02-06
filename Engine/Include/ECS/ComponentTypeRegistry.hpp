#pragma once

#include "Core.hpp"

#include <unordered_map>
#include <functional>
#include <string>

namespace rge
{
    class Component;

    class ComponentTypeRegistry final
    {
    public:
        using RegisterTypeFunc = std::function<Component*()>;

        ComponentTypeRegistry(const ComponentTypeRegistry&) = delete;
        ComponentTypeRegistry operator = (const ComponentTypeRegistry&) = delete;

        static bool ResisterComponent(const std::string& typeName, const RegisterTypeFunc& func);
    INTERNAL:
    private:
        static std::unordered_map<std::string, RegisterTypeFunc> m_Registry;
    };
}
