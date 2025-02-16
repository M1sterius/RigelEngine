#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "Event.hpp"

#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace rge
{
    class EventManager : public RigelSubsystem
    {
    public:
        using EventCallback = std::function<void(const Event&)>;
        using CallbackID = uid_t;

        template<typename EventType>
        CallbackID Subscribe(const EventCallback& callback)
        {
            CallbackID id = m_NextCallbackID++;

            m_Subscribers[typeid(EventType)].emplace_back(id, callback);
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
    INTERNAL:
        EventManager();
        ~EventManager();
    private:
        void Startup() override;
        void Shutdown() override;

        std::unordered_map<std::type_index, std::vector<std::pair<CallbackID, EventCallback>>> m_Subscribers { };
        CallbackID m_NextCallbackID = 0;
    };
}
