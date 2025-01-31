#pragma once

#include "Utils/HeaderUtils.hpp"
#include "Utils/Stopwatch.hpp"

#include <memory>

namespace rge
{
    class SceneManager;
    class Renderer;

    class RGE_API Engine
    {
    public:
        Engine(const Engine& other) = delete;
        Engine& operator = (const Engine&) = delete;
        ~Engine() = default;

        static RGE_API std::unique_ptr<Engine> CreateInstance();

        NODISCARD bool IsRunning() const { return m_IsRunning; }

        NODISCARD SceneManager& GetSceneManager() const;
        NODISCARD Renderer& GetRenderer() const;

        void Startup();
        void Run();
        void Shutdown();
    INTERNAL:
        // Returns the global engine core instance, for internal use only
        NODISCARD inline static Engine& Get() { return *m_GlobalInstance; }

        NODISCARD inline float64_t GetGlobalTime() const { return m_GlobalTimeStopwatch.GetElapsed().AsSeconds(); }
        NODISCARD inline float64_t GetDeltaTime() const { return m_DeltaTime; }
        NODISCARD inline uint64_t GetFrameCount() const { return m_FrameCounter; }

        uint64_t TargetFps = 240;
    private:
        Engine() = default;

        bool m_IsRunning = false;
        Stopwatch m_GlobalTimeStopwatch;
        Stopwatch m_DeltaTimeStopwatch;
        float64_t m_DeltaTime = 0.0;
        uint64_t m_FrameCounter = 0;

        void EngineUpdate();
        void GameUpdate() const;

        // Subsystem Instances
        SceneManager* m_SceneManager = nullptr;
        Renderer* m_Renderer = nullptr;

        static RGE_API Engine* m_GlobalInstance;
    };
}