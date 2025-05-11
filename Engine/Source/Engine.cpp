#include "Engine.hpp"

#include "InputManager.hpp"
#include "SceneManager.hpp"
#include "EventManager.hpp"
#include "EngineEvents.hpp"
#include "AssetManager.hpp"
#include "WindowManager.hpp"
#include "Renderer.hpp"
#include "PhysicsEngine.hpp"

#include "ThreadPool.hpp"
#include "InternalEvents.hpp"
#include "SleepUtility.hpp"
#include "Debug.hpp"
#include "Time.hpp"
#include "Directory.hpp"
#include "Editor.hpp"

/*
 * Helps avoid boilerplate when writing a dozen of identical getters for engine subsystems.
 * Use it only for that and nothing else!!!
 */
#define DEFINE_SUBSYSTEM_GETTER(Subsystem) \
    Subsystem& Engine::Get##Subsystem() const \
    { \
        ASSERT(m_##Subsystem, "Attempted to retrieve a Rigel::"#Subsystem" instance before it has been initialized"); \
        return *m_##Subsystem; \
    }

namespace Rigel
{
#pragma region SubsystemGetters
    DEFINE_SUBSYSTEM_GETTER(EventManager)
    DEFINE_SUBSYSTEM_GETTER(AssetManager)
    DEFINE_SUBSYSTEM_GETTER(SceneManager)
    DEFINE_SUBSYSTEM_GETTER(WindowManager)
    DEFINE_SUBSYSTEM_GETTER(InputManager)
    DEFINE_SUBSYSTEM_GETTER(Renderer)
    DEFINE_SUBSYSTEM_GETTER(PhysicsEngine)
#pragma endregion

    Engine::Engine() = default;
    Engine::~Engine()
    {
        Shutdown();
        s_Instance = nullptr; // Reset the global instance so that a new one can be properly created
    }

    std::unique_ptr<Engine> Engine::CreateInstance(const ProjectSettings& settings)
    {
        ASSERT(s_Instance == nullptr, "Only a single instance of RigelEngine core class is allowed!")
        Debug::Trace("Creating Rigel engine instance.");

        const auto instance = new Engine();
        ASSERT(instance != nullptr, "Failed to create RigelEngine instance!");
        s_Instance = instance;
        s_Instance->m_ProjectSettings = settings;
        instance->Startup();

        return std::unique_ptr<Engine>(instance);
    }

    void Engine::Startup()
    {
        Debug::Trace("Starting up Rigel engine.");
        Debug::Trace("Engine working directory: {}", Directory::WorkingDirectory().string());
        Debug::Trace("Starting up subsystems:");

        // Create subsystem instances
        m_TimeManager = std::make_unique<Time>();
        m_AssetManager = std::make_unique<AssetManager>();
        m_EventManager = std::make_unique<EventManager>();
        m_SceneManager = std::make_unique<SceneManager>();
        m_WindowManager = std::make_unique<WindowManager>();
        m_InputManager = std::make_unique<InputManager>();
        m_Renderer = std::make_unique<Renderer>();
        m_PhysicsEngine = std::make_unique<PhysicsEngine>();

        // Start up subsystems
        m_TimeManager->Startup();
        m_AssetManager->Startup();
        m_EventManager->Startup();
        m_SceneManager->Startup();
        m_WindowManager->Startup();
        m_InputManager->Startup();
        m_Renderer->Startup();
        m_PhysicsEngine->Startup();

        // Additional subsystem initialization methods
        m_Renderer->LateInit();

        m_ThreadPool = std::make_unique<ThreadPool>();
        Debug::Trace("Creating global thread pool with {} threads.", m_ThreadPool->GetSize());

        m_Editor = std::make_unique<Backend::Editor::Editor>();

        m_Running = true;
        m_GlobalTimeStopwatch.Start();

        Debug::Trace("Rigel engine initialization complete.");
    }

    void Engine::Shutdown()
    {
        Debug::Trace("Shutting down Rigel engine.");

        // Additional logic needed to shut down subsystems
        m_Renderer->WaitForFinish();
        m_SceneManager->UnloadCurrentScene();
        m_AssetManager->UnloadAllAssets();

        // Shut down subsystems
        m_PhysicsEngine->Shutdown();
        m_Renderer->Shutdown();
        m_InputManager->Shutdown();
        m_WindowManager->Shutdown();
        m_SceneManager->Shutdown();
        m_EventManager->Shutdown();
        m_AssetManager->Shutdown();
        m_TimeManager->Shutdown();

        m_GlobalTimeStopwatch.Stop();
    }

    void Engine::Run()
    {
        auto fpsLimitStopwatch = Stopwatch();
        m_DeltaTimeStopwatch.Start();

        if (!m_SceneManager->IsSceneLoaded())
            Debug::Warning("No scene is loaded before the engine enters the game loop!");

        Debug::Trace("Entering the game loop. Target FPS: {}", Time::GetTargetFPS());

        while (Running())
        {
            fpsLimitStopwatch.Start();
            EngineUpdate();
            SleepUtility::LimitFPS(fpsLimitStopwatch.Stop(), Time::GetTargetFPS());

            m_DeltaTime = m_DeltaTimeStopwatch.Restart().AsSeconds();
            m_FrameCounter++;
        }
    }

    void Engine::EngineUpdate()
    {
        m_Editor->Draw();

        m_WindowManager->PollGLFWEvents();
        m_PhysicsEngine->Tick();
        m_EventManager->Dispatch(GameUpdateEvent(Time::GetDeltaTime(), Time::GetFrameCount()));
        // m_EventManager->DispatchThreaded(Backend::TransformUpdateEvent(), *m_ThreadPool, m_ThreadPool->GetSize());
        m_EventManager->Dispatch(Backend::TransformUpdateEvent());
        m_Renderer->Prepare();
        m_Renderer->Render();

        m_InputManager->ResetInputState();
        m_EventManager->Dispatch(Backend::EndOfFrameEvent());

        // for now the only condition for the engine to keep running is the window not being closed.
        m_Running = !m_WindowManager->WindowShouldClose();
    }
}
