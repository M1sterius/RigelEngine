#pragma once

#include "Core.hpp"
#include "Stopwatch.hpp"

#include <memory>
#include <filesystem>

namespace rge
{
    class SceneManager;
    class Renderer;
    class Logger;
    class EventManager;
    class AssetManager;

    class Engine
    {
    public:
        Engine(const Engine& other) = delete;
        Engine& operator = (const Engine&) = delete;
        ~Engine();

        static std::unique_ptr<Engine> CreateInstance();

        NODISCARD bool IsRunning() const { return m_Running; }

        NODISCARD AssetManager& GetAssetManager() const;
        NODISCARD SceneManager& GetSceneManager() const;
        NODISCARD Renderer& GetRenderer() const;
//        NODISCARD Logger& GetLogger() const;

        NODISCARD inline std::filesystem::path GetWorkingDirectory() const { return m_WorkingDirectory; }

        void Run();
    INTERNAL:
        // Returns the global engine core instance, for internal use only
        NODISCARD inline static Engine& Get() { return *m_GlobalInstance; }

        NODISCARD inline float64_t GetGlobalTime() const { return m_GlobalTimeStopwatch.GetElapsed().AsSeconds(); }
        NODISCARD inline float64_t GetDeltaTime() const { return m_DeltaTime; }
        NODISCARD inline uint64_t GetFrameCount() const { return m_FrameCounter; }

        uint64_t TargetFps = 240;
    private:
        Engine();

        void Startup();
        void Shutdown();

        void EngineUpdate();
        void GameUpdate() const;

        bool m_Running = false;
        Stopwatch m_GlobalTimeStopwatch;
        Stopwatch m_DeltaTimeStopwatch;
        float64_t m_DeltaTime = 1.0 / static_cast<float64_t>(TargetFps);
        uint64_t m_FrameCounter = 0;

        std::filesystem::path m_WorkingDirectory;

        // Subsystem instances
        std::unique_ptr<EventManager> m_EventManager;
        std::unique_ptr<AssetManager> m_AssetManager;
        std::unique_ptr<SceneManager> m_SceneManager;
        std::unique_ptr<Renderer> m_Renderer;

        static Engine* m_GlobalInstance;
    };
}
