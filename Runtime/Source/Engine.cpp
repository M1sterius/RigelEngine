#include "Engine.hpp"

#include "Time.hpp"
#include "SceneManager.hpp"
#include "RGE_PCH.hpp"
#include "SleepUtility.hpp"
#include "UIdGenerator.hpp"

namespace rge
{
    Engine* Engine::m_Instance = nullptr;

    void Engine::Startup()
    {
        ASSERT(m_Instance == nullptr, "Core subsystem has already been initialized!")
        m_Instance = new Engine();
        m_Instance->InternalStartup(); // Startup redirected to non-static method to avoid having to use m_Instance->
    }
    void Engine::Shutdown()
    {
        ASSERT(m_Instance != nullptr, "Unable to shutdown Core subsystem because "
                  "it's either not initialized yet or has already been shut down!")

        m_Instance->InternalShutdown(); // Shutdown redirected to non-static method to avoid having to use m_Instance->
        delete m_Instance;
        m_Instance = nullptr;
    }

    void Engine::InternalStartup()
    {
        rge::Logger::VerboseMessage("Engine core successfully initialized.");

        // Start up all subsystems
        rge::SceneManager::Get().Startup();

        rge::Time::GlobalTimeStopwatch.Start();
        m_IsRunning = true;
    }

    void Engine::InternalShutdown()
    {
        // Shut down all subsystems
        rge::SceneManager::Get().Shutdown();

        rge::Time::GlobalTimeStopwatch.Stop();

        rge::Logger::VerboseMessage("Engine core successfully shut down.");
    }

    void Engine::Run()
    {
        while (IsRunning())
        {
            rge::Time::SetDeltaTime(rge::Time::DeltaTimeStopwatch.Restart().AsSeconds());
            rge::Time::FrameCount++;

            static auto sw = Stopwatch();
            sw.Start();

            EngineUpdate();

            printf("Frame: %zu, Delta time: %.4f\n", rge::Time::GetFrameCount(), rge::Time::GetDeltaTime());
            LimitFPS(sw.Stop().AsSeconds(), Time::GetTargetFPS());
        }
    }

    void Engine::EngineUpdate()
    {
        // Poll and process events
        // Update input state
        // Game update
        // Transform update
        // Pre-render calculations
        // Scene render
        // Gizmo render
        // GUI render
        // Sleep to target FPS
    }
}
