#pragma once

#include "RigelHandle.hpp"
#include "RigelObject.hpp"
#include "Component.hpp"
#include "Utils/HeaderUtils.hpp"
#include "Scene.hpp"

#include <string>
#include <iostream>
#include <unordered_map>

namespace rge
{
    class RGE_API GameObject final : public RigelObject
    {
    public:
        NODISCARD inline std::string GetName() const { return m_Name; }
        inline void SetName(const std::string& name) { m_Name = name; }

        NODISCARD bool GetActive() const { return m_IsActive; }
        void SetActive(const bool active);

        template<typename T, typename... Args>
        ComponentHandle AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            auto component = static_cast<Component*>(new T(std::forward<Args>(args)...));
            auto id = component->GetID();

            m_Components[id] = component;

            return ConstructComponentHandle(component, id);
        }

        template<typename T>
        ComponentHandle GetComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            for (const auto& [id, component] : m_Components)
            {
                if (auto cast = dynamic_cast<T*>(component))
                    return ConstructComponentHandle(component, id);
            }

            return ConstructComponentHandle(nullptr, NULL_ID);
        }
    INTERNAL:
        explicit GameObject(std::string name = "Game Object");
        ~GameObject();

        ComponentHandle ConstructComponentHandle(Component* ptr, const uid_t id);

        // Event functions
        void OnLoad();
        void OnUnload();
        void OnGameUpdate();
        void OnEnable();
        void OnDisable();
    private:
        std::string m_Name;
        bool m_IsActive = true;
        std::unordered_map<uid_t, Component*> m_Components;
    };

    class GOHandle final : public RigelHandle<GameObject>
    {
    public:
        NODISCARD bool IsValid() const override;
        NODISCARD bool IsNull() const override;

        bool operator == (const RigelHandle& other) const override;
        bool operator != (const RigelHandle& other) const override;
    INTERNAL:
        GOHandle(GameObject* ptr, const uid_t id);
    };
}
