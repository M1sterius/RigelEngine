#include "ComponentTypeRegistry.hpp"
#include "Component.hpp"

namespace Rigel
{
    std::unique_ptr<Component> ComponentTypeRegistry::FindType(const std::string& type)
    {
        for (const auto& [name, func] : GetRegistry())
        {
            if (name == type)
                return std::unique_ptr<Component>(func());
        }

        return nullptr;
    }
}
