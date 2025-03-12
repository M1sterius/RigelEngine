#include "Engine.hpp"

#include "InputManager.hpp"
#include "SceneManager.hpp"
#include "EventManager.hpp"
#include "EngineEvents.hpp"
#include "AssetManager.hpp"
#include "WindowManager.hpp"
#include "Renderer.hpp"
#include "PhysicsEngine.hpp"

#include "InternalEvents.hpp"
#include "SleepUtility.hpp"
#include "Debug.hpp"
#include "Time.hpp"
#include "Directory.hpp"

/*
 * Helps avoid boilerplate when writing a dozen of identical getters for engine subsystems.
 * Use it only for that and nothing else!!!
 */
#define DEFINE_SUBSYSTEM_GETTER(Subsystem) \
    Subsystem& Engine::Get##Subsystem() const \
    { \
        ASSERT(m_##Subsystem, "Attempted to retrieve a rge::"#Subsystem" instance before it has been initialized"); \
        return *m_##Subsystem; \
    }

namespace rge
{
    Engine* Engine::m_GlobalInstance = nullptr;

    Engine::Engine() = default;
    Engine::~Engine() { Shutdown(); }

    std::unique_ptr<Engine> Engine::CreateInstance()
    {
        ASSERT(m_GlobalInstance == nullptr, "Only a single instance of RigelEngine core class is allowed!")
        RGE_TRACE("Creating Rigel engine instance.");
        const auto instance = new Engine();
        ASSERT(instance != nullptr, "Failed to create RigelEngine instance!");
        m_GlobalInstance = instance;
        instance->Startup();
        return std::unique_ptr<Engine>(instance);
    }

    void Engine::Startup()
    {
        RGE_TRACE("Starting up Rigel engine.");
        RGE_TRACE("Engine working directory: " + rge::Directory::WorkingDirectory().string());
        RGE_TRACE("Starting up subsystems:");

        m_AssetManager = std::make_unique<AssetManager>();
        m_EventManager = std::make_unique<EventManager>();
        m_SceneManager = std::make_unique<SceneManager>();
        m_WindowManager = std::make_unique<WindowManager>();
        m_InputManager = std::make_unique<InputManager>();
        m_Renderer = std::make_unique<Renderer>();
        m_PhysicsEngine = std::make_unique<PhysicsEngine>();

        m_Running = true;
        m_GlobalTimeStopwatch.Start();

        RGE_TRACE("Rigel engine initialization complete.");
    }

    void Engine::Shutdown()
    {
        RGE_TRACE("Shutting down Rigel engine.");

        // Shut down all subsystems and global tools
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
    DEFINE_SUBSYSTEM_GETTER(PhysicsEngine);
#pragma endregion

    void Engine::Run()
    {
        auto fpsLimitStopwatch = Stopwatch();
        m_DeltaTimeStopwatch.Start();

        RGE_TRACE("Entering game loop. Target FPS: " + std::to_string(Time::GetTargetFPS()));

        while (IsRunning())
        {
            fpsLimitStopwatch.Start();

            EngineUpdate();

            LimitFPS(fpsLimitStopwatch.Stop().AsSeconds(), Time::GetTargetFPS());

            m_DeltaTime = m_DeltaTimeStopwatch.Restart().AsSeconds();
            m_FrameCounter++;
        }
    }

    void Engine::EngineUpdate()
    {
        // Update input state
        // Poll and process glfw events
        // Physics update
        // Game update
        // Transform update
        // Pre-render update
        // Scene render
        // Gizmo render
        // GUI render

        m_InputManager->PreProcessInput();

        m_WindowManager->PollGLFWEvents();
        m_PhysicsEngine->Tick();
        m_EventManager->Dispatch(GameUpdateEvent(Time::GetDeltaTime(), Time::GetFrameCount()));
        m_EventManager->Dispatch(backend::TransformUpdateEvent()); // TODO: Implement multithreaded dispatch
        m_Renderer->Render();

        // for now the only condition for the engine to keep running is the window not being closed.
        m_Running = !m_WindowManager->WindowShouldClose();
    }
}
