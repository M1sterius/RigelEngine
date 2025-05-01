#pragma once

#include "Core.hpp"
#include "Event.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"
#include "GOHandle.hpp"
#include "SceneHandle.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"

#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace Rigel
{
    class Component : public RigelObject, public ISerializable
    {
    public:
        ~Component() override;

        // Returns the type of derived component represented as a string
        NODISCARD virtual const char* GetTypeName() const = 0;

        NODISCARD inline SceneHandle GetScene() const { return m_Scene; }
        NODISCARD inline GOHandle GetGameObject() const { return m_GameObject; }

        void SetActive(const bool active);
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

        template<typename EventType, typename T> requires std::is_base_of_v<Event, EventType>
        void SubscribeEvent(void (T::*callback)())
        {
            const auto typeIndex = TYPE_INDEX(EventType);
            if (m_EventsRegistry.contains(typeIndex))
            {
                Debug::Error("Component with ID {} has already subscribed to an event of type {}!",
                    this->GetID(), TYPE_NAME(EventType));
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
