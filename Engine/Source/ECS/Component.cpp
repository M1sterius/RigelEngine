#include "Component.hpp"
#include "UIDGenerator.hpp"

namespace rge
{
    Component::Component()
    {
        m_ID = UIDGenerator::Generate();
    }
}
