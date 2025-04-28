#pragma once

#include "Core.hpp"
#include "Debug.hpp"
#include "RigelSubsystem.hpp"
#include "Event.hpp"
#include "Engine.hpp"
#include "ThreadPool.hpp"

#include <ranges>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace Rigel
{
    template<typename T>
    concept EventTypeConcept = std::is_base_of_v<Event, T>;

    class EventManager final : public RigelSubsystem
    {
    public:
        using CallbackID = uid_t;

        /**
         * Subscribe a callback function to an event of type EventType
         * NOTE: This overload only supports lambdas and free functions
         * @tparam EventType The type of event to subscribe to
         * @param callback The event callback function
         * @return A unique event callback ID, you can use it to unsubscribe the callback
         */
        template<EventTypeConcept EventType>
        CallbackID Subscribe(const std::function<void(const EventType&)>& callback)
        {
            CallbackID id = m_NextCallbackID++;
            auto wrapper = [callback](const Event& event) {
                callback(static_cast<const EventType&>(event));
            };

            m_Subscribers[typeid(EventType)].emplace_back(id, wrapper);
            return id;
        }

        /**
         * Subscribe a callback method to an event of type EventType
         * NOTE: This overload is meant to work with class method and does not
         * support lambdas or free functions
         * @tparam EventType The type of event to subscribe to
         * @tparam T Class instance type
         * @param instance The class instance
         * @param memberFunc The class member function
         * @return A unique event callback ID, you can use it to unsubscribe the callback
         */
        template<EventTypeConcept EventType, typename T>
        CallbackID Subscribe(T* instance, void (T::*memberFunc)(const EventType&))
        {
            CallbackID id = m_NextCallbackID++;
            auto wrapper = [instance, memberFunc](const Event& event) {
                (instance->*memberFunc)(static_cast<const EventType&>(event));
            };

            m_Subscribers[typeid(EventType)].emplace_back(id, wrapper);
            return id;
        }

        template<EventTypeConcept EventType>
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

        template<EventTypeConcept EventType>
        void Dispatch(const EventType& event)
        {
            auto it = m_Subscribers.find(typeid(EventType));
            if (it != m_Subscribers.end())
            {
                for (const auto& callback : it->second | std::views::values)
                    callback(event);
            }
        }

        /**
         * Executes an event of 'EventType' on a thread pool. All subscriber functions are grouped to avoid Enqueue overhead.
         * Assumes that all subscriber functions are atomic.
         * Blocks the calling thread until all event subscribers have been dispatched.
         * @tparam EventType The type of the event that will be dispatched
         * @param pool The thread pool that will be used to dispatch the event subscribers
         * @param event An event that will be dispatched on a thread pool
         * @param groups How many groups all the event subscribers will be divided to. Each group
         * guaranteed to be executed a separate thread.
         */
        template<EventTypeConcept EventType>
        void DispatchThreaded(const EventType& event, ThreadPool& pool, const size_t groups)
        {
            ASSERT(groups <= pool.GetSize(), "The number of threaded dispatch task groups must be less than the number of threads in the pool");

            auto futures = std::vector<std::future<void>>();

            auto it = m_Subscribers.find(typeid(EventType));
            if (it != m_Subscribers.end())
            {
                const auto totalEvents = it->second.size();
                const auto eventsPerThread = (totalEvents + groups - 1) / groups;

                for (size_t workerIdx = 0; workerIdx < groups; workerIdx++)
                {
                    const auto startIdx = workerIdx * eventsPerThread;
                    if (startIdx >= totalEvents) break;

                    const auto endIdx = std::min(startIdx + eventsPerThread, totalEvents);

                    futures.emplace_back(pool.Enqueue([startIdx, endIdx, it, event]
                    {
                        for (size_t i = startIdx; i < endIdx; ++i)
                            it->second[i].second(event);
                    }));
                }
            }

            for (auto& future : futures)
                future.wait();
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
