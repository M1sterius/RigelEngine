#pragma once

#include "Core.hpp"
#include "Event.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"
#include "GOHandle.hpp"
#include "SceneHandle.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"
#include "TypeRegistry.hpp"

#include <type_traits>
#include <typeindex>
#include <unordered_map>

/**
 * @brief A Component specific version of RIGEL_REGISTER_TYPE macro, see it's description for more info.
 */
#define RIGEL_REGISTER_COMPONENT(Type) \
    friend class Rigel::GameObject; \
    RIGEL_REGISTER_TYPE(Type)

namespace Rigel
{
    class Component : public RigelObject, public ISerializable, public ITypeRegistrable
    {
    public:
        ~Component() override;

        // Returns the type of derived component represented as a string
        NODISCARD const char* GetTypeName() const override = 0;

        NODISCARD inline SceneHandle GetScene() const { return m_Scene; }
        NODISCARD inline GOHandle GetGameObject() const { return m_GameObject; }

        void SetActive(const bool active);
        NODISCARD bool IsActive() const { return m_Active; }
    protected:
        Component();

        // NEVER invoke these methods directly! Use private ones with Call prefix instead.
        virtual void OnLoad() { }
        virtual void OnStart() { }
        virtual void OnDestroy() { }
        virtual void OnEnable() { }
        virtual void OnDisable() { }

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        /**
         * @brief Subscribes a component method to an event of the specified type.
         *
         * Use this method to subscribe components to engine events (e.g. GameUpdateEvent or PhysicsTickEvent)
         * Using this method insures that all callbacks will be automatically unsubscribed when this component is destroyed
         * and suspended when it becomes inactive.
         *
         * Note that only one callback per event type is allowed at a time. Swapping callback methods
         * after they have already been subscribed is also not allowed.
         *
         * @tparam EventType EventType The type of the event to subscribe to. Must derive from Rigel::Event
         * @tparam T The type of the component owning the callback method. Must derive from Rigel::Component.
         * @param callback A pointer to the component member function to call when the event is dispatched.
         */
        template<typename EventType, typename T> requires std::is_base_of_v<Event, EventType>
        void SubscribeEvent(void (T::*callback)())
        {
            static_assert(std::is_base_of_v<Component, T>, "Callback owning class must inherit from Rigel::Component!");

            const auto typeIndex = TYPE_INDEX(EventType);
            if (m_EventsRegistry.contains(typeIndex))
            {
                Debug::Error("Component with ID {} has already subscribed to an event of type {}!",
                    this->GetID(), TypeUtility::GetTypeName<EventType>());
                return;
            }

            auto& eventManager = Engine::Get().GetEventManager();
            const auto callbackID = eventManager.Subscribe(typeIndex, static_cast<T*>(this), callback);

            m_EventsRegistry[typeIndex] = callbackID;
        }
    private:
        SceneHandle m_Scene;
        GOHandle m_GameObject;
        bool m_Active = true;
        bool m_Loaded = false;

        // Use these methods to propagate events instead of calling virtual event methods directly
        void CallOnLoad();
        void CallOnStart();
        void CallOnDestroy();
        void CallOnEnable();
        void CallOnDisable();

        std::unordered_map<std::type_index, uid_t> m_EventsRegistry{};

        friend class GameObject;
    };
}
