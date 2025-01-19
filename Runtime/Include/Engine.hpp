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
        ~Engine() = default;

        static RGE_API std::unique_ptr<Engine> CreateInstance();
        NODISCARD inline static bool IsInitialized() { return (m_GlobalInstance != nullptr); }
        NODISCARD bool IsRunning() const { return m_IsRunning; }

        NODISCARD SceneManager& GetSceneManager() const;

        void Startup();
        void Run();
        void Shutdown();
    INTERNAL:
        // Returns the global engine core instance, only for internal engine use
        NODISCARD inline static Engine* Get() { return m_GlobalInstance; }
    private:
        Engine() = default;

        bool m_IsRunning = false;

        void EngineUpdate();
        void GameUpdate() const;

        // Subsystem Instances
        SceneManager* m_SceneManager = nullptr;

        static RGE_API Engine* m_GlobalInstance;
    };
}