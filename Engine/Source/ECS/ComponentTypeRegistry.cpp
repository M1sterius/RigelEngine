#include "ComponentTypeRegistry.hpp"

namespace Rigel
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
