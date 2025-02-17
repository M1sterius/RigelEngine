#include "Engine.hpp"

#include <Time.hpp>

#include "SceneManager.hpp"
#include "EventManager.hpp"
#include "EngineEvents.hpp"
#include "AssetManager.hpp"
#include "WindowManager.hpp"
#include "Renderer.hpp"
#include "SleepUtility.hpp"
#include "Logger.hpp"
#include "Assert.hpp"
#include "Debug.hpp"

namespace rge
{
    Engine* Engine::m_GlobalInstance = nullptr;

    Engine::Engine() = default;
    Engine::~Engine() { Shutdown(); }

    std::unique_ptr<Engine> Engine::CreateInstance()
    {
        ASSERT(m_GlobalInstance == nullptr, "Only a single instance of RigelEngine core class is allowed!")
        VERBOSE_MESSAGE("Creating Rigel engine instance.");
        const auto instance = new Engine();
        ASSERT(instance != nullptr, "Failed to create RigelEngine instance!");
        m_GlobalInstance = instance;
        instance->Startup();
        return std::unique_ptr<Engine>(instance);
    }

    void Engine::Startup()
    {
        VERBOSE_MESSAGE("Starting up Rigel engine.");
        m_WorkingDirectory = std::filesystem::current_path();
        VERBOSE_MESSAGE("Engine working directory: " + m_WorkingDirectory.string());
        VERBOSE_MESSAGE("Starting up subsystems:");

        // Instantiate and start up all subsystems
        m_AssetManager = std::make_unique<AssetManager>();
        m_EventManager = std::make_unique<EventManager>();
        m_SceneManager = std::make_unique<SceneManager>();
        m_WindowManager = std::make_unique<WindowManager>();
        m_Renderer = std::make_unique<Renderer>();

        m_Running = true;
        m_GlobalTimeStopwatch.Start();

        VERBOSE_MESSAGE("Rigel engine initialization completed.");
    }

    void Engine::Shutdown()
    {
        // Shut down all subsystems and global tools
        m_Renderer.reset();
        m_WindowManager.reset();
        m_SceneManager.reset();
        m_EventManager.reset();
        m_AssetManager.reset();

        m_GlobalTimeStopwatch.Stop();
    }

    EventManager& Engine::GetEventManager() const
    {
        ASSERT(m_EventManager, "Attempted to retrieve a rge::EventManager instance before it has been initialized.")
        return *m_EventManager;
    }

    AssetManager& Engine::GetAssetManager() const
    {
        ASSERT(m_AssetManager, "Attempted to retrieve a rge::AssetManager instance before it has been initialized.")
        return *m_AssetManager;
    }

    SceneManager& Engine::GetSceneManager() const
    {
        ASSERT(m_SceneManager, "Attempted to retrieve a rge::SceneManager instance before it has been initialized.")
        return *m_SceneManager;
    }

    Renderer& Engine::GetRenderer() const
    {
        ASSERT(m_Renderer, "Attempted to retrieve a rge::Renderer instance before it has been initialized.");
        return *m_Renderer;
    }

    void Engine::Run()
    {
        auto fpsLimitStopwatch = Stopwatch();
        m_DeltaTimeStopwatch.Start();

        VERBOSE_MESSAGE("Entering game loop. Target FPS: " + std::to_string(TargetFps));

        while (IsRunning())
        {
            fpsLimitStopwatch.Start();

            EngineUpdate();

            LimitFPS(fpsLimitStopwatch.Stop().AsSeconds(), TargetFps);

            m_DeltaTime = m_DeltaTimeStopwatch.Restart().AsSeconds();
            m_FrameCounter++;
        }
    }

    void Engine::EngineUpdate()
    {
        // Poll and process events
        // Update input state
        // Game update
        // Transform update
        // Pre-render update
        // Scene render
        // Gizmo render
        // GUI render

        m_EventManager->Dispatch<GameUpdateEvent>(GameUpdateEvent(Time::GetDeltaTime(), Time::GetFrameCount()));
    }
}
