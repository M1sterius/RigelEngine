#include "ComponentTypeRegistry.hpp"

namespace rge
{
    Component* ComponentTypeRegistry::FindType(const std::string& type)
    {
        for (const auto& [name, func] : GetRegistry())
        {
            if (name == type)
                return func();
        }

        return nullptr;
    }
}
