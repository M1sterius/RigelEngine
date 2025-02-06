#include "ComponentTypeRegistry.hpp"

namespace rge
{
    std::unordered_map<std::string, std::function<Component*()>> ComponentTypeRegistry::m_Registry = { };

    bool ComponentTypeRegistry::ResisterComponent(const std::string& typeName, const RegisterTypeFunc& func)
    {
        m_Registry[typeName] = func;
        return true;
    }
}
