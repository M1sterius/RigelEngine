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
    Engine* Engine::m_GlobalInstance = nullptr;

    Engine::Engine() = default;
    Engine::~Engine()
    {
        Shutdown();

        // Reset the global instance so that a new one can be properly created
        m_GlobalInstance = nullptr;
    }

    std::unique_ptr<Engine> Engine::CreateInstance(const int32_t argc, char** argv)
    {
        ASSERT(m_GlobalInstance == nullptr, "Only a single instance of RigelEngine core class is allowed!")
        Debug::Trace("Creating Rigel engine instance.");

        const auto instance = new Engine();
        ASSERT(instance != nullptr, "Failed to create RigelEngine instance!");
        m_GlobalInstance = instance;
        instance->Startup(argc, argv);

        return std::unique_ptr<Engine>(instance);
    }

    void Engine::Startup(const int32_t argc, char** argv)
    {
        Debug::Trace("Starting up Rigel engine.");
        Debug::Trace("Engine working directory: {}", Directory::WorkingDirectory().string());
        Debug::Trace("Starting up subsystems:");

        // Create subsystem instances, startup methods are called in constructors
        m_AssetManager = std::make_unique<AssetManager>();
        m_EventManager = std::make_unique<EventManager>();
        m_SceneManager = std::make_unique<SceneManager>();
        m_WindowManager = std::make_unique<WindowManager>();
        m_InputManager = std::make_unique<InputManager>();
        m_Renderer = std::make_unique<Renderer>();
        m_PhysicsEngine = std::make_unique<PhysicsEngine>();

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
        m_AssetManager->UnloadAllAssets();

        // Delete subsystem instances, shutdown methods are called in destructors
        m_PhysicsEngine.reset();
        m_Renderer.reset();
        m_InputManager.reset();
        m_WindowManager.reset();
        m_SceneManager.reset();
        m_EventManager.reset();
        m_AssetManager.reset();

        m_GlobalTimeStopwatch.Stop();
    }

#pragma region SubsystemGetters
    DEFINE_SUBSYSTEM_GETTER(EventManager)
    DEFINE_SUBSYSTEM_GETTER(AssetManager)
    DEFINE_SUBSYSTEM_GETTER(SceneManager)
    DEFINE_SUBSYSTEM_GETTER(WindowManager)
    DEFINE_SUBSYSTEM_GETTER(InputManager)
    DEFINE_SUBSYSTEM_GETTER(Renderer)
    DEFINE_SUBSYSTEM_GETTER(PhysicsEngine)

    SubsystemsCollection Engine::GetAllSubsystems() const
    {
        return SubsystemsCollection {
            .SceneManager = GetSceneManager(),
            .Renderer= GetRenderer(),
            .EventManager = GetEventManager(),
            .AssetManager = GetAssetManager(),
            .WindowManager = GetWindowManager(),
            .InputManager = GetInputManager(),
            .PhysicsEngine = GetPhysicsEngine()
        };
    }
#pragma endregion

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
        m_EventManager->DispatchThreaded(Backend::TransformUpdateEvent(), m_ThreadPool->GetSize());
        m_Renderer->Prepare();
        m_Renderer->Render();

        m_InputManager->ResetInputState();
        m_EventManager->Dispatch(Backend::EndOfFrameEvent());

        // for now the only condition for the engine to keep running is the window not being closed.
        m_Running = !m_WindowManager->WindowShouldClose();
    }
}
