#pragma once

#include "NODISCARD.hpp"

namespace rge
{
    class Filesystem;

    class Core
    {
    public:
        Core(const Core& other) = delete;
        Core& operator =(Core&&) = delete;

        static void Startup();
        static void Shutdown();

        NODISCARD inline static Core* Get() { return m_Instance; }
        NODISCARD inline static bool IsInitialized() { return m_Instance != nullptr; }

        NODISCARD inline Filesystem* GetFilesystem() const { return m_Filesystem; }

        void EngineUpdate();
        NODISCARD bool IsRunning() const { return m_IsRunning; }
    private:
        Core() = default;
        ~Core() = default;

        static Core* m_Instance;

        bool m_IsRunning = false;

        Filesystem* m_Filesystem = nullptr;
    };
}
