#include "Subsystems/PhysicsEngine/PhysicsEngine.hpp"
#include "Subsystems/EventSystem/EventManager.hpp"
#include "Subsystems/EventSystem/EngineEvents.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/Time.hpp"
#include "Debug.hpp"
#include "Engine.hpp"

namespace Rigel
{
    ErrorCode PhysicsEngine::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up physics engine.");
        return ErrorCode::OK;
    }

    ErrorCode PhysicsEngine::Shutdown()
    {
        Debug::Trace("Shutting down physics engine.");
        return ErrorCode::OK;
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
            GetEventManager()->Dispatch<PhysicsTickEvent>(PhysicsTickEvent(m_TickTime));
            m_Accumulator -= m_TickTime;
        }
    }
}
