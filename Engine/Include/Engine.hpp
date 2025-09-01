#pragma once

#include "Core.hpp"
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
    class AssetManager;
    class SceneManager;
    class Renderer;
    class EventManager;
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
        ErrorCode Startup(const ProjectSettings& settings);

        void Shutdown();

        NODISCARD static Ref<Engine> Get();

        NODISCARD Ref<Time> GetTime() const;
        NODISCARD Ref<AssetManager> GetAssetManager() const;
        NODISCARD Ref<SceneManager> GetSceneManager() const;
        NODISCARD Ref<Renderer> GetRenderer() const;
        NODISCARD Ref<EventManager> GetEventManager() const;
        NODISCARD Ref<WindowManager> GetWindowManager() const;
        NODISCARD Ref<InputManager> GetInputManager() const;
        NODISCARD Ref<PhysicsEngine> GetPhysicsEngine() const;

        NODISCARD bool Running() const { return m_Running; }

        void Run();
    private:
        Engine();
        void EngineUpdate() const;

        void DrawDebugGUI();

        ProjectSettings m_ProjectSettings;
        bool m_Initialized = false;
        bool m_Running = false;

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
