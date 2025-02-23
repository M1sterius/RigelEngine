#pragma once

#include "Core.hpp"
#include "Stopwatch.hpp"

#include <memory>
#include <filesystem>

namespace rge
{
    class SceneManager;
    class Renderer;
    class EventManager;
    class AssetManager;
    class WindowManager;
    class InputManager;

    struct EngineConfigInfo
    {
        uint64_t TargetFPS = 240;
        uint64_t TargetTickrate = 50;

        uint64_t MaxThreads = 0;
    };

    class Engine final
    {
    public:
        Engine(const Engine& other) = delete;
        Engine& operator = (const Engine&) = delete;
        ~Engine();

        static std::unique_ptr<Engine> CreateInstance();

        NODISCARD bool IsRunning() const { return m_Running; }

        NODISCARD inline static Engine& Get() { return *m_GlobalInstance; }

        NODISCARD AssetManager& GetAssetManager() const;
        NODISCARD SceneManager& GetSceneManager() const;
        NODISCARD Renderer& GetRenderer() const;
        NODISCARD EventManager& GetEventManager() const;
        NODISCARD WindowManager& GetWindowManager() const;
        NODISCARD InputManager& GetInputManager() const;

        NODISCARD inline std::filesystem::path GetWorkingDirectory() const { return m_WorkingDirectory; }

        void Run();
    private:
        Engine();

        void Startup();
        void Shutdown();

        void EngineUpdate();

        bool m_Running = false;
        Stopwatch m_GlobalTimeStopwatch;
        Stopwatch m_DeltaTimeStopwatch;
        uint64_t m_TargetFps = 240;
        float64_t m_DeltaTime = 1.0 / static_cast<float64_t>(m_TargetFps);
        float64_t m_PhysicsTickTime = 0.05;
        uint64_t m_FrameCounter = 0;

        std::filesystem::path m_WorkingDirectory;

        // Subsystem instances
        std::unique_ptr<EventManager> m_EventManager;
        std::unique_ptr<AssetManager> m_AssetManager;
        std::unique_ptr<SceneManager> m_SceneManager;
        std::unique_ptr<WindowManager> m_WindowManager;
        std::unique_ptr<InputManager> m_InputManager;
        std::unique_ptr<Renderer> m_Renderer;

        static Engine* m_GlobalInstance;

        friend class Time;
    };
}
