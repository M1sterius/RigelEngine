#include "PhysicsEngine.hpp"
#include "EventManager.hpp"
#include "EngineEvents.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "Time.hpp"

namespace rge
{
    PhysicsEngine::PhysicsEngine() { Startup(); }
    PhysicsEngine::~PhysicsEngine() { Shutdown(); }

    void PhysicsEngine::Startup()
    {
        Debug::Trace("Starting up physics engine.");
    }

    void PhysicsEngine::Shutdown()
    {
        Debug::Trace("Shutting down physics engine.");
    }

    void PhysicsEngine::Tick()
    {
        /*
         * Handles dispatching physics update event at the exact tickrate.
         * For more info see: https://gafferongames.com/post/fix_your_timestep/
         */

        m_Accumulator += Time::GetDeltaTime();

        while (m_Accumulator >= m_TickTime)
        {
            Engine::Get().GetEventManager().Dispatch<PhysicsTickEvent>(PhysicsTickEvent(m_TickTime));
            m_Accumulator -= m_TickTime;
        }
    }
}
