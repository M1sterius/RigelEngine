#pragma once

#include "Core.hpp"
#include "Debug.hpp"
#include "ComponentHandle.hpp"
#include "HandleValidator.hpp"
#include "Component.hpp"
#include "ISerializable.hpp"
#include "RigelObject.hpp"
#include "SceneHandle.hpp"
#include "Transform.hpp"

#include <string>
#include <ranges>
#include <unordered_map>

namespace Rigel
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

        NODISCARD inline ComponentHandle<Transform> GetTransform() const
        {
            if (!HasComponent<Transform>())
            {
                Debug::Error("A Transform component is not present on a Game Object with ID {}!", this->GetID());
                return ComponentHandle<Transform>::Null();
            }

            return GetComponent<Transform>();
        }

#pragma region Components
        template<ComponentConcept T, typename... Args>
        ComponentHandle<T> AddComponent(Args&&... args)
        {
            if (typeid(T) == typeid(Transform) && HasComponent<Transform>())
            {
                Debug::Error("Only one instance of Transform component is allowed to be attached to a Game Object!");
                return ComponentHandle<T>::Null();
            }

            const auto component = static_cast<Component*>(new T(std::forward<Args>(args)...));
            component->m_Scene = m_Scene;
            component->m_GameObject = GOHandle(this, this->GetID());

            // ID assigning implemented as a private method to allow RigelObject::OverrideID to remain internal
            const auto id = AssignIDToComponent(component);

            using namespace Backend::HandleValidation;
            HandleValidator::AddHandle<HandleType::ComponentHandle>(id);

            m_Components[id] = std::unique_ptr<Component>(component);

            return ComponentHandle<T>(static_cast<T*>(component), id);
        }

        template<ComponentConcept T>
        void RemoveComponent()
        {
            for (auto it = m_Components.begin(); it != m_Components.end();)
            {
                const auto& [id, component] = *it;

                if (const auto cast = dynamic_cast<T*>(component.get()))
                {
                    using namespace Backend::HandleValidation;
                    HandleValidator::RemoveHandle<HandleType::ComponentHandle>(id);

                    it = m_Components.erase(it); // returns next valid iterator
                    return;
                }
                else
                {
                    ++it;
                }
            }

            Debug::Error("Failed to remove component of type {} from GameObject with ID {}!",
                         typeid(T).name(), this->GetID());
        }

        /**
         * Returns handle to the first component of type T attached to this GameObject
         */
        template<ComponentConcept T>
        NODISCARD ComponentHandle<T> GetComponent() const
        {
            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component.get()))
                    return ComponentHandle<T>(cast, id);
            }

            Debug::Error("Failed to retrieve a component of type " + static_cast<std::string>(typeid(T).name()));

            return ComponentHandle<T>::Null();
        }

        template<ComponentConcept T>
        NODISCARD std::vector<ComponentHandle<T>> GetComponents() const
        {
            auto components = std::vector<ComponentHandle<T>>();

            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component.get()))
                    components.emplace_back(cast, id);
            }

            return components;
        }

        NODISCARD ComponentHandle<Component> GetComponentByID(const uid_t ID) const
        {
            for (const auto& [currentID, component] : m_Components)
            {
                if (ID == currentID)
                    return {component.get(), currentID};
            }

            return ComponentHandle<Component>::Null();
        }

        template<ComponentConcept T>
        NODISCARD bool HasComponent() const
        {
            for (const auto& component : m_Components | std::views::values)
            {
                if (const auto cast = dynamic_cast<T*>(component.get()))
                    return true;
            }

            return false;
        }
#pragma endregion
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
