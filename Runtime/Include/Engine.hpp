#pragma once

#include "Utils/NODISCARD.hpp"

#include <memory>

namespace rge
{
    class SceneManager;

    class Engine
    {
    public:
        Engine(const Engine& other) = delete;
        Engine& operator = (const Engine&) = delete;

        static void Startup();
        static void Shutdown();

        NODISCARD inline static Engine* Get() { return m_Instance; }
        NODISCARD inline static bool IsInitialized() { return (m_Instance != nullptr); }

        NODISCARD inline SceneManager* GetSceneManager() const { return m_SceneManager; }

        void Run();
        NODISCARD bool IsRunning() const { return m_IsRunning; }
    private:
        Engine() = default;
        ~Engine() = default;

        bool m_IsRunning = false;

        void InternalStartup();
        void InternalShutdown();

        void EngineUpdate();

        SceneManager* m_SceneManager = nullptr;

        static Engine* m_Instance;
    };
}
