#pragma once

#include "NODISCARD.hpp"

#include <memory>

namespace rge
{
    class Core
    {
    public:
        Core(const Core& other) = delete;
        Core& operator = (const Core&) = delete;

        static void Startup();
        static void Shutdown();

        NODISCARD inline static Core* Get() { return m_Instance; }
        NODISCARD inline static bool IsInitialized() { return m_Instance != nullptr; }

        void Run();
        void EngineUpdate();
        NODISCARD bool IsRunning() const { return m_IsRunning; }
    private:
        Core() = default;
        ~Core() = default;

        static Core* m_Instance;

        bool m_IsRunning = false;

        void InternalStartup();
        void InternalShutdown();
    };
}
