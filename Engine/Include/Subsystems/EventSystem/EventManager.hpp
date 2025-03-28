#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "Event.hpp"
#include "Engine.hpp"
#include "ThreadPool.hpp"

#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace rge
{
    template<typename EventType>
    const EventType& CastEvent(const Event& event)
    {
        return static_cast<const EventType&>(event);
    }

    class EventManager final : public RigelSubsystem
    {
    public:
        using CallbackID = uid_t;

        template<typename EventType>
        CallbackID Subscribe(const std::function<void(const EventType&)>& callback)
        {
            CallbackID id = m_NextCallbackID++;
            auto wrapper = [callback](const Event& event) {
                callback(static_cast<const EventType&>(event)); // Safe cast
            };
            m_Subscribers[typeid(EventType)].emplace_back(id, wrapper);
            return id;
        }

        template<typename EventType>
        void Unsubscribe(CallbackID id)
        {
            auto it = m_Subscribers.find(typeid(EventType));
            if (it != m_Subscribers.end())
            {
                auto& vec = it->second;
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                    [id](const auto& pair) { return pair.first == id; }), vec.end());
            }
        }

        // TODO: Implement multithreaded dispatch
        template<typename EventType>
        void Dispatch(const EventType& event)
        {
            auto it = m_Subscribers.find(typeid(EventType));
            if (it != m_Subscribers.end())
            {
                for (const auto& [_, callback] : it->second)
                    callback(event);
            }
        }

        template<typename EventType>
        void DispatchThreaded(const EventType& event)
        {
            auto it = m_Subscribers.find(typeid(EventType));
            if (it != m_Subscribers.end())
            {
                auto& pool = Engine::Get().GetThreadPool();

                for (const auto& [_, callback] : it->second)
                {
                    pool.Enqueue([callback, event]()
                    {
                        callback(event);
                    });
                }

                pool.WaitForAll();
            }
        }
    INTERNAL:
        EventManager();
        ~EventManager() override;
    private:
        void Startup() override;
        void Shutdown() override;

        std::unordered_map<std::type_index, std::vector<std::pair<CallbackID, std::function<void(const Event&)>>>> m_Subscribers {};
        CallbackID m_NextCallbackID = 0;
    };
}
