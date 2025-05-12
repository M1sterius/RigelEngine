#pragma once

#include "Core.hpp"

#include <mutex>

namespace Rigel
{
    class ProjectSettings;

    class RigelSubsystem
    {
    public:
        RigelSubsystem(const RigelSubsystem&) = delete;
        RigelSubsystem& operator = (const RigelSubsystem&) = delete;

        virtual int32_t Startup(const ProjectSettings& settings) = 0;
        virtual int32_t Shutdown() = 0;

        NODISCARD bool IsInitialized() const { return m_Initialized; }
    protected:
        RigelSubsystem() = default;
        virtual ~RigelSubsystem() = default;

        bool m_Initialized = false;
    };
}
