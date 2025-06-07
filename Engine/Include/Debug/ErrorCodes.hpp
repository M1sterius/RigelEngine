#pragma once

#include <cstdint>

namespace Rigel
{
    enum class ErrorCode : int32_t
    {
        NONE = 0,
        UNKNOWN = 1,

        // Subsystems
        SUBSYSTEM_STARTUP_FAILURE = 11,
        SUBSYSTEM_SHUTDOWN_FAILURE = 21
    };
}