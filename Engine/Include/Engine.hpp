#pragma once

#include "Core.hpp"
#include "Stopwatch.hpp"
#include "ProjectSettings.hpp"

#include <memory>
#include <filesystem>

namespace Rigel
{
    namespace Backend::Editor
    {
        class Editor;
    }

    class Time;
    class SceneManager;
    class Renderer;
    class EventManager;
    class AssetManager;
    class WindowManager;
    class InputManager;
    class PhysicsEngine;

    class ThreadPool;

    class Engine final
    {
    public:
        ~Engine();

        Engine(const Engine& other) = delete;
        Engine& operator = (const Engine&) = delete;

        /**
        * Creates a single, global instance of Rigel engine.
        */
        static std::unique_ptr<Engine> CreateInstance();

        /**
         * Initializes Rigel engine, must be called right after the instance of the engine is created
         * @param settings Project settings
         * @return 0 for success, any other value for failure
         */
        int32_t Startup(const ProjectSettings& settings);

        NODISCARD inline static Engine& Get()
        {
            ASSERT(s_Instance, "Attempted to retrieve Rigel engine instance before it has been instantiated!");
            return *s_Instance;
        }

        NODISCARD bool Running() const { return m_Running; }

        NODISCARD Time& GetTime() const;
        NODISCARD AssetManager& GetAssetManager() const;
        NODISCARD SceneManager& GetSceneManager() const;
        NODISCARD Renderer& GetRenderer() const;
        NODISCARD EventManager& GetEventManager() const;
        NODISCARD WindowManager& GetWindowManager() const;
        NODISCARD InputManager& GetInputManager() const;
        NODISCARD PhysicsEngine& GetPhysicsEngine() const;

        void Run();
    INTERNAL:
        NODISCARD inline ThreadPool& GetThreadPool() const { return *m_ThreadPool; }
    private:
        Engine();
        void Shutdown();
        void EngineUpdate() const;

        bool m_Initialized = false;
        bool m_Running = false;

        ProjectSettings m_ProjectSettings;

        std::unique_ptr<ThreadPool> m_ThreadPool;
        std::unique_ptr<Backend::Editor::Editor> m_Editor; // this is probably temporary

        // Subsystem instances
        std::unique_ptr<Time> m_Time;
        std::unique_ptr<EventManager> m_EventManager;
        std::unique_ptr<AssetManager> m_AssetManager;
        std::unique_ptr<SceneManager> m_SceneManager;
        std::unique_ptr<WindowManager> m_WindowManager;
        std::unique_ptr<InputManager> m_InputManager;
        std::unique_ptr<Renderer> m_Renderer;
        std::unique_ptr<PhysicsEngine> m_PhysicsEngine;

        inline static Engine* s_Instance = nullptr;

        friend class Time;
    };
}
