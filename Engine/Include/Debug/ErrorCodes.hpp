#pragma once

#include <cstdint>

namespace Rigel
{
    enum class ErrorCode : int32_t
    {
        OK = 0,
        UNKNOWN = 1,

        NULL_HANDLE_DEREFERENCE = 2,
        INVALID_HANDLE_DEREFERENCE = 3,

        // Subsystems
        SUBSYSTEM_STARTUP_FAILURE = 11,
        SUBSYSTEM_SHUTDOWN_FAILURE = 12,
        BUILT_IN_ASSET_NOT_LOADED = 14,

        // Renderer
        RENDERER_BACKEND_START_UP_FAILURE = 31,
        IMGUI_BACKEND_STARTUP_FAILURE = 32,
        RENDERER_LATE_STARTUP_FAILURE = 33,

        // I/O
        FAILED_TO_OPEN_FILE = 101,
        NLOHMANN_JSON_PARSING_ERROR = 101,
        NLOHMANN_JSON_READING_ERROR = 102,

        // Assets
        FAILED_TO_LOAD_ASSET = 201,
        FAILED_TO_CREATE_ASSET_BACKEND = 202,
        ASSET_FILE_FORMAT_NOT_SUPPORTED = 203,

        // Vulkan
        VULKAN_UNRECOVERABLE_ERROR = 501,
        VULKAN_BINDLESS_TEXTURE_REGISTRY_OVERFLOW = 502,

        // Window manager
        MONITOR_INFO_ENUMERATION_FAILURE = 701,
        GLFW_INIT_FAILURE = 702,

        MAX_VALUE = 0x0FFFFFFF
    };
}