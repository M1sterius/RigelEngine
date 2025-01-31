#include "UIDGenerator.hpp"

namespace rge
{
    uid_t UIDGenerator::LastUID = LAST_RESERVED_UID;

    uid_t UIDGenerator::Generate()
    {
        return LastUID++;
    }
}
