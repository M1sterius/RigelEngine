#pragma once

#include "Core.hpp"
#include "Debug.hpp"
#include "ComponentHandle.hpp"
#include "Component.hpp"
#include "ISerializable.hpp"
#include "RigelObject.hpp"
#include "SceneHandle.hpp"

#include <string>
#include <unordered_map>

namespace rge
{
    template<typename T>
    concept ComponentConcept = std::is_base_of_v<Component, T>;

    /**
    *   The class for all objects that can exist in a scene
    */
    class GameObject final : public RigelObject, public ISerializable
    {
    public:
        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD inline std::string GetName() const { return m_Name; }
        inline void SetName(std::string name) { m_Name = std::move(name); }

        // Returns handle to the scene this object is attached to
        NODISCARD inline SceneHandle GetScene() const { return m_Scene; }

        template<ComponentConcept T, typename... Args>
        ComponentHandle<T> AddComponent(Args&&... args)
        {
            const auto component = static_cast<Component*>(new T(std::forward<Args>(args)...));

            // ID assigning implemented as a private method to allow RigelObject::OverrideID to remain internal
            const auto id = AssignIDToComponent(component);

            m_Components[id] = std::unique_ptr<Component>(component);

            return ComponentHandle<T>(static_cast<T*>(component), id, this->GetID(), m_Scene.GetID());
        }

        /**
         * Returns handle to the first component of type T attached to this GameObject
         */
        template<ComponentConcept T>
        NODISCARD ComponentHandle<T> GetComponent()
        {
            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component.get()))
                    return ComponentHandle<T>(cast, id, this->GetID(), m_Scene.GetID());
            }

            Debug::Error("Failed to retrieve a component of type " + static_cast<std::string>(typeid(T).name()));

            return ComponentHandle<T>::Null();
        }

        template<ComponentConcept T>
        NODISCARD std::vector<ComponentHandle<T>> GetComponents()
        {
            auto components = std::vector<ComponentHandle<T>>();

            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component.get()))
                    components.emplace_back(cast, id, this->GetID(), m_Scene.GetID());
            }

            return components;
        }

        ComponentHandle<Component> GetComponentByID(const uid_t ID) const
        {
            for (const auto& [currentID, component] : m_Components)
            {
                if (ID == currentID)
                    return { component.get(), currentID, this->GetID(), m_Scene.GetID() };
            }

            return ComponentHandle<Component>::Null();
        }

        template<ComponentConcept T>
        NODISCARD bool HasComponent()
        {
            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component.get()))
                    return true;
            }

            return false;
        }
    INTERNAL:
        ~GameObject() override;
    private:
        explicit GameObject(const uid_t id, std::string name);

        void OnLoad(); // Handles asset loading.
        void OnStart(); // Handles start behaviour that does not involve loading assets, guaranteed to run after OnLoad.
        void OnDestroy(); // Handles freeing assets and other behaviour that may be required during object's destruction.

        NODISCARD uid_t AssignIDToComponent(Component* ptr);

        SceneHandle m_Scene;
        std::string m_Name;
        std::unordered_map<uid_t, std::unique_ptr<Component>> m_Components;

        friend class Scene;
    };
}
