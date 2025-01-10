#pragma once

#include "NODISCARD.hpp"

#include <memory>

namespace rge
{
    class SceneManager;

    class Core
    {
    public:
        Core(const Core& other) = delete;
        Core& operator = (const Core&) = delete;

        static void Startup();
        static void Shutdown();

        NODISCARD inline static Core* Get() { return m_Instance; }
        NODISCARD inline static bool IsInitialized() { return (m_Instance != nullptr); }

        NODISCARD inline SceneManager* GetSceneManager() const { return m_SceneManager; }

        void Run();
        NODISCARD bool IsRunning() const { return m_IsRunning; }
    private:
        Core() = default;
        ~Core() = default;

        bool m_IsRunning = false;

        void InternalStartup();
        void InternalShutdown();

        void EngineUpdate();

        SceneManager* m_SceneManager = nullptr;

        static Core* m_Instance;
    };
}
