#pragma once

#include "Core.hpp"
#include "Debug.hpp"
#include "Handles/ComponentHandle.hpp"
#include "Handles/HandleValidator.hpp"
#include "Component.hpp"
#include "Utilities/Serialization/ISerializable.hpp"
#include "RigelObject.hpp"
#include "Handles/SceneHandle.hpp"
#include "Components/Transform.hpp"

#include <string>
#include <typeindex>
#include <type_traits>
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

        void SetActive(const bool active);
        NODISCARD bool IsActive() const { return m_Active; }

        // Returns handle to the scene this object is attached to
        NODISCARD inline SceneHandle GetScene() const { return m_Scene; }

        /**
         * Returns Transform component attached to the GameObject
         * @return Rigel::ComponentHandle<Rigel::Transform> to Transform attached to the game object
         */
        NODISCARD inline ComponentHandle<Transform> GetTransform() const
        {
            if (!HasComponent<Transform>())
            {
                Debug::Error("A Transform component is not present on a Game Object with ID {}!", this->GetID());
                return ComponentHandle<Transform>::Null();
            }

            return GetComponent<Transform>();
        }

        /**
         * Adds a component of type T to the GameObject.
         *
         * If a component of the same type already exists, a null ComponentHandle is returned.
         * A component of any type can only exist once per GameObject at the same time.
         *
         * @tparam T The type of the component to add.
         * @param args Constructor arguments for creating the component instance.
         * @return A valid ComponentHandle for the added component, or a null ComponentHandle if component of the
         * same type is already attached to the GameObject.
         */
        template<ComponentConcept T, typename... Args>
        ComponentHandle<T> AddComponent(Args&&... args)
        {
            if (HasComponent<T>())
            {
                Debug::Error("Only one instance of a component of any type is allowed to be attached to a GameObject at a time!");
                return ComponentHandle<T>::Null();
            }

            const auto component = static_cast<Component*>(new T(std::forward<Args>(args)...));
            component->m_Scene = m_Scene;
            component->m_GameObject = GOHandle(this, this->GetID());

            // ID assigning implemented as a private method to allow RigelObject::OverrideID to remain internal
            const auto id = AssignIDToComponent(component);

            using namespace Backend::HandleValidation;
            HandleValidator::AddHandle<HandleType::ComponentHandle>(id);

            m_Components[TYPE_INDEX(T)] = std::unique_ptr<Component>(component);

            if (m_Loaded)
            {
                component->CallOnLoad();
                component->CallOnStart();
            }

            return ComponentHandle<T>(static_cast<T*>(component), id);
        }

        /**
         * Removes component of type T attached to the GameObject.
         * Calls OnDestroy component method if the component was loaded.
         * @tparam T The type of the component to remove.
         */
        template<ComponentConcept T>
        void RemoveComponent()
        {
            if (!HasComponent<T>())
            {
                Debug::Error("Failed to remove a component of type {} from game object with ID {}, "
                        "the component is not attached to this game object!", TypeUtility::GetTypeName<T>(), GetID());
                return;
            }

            if (TYPE_EQUAL(T, Transform))
            {
                Debug::Error("Transform component cannot be removed from a GameObject!");
                return;
            }

            const auto index = TYPE_INDEX(T);

            if (m_Loaded)
                m_Components[index]->CallOnDestroy();

            using namespace Backend::HandleValidation;
            HandleValidator::RemoveHandle<HandleType::ComponentHandle>(m_Components.at(index)->GetID());

            m_Components.erase(index);
        }

        /**
         * Returns handle to the component of type T attached to this GameObject.
         * Will Return a null handle if the component of type T is not attached to the GameObject.
         * @tparam T The type of the component to get.
         */
        template<ComponentConcept T>
        NODISCARD ComponentHandle<T> GetComponent() const
        {
            if (const auto index = TYPE_INDEX(T); m_Components.contains(index))
                return ComponentHandle<T>(static_cast<T*>(m_Components.at(index).get()),
                    m_Components.at(index)->GetID());

            Debug::Error("Component of type {} is not attached to game object with ID {}!", TypeUtility::GetTypeName<T>(), GetID());
            return ComponentHandle<T>::Null();
        }

        /**
         * Checks whether the component of type T is attached to the GameObject.
         * @tparam T The type of the component
         * @return A bool indicating whether the component of type T is attached to the GameObject
         */
        template<ComponentConcept T>
        NODISCARD bool HasComponent() const
        {
            return m_Components.contains(TYPE_INDEX(T));
        }
    INTERNAL:
        ~GameObject() override;

        NODISCARD std::vector<ComponentHandle<Component>> GetComponents() const
        {
            auto vec = std::vector<GenericComponentHandle>();
            vec.reserve(m_Components.size());

            for (const auto& component : m_Components | std::views::values)
                vec.emplace_back(component.get(), component->GetID());

            return vec;
        }
    private:
        explicit GameObject(const uid_t id, std::string name);

        void OnLoad(); // Handles asset loading.
        void OnStart(); // Handles start behaviour that does not involve loading assets, guaranteed to run after OnLoad.
        void OnDestroy(); // Handles freeing assets and other behaviour that may be required during object's destruction.

        NODISCARD uid_t AssignIDToComponent(Component* ptr);

        // m_Loaded defines whether loading logic for Components should be executed,
        // will be set to true in OnLoad method, which is called by Scene::OnLoad
        bool m_Loaded = false;
        bool m_Active = true;

        SceneHandle m_Scene;
        std::string m_Name;
        std::unordered_map<std::type_index, std::unique_ptr<Component>> m_Components;

        friend class Scene;
    };
}
