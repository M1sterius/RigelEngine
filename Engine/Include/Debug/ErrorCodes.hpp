#pragma once

#include <cstdint>

namespace Rigel
{
    enum class ErrorCode : int32_t
    {
        OK = 0,
        UNKNOWN = 1,

        // Subsystems
        SUBSYSTEM_STARTUP_FAILURE = 11,
        SUBSYSTEM_SHUTDOWN_FAILURE = 12,

        RENDERER_BACKEND_START_UP_FAILURE = 31,
        IMGUI_BACKEND_STARTUP_FAILURE = 32,
        RENDERER_LATE_STARTUP_FAILURE = 33,

        // I/O
        FAILED_TO_OPEN_FILE = 101,

        MAX_VALUE = 0x0FFFFFFF
    };
}