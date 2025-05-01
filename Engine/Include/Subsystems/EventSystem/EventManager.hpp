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
    class Component;

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
            const auto id = m_NextCallbackID++;
            auto wrapper = [callback](const Event& event) {
                callback(static_cast<const EventType&>(event));
            };

            m_Subscribers[TYPE_INDEX(EventType)].emplace_back(id, wrapper);
            m_SuspendTable[id] = false;
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
        template<EventTypeConcept EventType, typename T> requires std::is_base_of_v<Component, T>
        CallbackID Subscribe(T* instance, void (T::*memberFunc)(const EventType&))
        {
            const auto id = m_NextCallbackID++;
            auto wrapper = [instance, memberFunc](const Event& event) {
                (instance->*memberFunc)(static_cast<const EventType&>(event));
            };

            m_Subscribers[TYPE_INDEX(EventType)].emplace_back(id, wrapper);
            m_SuspendTable[id] = false;

            return id;
        }

        template<typename T> requires std::is_base_of_v<Component, T>
        CallbackID Subscribe(const std::type_index typeIndex, T* instance, void (T::*memberFunc)())
        {
            const auto id = m_NextCallbackID++;
            auto wrapper = [instance, memberFunc](const Event&) {
                (instance->*memberFunc)();
            };

            m_Subscribers[typeIndex].emplace_back(id, wrapper);
            m_SuspendTable[id] = false;

            return id;
        }

        void SetSuspend(const CallbackID id, const bool state)
        {
            if (!m_SuspendTable.contains(id))
            {
                Debug::Error("Attempted to suspend an event callback with invalid ID {}!", id);
                return;
            }

            m_SuspendTable[id] = state;
        }

        /**
         * @brief Unsubscribes a previously registered callback from an event of type EventType.
         *
         * This overload is type-safe and should be used when the event type is known at compile time.
         * If the given CallbackID is invalid (NULL_ID), the call is ignored.

         * @tparam EventType The event type to unsubscribe from.
         * @param id The unique ID of the callback to be removed.
         */
        template<EventTypeConcept EventType>
        void Unsubscribe(const CallbackID id)
        {
            if (id == NULL_ID) return;

            Unsubscribe(TYPE_INDEX(EventType), id);
        }

        /**
         * @brief Unsubscribes a previously registered callback using both event type and callback ID.
         *
         * This overload is useful when the type is only known at runtime (e.g., from a registry).
         * If the given CallbackID is invalid (NULL_ID), the call is ignored.
         *
         * @param type The type index of the event to unsubscribe from.
         * @param id The unique ID of the callback to remove.
         */
        void Unsubscribe(const std::type_index type, const CallbackID id)
        {
            if (id == NULL_ID) return;

            if (const auto it = m_Subscribers.find(type); it != m_Subscribers.end())
            {
                auto& vec = it->second;
                std::erase_if(vec, [id](const auto& pair) { return pair.first == id; });
            }

            m_SuspendTable.erase(id);
        }

        /**
         * @brief Dispatches an event to all registered callbacks for the given event type on the calling thread.
         * @tparam EventType The concrete event type to dispatch.
         * @param event The event instance to pass to all subscribers.
         */
        template<EventTypeConcept EventType>
        void Dispatch(const EventType& event)
        {
            auto it = m_Subscribers.find(typeid(EventType));
            if (it != m_Subscribers.end())
            {
                for (const auto& [id, callback] : it->second)
                {
                    if (!m_SuspendTable.at(id))
                        callback(event);
                }
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

                    futures.emplace_back(pool.Enqueue([startIdx, endIdx, it, event, this]
                    {
                        for (size_t i = startIdx; i < endIdx; ++i)
                        {
                            const auto& [id, callback] = it->second[i];
                            if (!m_SuspendTable.at(id))
                                callback(event);
                        }
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

        std::unordered_map<std::type_index, std::vector<std::pair<CallbackID, std::function<void(const Event&)>>>> m_Subscribers{};
        std::unordered_map<uid_t, bool> m_SuspendTable{}; // Ideally this should be optimized to use 1 bit per flag.
        CallbackID m_NextCallbackID = 0;
    };
}
