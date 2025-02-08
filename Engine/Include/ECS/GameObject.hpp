#pragma once

#include <Debug.hpp>

#include "Core.hpp"
#include "ComponentHandle.hpp"
#include "Component.hpp"
#include "ISerializable.hpp"
#include "RigelObject.hpp"

#include <string>
#include <unordered_map>

namespace rge
{
    /**
    *   The class for all objects that can exist in a scene
    */
    class GameObject final : public RigelObject, public ISerializable
    {
    public:
        NODISCARD nlohmann::json Serialize() const override;
        void Deserialize(const nlohmann::json& json) override;

        NODISCARD inline uid_t GetSceneID() const { return m_SceneID; }

        template<typename T, typename... Args> ComponentHandle<T> AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            const auto component = static_cast<Component*>(new T(std::forward<Args>(args)...));

            // ID assigning implemented as a private method to allow RigelObject::OverrideID to remain internal
            const auto id = AssignIDToComponent(component);

            m_Components[id] = component;

            return ComponentHandle<T>(static_cast<T*>(component), id, this->GetID(), m_SceneID);
        }

        /*
         * Returns a handle to the first component of type T attached to this GameObject
         */
        template<typename T> ComponentHandle<T> GetComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component))
                    return ComponentHandle<T>(cast, id, this->GetID(), m_SceneID);
            }

            Debug::Error("Failed to retrieve a component of type " + static_cast<std::string>(typeid(T).name()));

            return ComponentHandle<T>(nullptr, NULL_ID, NULL_ID, NULL_ID);
        }
    private:
        explicit GameObject(const uid_t id, std::string name);
        ~GameObject() override;

        NODISCARD uid_t AssignIDToComponent(Component* ptr) const;

        uid_t m_SceneID = NULL_ID;
        std::string m_Name;
        std::unordered_map<uid_t, Component*> m_Components;

        friend class Scene;
    };
}
