#pragma once

#include "HeaderUtils.hpp"
#include "ComponentHandle.hpp"
#include "Component.hpp"

#include <string>
#include <unordered_map>

namespace rge
{
    class GameObject final
    {
    public:
        GameObject(const GameObject&) = delete;
        GameObject operator = (const GameObject&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
        NODISCARD inline uid_t GetSceneID() const { return m_SceneID; }

        template<typename T, typename... Args>
        ComponentHandle<T> AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            const auto component = static_cast<Component*>(new T(std::forward<Args>(args)...));
            const auto id = component->GetID();

            m_Components[id] = component;

            return ComponentHandle<T>(static_cast<T*>(component), id, m_ID, m_SceneID);
        }

        template<typename T>
        ComponentHandle<T> GetComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component))
                    return ComponentHandle<T>(cast, id, m_ID, m_SceneID);
            }

            return ComponentHandle<T>(nullptr, NULL_ID, NULL_ID, NULL_ID);
        }
    private:
        explicit GameObject(std::string name);
        ~GameObject();

        uid_t m_ID = NULL_ID;
        uid_t m_SceneID = NULL_ID;
        std::string m_Name;

        std::unordered_map<uid_t, Component*> m_Components;

        friend class Scene;
    };
}
