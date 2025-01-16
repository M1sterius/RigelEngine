#pragma once

#include "Utils/HeaderUtils.hpp"

#include <memory>

namespace rge
{
    class SceneManager;

    class RGE_API Engine
    {
    public:
        Engine(const Engine& other) = delete;
        Engine& operator = (const Engine&) = delete;

        static void Startup();
        static void Shutdown();

        NODISCARD inline static Engine* Get() { return m_Instance; }
        NODISCARD SceneManager& GetSceneManager() const;
        NODISCARD inline static bool IsInitialized() { return (m_Instance != nullptr); }

        void Run();
        NODISCARD bool IsRunning() const { return m_IsRunning; }
    private:
        Engine() = default;
        ~Engine() = default;

        bool m_IsRunning = false;

        void InternalStartup();
        void InternalShutdown();

        void EngineUpdate();

        void GameUpdate() const;

        SceneManager* m_SceneManager = nullptr;
        static RGE_API Engine* m_Instance;
    };
}
