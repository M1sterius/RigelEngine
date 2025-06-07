#include "PhysicsEngine.hpp"
#include "EventManager.hpp"
#include "EngineEvents.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "Time.hpp"

namespace Rigel
{
    ErrorCode PhysicsEngine::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up physics engine.");
        return ErrorCode::NONE;
    }

    ErrorCode PhysicsEngine::Shutdown()
    {
        Debug::Trace("Shutting down physics engine.");
        return ErrorCode::NONE;
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
