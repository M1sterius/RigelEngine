#include "RigelObject.hpp"
#include "UIDGenerator.hpp"

namespace rge
{
    RigelObject::RigelObject()
    {
        m_ID = UIDGenerator::Generate();
    }
}