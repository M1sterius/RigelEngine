#include "ComponentTypeRegistry.hpp"

namespace rge
{
    std::unordered_map<std::string, std::function<Component*()>> ComponentTypeRegistry::m_Registry = { };

    bool ComponentTypeRegistry::ResisterComponent(const std::string& typeName, const RegisterTypeFunc& func)
    {
        m_Registry[typeName] = func;
        return true;
    }

    Component* ComponentTypeRegistry::FindType(const std::string& type)
    {
        for (const auto& [name, func] : m_Registry)
        {
            if (name == type)
                return func();
        }

        return nullptr;
    }
}
