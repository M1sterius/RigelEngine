#pragma once

#include "Core.hpp"

namespace rge
{
    class UIDGenerator
    {
    public:
        static uid_t Generate();

        static constexpr uid_t LAST_RESERVED_UID = 1;
    private:
        static uid_t LastUID;
    };
}
