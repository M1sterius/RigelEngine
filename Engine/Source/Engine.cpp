#include "Engine.hpp"

#include "../Include/Subsystems/InputManager/InputManager.hpp"
#include "SceneManager.hpp"
#include "EventManager.hpp"
#include "EngineEvents.hpp"
#include "AssetManager.hpp"
#include "../Include/Subsystems/WindowManager/WindowManager.hpp"
#include "Renderer.hpp"
#include "PhysicsEngine.hpp"

#include "ThreadPool.hpp"
#include "InternalEvents.hpp"
#include "SleepUtility.hpp"
#include "Debug.hpp"
#include "Time.hpp"
#include "Directory.hpp"
#include "Editor.hpp"

namespace Rigel
{
#pragma region Helpers
    /*
     * Helps to avoid boilerplate when writing a dozen of identical getters for engine subsystems.
     * Use it only for that and nothing else!!!
     */
    #define DEFINE_SUBSYSTEM_GETTER(Subsystem) \
        Subsystem& Engine::Get##Subsystem() const \
        { \
            ASSERT(m_##Subsystem, "Attempted to retrieve a Rigel::"#Subsystem" instance before it has been initialized"); \
            return *m_##Subsystem; \
        }

    template<typename T>
    static bool StartUpSubsystem(const ProjectSettings& settings, const std::unique_ptr<T>& subsystem, const std::string& name)
    {
        if (const auto result = subsystem->Startup(settings); result != 0)
        {
            Debug::Error("Rigel engine startup failed: {} subsystem failed to initialize! Error code: {}.", name, result);
            return false;
        }

        return true;
    }

    template<typename T>
    static void ShutDownSubsystem(const std::unique_ptr<T>& subsystem, const std::string& name)
    {
        if (subsystem->IsInitialized())
        {
            if (const auto result = subsystem->Shutdown(); result != 0)
                Debug::Error("An error occurred during shutdown process of {} subsystem! Error code: {}.", name, result);
        }
    }
#pragma endregion

    Engine::Engine() = default;
    Engine::~Engine()
    {
        Shutdown();
        s_Instance = nullptr; // Reset the global instance so that a new one can be properly created
    }

    DEFINE_SUBSYSTEM_GETTER(Time);
    DEFINE_SUBSYSTEM_GETTER(EventManager)
    DEFINE_SUBSYSTEM_GETTER(AssetManager)
    DEFINE_SUBSYSTEM_GETTER(SceneManager)
    DEFINE_SUBSYSTEM_GETTER(WindowManager)
    DEFINE_SUBSYSTEM_GETTER(InputManager)
    DEFINE_SUBSYSTEM_GETTER(Renderer)
    DEFINE_SUBSYSTEM_GETTER(PhysicsEngine)

    std::unique_ptr<Engine> Engine::CreateInstance()
    {
        ASSERT(!s_Instance, "Only a single instance of RigelEngine core class is allowed!")
        Debug::Trace("Creating Rigel engine instance.");

        const auto instance = new Engine();
        ASSERT(instance, "Failed to create RigelEngine instance!");
        s_Instance = instance;

        return std::unique_ptr<Engine>(instance);
    }

    int32_t Engine::Startup(const ProjectSettings& settings)
    {
        m_ProjectSettings = settings;

        Debug::Trace("Starting up Rigel engine.");
        Debug::Trace("Engine working directory: {}", Directory::WorkingDirectory().string());
        Debug::Trace("Starting up subsystems:");

        // Create subsystem instances, no startup logic in constructors
        m_Time = std::make_unique<Time>();
        m_AssetManager = std::make_unique<AssetManager>();
        m_EventManager = std::make_unique<EventManager>();
        m_SceneManager = std::make_unique<SceneManager>();
        m_WindowManager = std::make_unique<WindowManager>();
        m_InputManager = std::make_unique<InputManager>();
        m_Renderer = std::make_unique<Renderer>();
        m_PhysicsEngine = std::make_unique<PhysicsEngine>();

        // Startup order matters A LOT!
        if (!StartUpSubsystem(m_ProjectSettings, m_Time, "Time manager")) return 11;
        if (!StartUpSubsystem(m_ProjectSettings, m_AssetManager, "Asset manager")) return 12;
        if (!StartUpSubsystem(m_ProjectSettings, m_EventManager, "Event manager")) return 13;
        if (!StartUpSubsystem(m_ProjectSettings, m_SceneManager, "Scene manager")) return 14;
        if (!StartUpSubsystem(m_ProjectSettings, m_WindowManager, "Window manager")) return 15;
        if (!StartUpSubsystem(m_ProjectSettings, m_InputManager, "Input manager")) return 16;
        if (!StartUpSubsystem(m_ProjectSettings, m_Renderer, "Renderer")) return 17;
        if (!StartUpSubsystem(m_ProjectSettings, m_PhysicsEngine, "Physics engine")) return 18;

        // Additional subsystem initialization logic
        m_Renderer->LateInit(); // no check for initialization because Engine::Startup would return early if renderer wasn't initialized up correctly

        m_ThreadPool = std::make_unique<ThreadPool>();
        Debug::Trace("Creating global thread pool with {} threads.", m_ThreadPool->GetSize());

        m_Editor = std::make_unique<Backend::Editor::Editor>();

        Debug::Trace("Rigel engine initialization complete.");

        m_Initialized = true;
        return 0;
    }

    void Engine::Shutdown()
    {
        Debug::Trace("Shutting down Rigel engine.");

        // Additional logic needed to shut down subsystems
        if (m_Renderer->IsInitialized()) m_Renderer->WaitForFinish();
        if (m_SceneManager->IsInitialized()) m_SceneManager->UnloadCurrentScene();
        if (m_AssetManager->IsInitialized()) m_AssetManager->UnloadAllAssets();

        // Shutdown order matters A LOT!
        ShutDownSubsystem(m_PhysicsEngine, "Physics engine");
        ShutDownSubsystem(m_Renderer, "Renderer");
        ShutDownSubsystem(m_InputManager, "Input manager");
        ShutDownSubsystem(m_WindowManager, "Window manager");
        ShutDownSubsystem(m_SceneManager, "Scene manager");
        ShutDownSubsystem(m_EventManager, "Event manager");
        ShutDownSubsystem(m_AssetManager, "Asset manager");
        ShutDownSubsystem(m_Time, "Time manager");
    }

    void Engine::Run()
    {
        auto fpsLimitStopwatch = Stopwatch();
        m_Time->m_DeltaTimeStopwatch.Start();
        m_Time->m_GlobalTimeStopwatch.Start();
        m_Running = true;

        if (!m_SceneManager->IsSceneLoaded())
            Debug::Warning("No scene is loaded before the engine enters the game loop!");

        Debug::Trace("Entering the game loop. Target FPS: {}", Time::GetTargetFPS());

        while (Running())
        {
            fpsLimitStopwatch.Start();
            EngineUpdate();
            SleepUtility::LimitFPS(fpsLimitStopwatch.Stop(), Time::GetTargetFPS());

            m_Time->m_DeltaTime = m_Time->m_DeltaTimeStopwatch.Restart().AsSeconds();
            m_Time->m_FrameCounter++;

            // for now the only condition for the engine to keep running is the window not being closed.
            m_Running = !m_WindowManager->WindowShouldClose();
        }

        m_Time->m_GlobalTimeStopwatch.Stop();
    }

    void Engine::EngineUpdate() const
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
    }
}
