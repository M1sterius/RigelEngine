#pragma once

#include "Core.hpp"
#include "Debug.hpp"
#include "ComponentHandle.hpp"
#include "Component.hpp"
#include "ISerializable.hpp"
#include "RigelObject.hpp"
#include "SceneHandle.hpp"

#include <string>
#include <vector>
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
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD inline std::string GetName() const { return m_Name; }
        inline void SetName(std::string name) { m_Name = std::move(name); }

        // Returns handle to the scene this object is attached to
        NODISCARD inline SceneHandle GetScene() const { return m_Scene; }

        template<typename T, typename... Args> ComponentHandle<T> AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            const auto component = static_cast<Component*>(new T(std::forward<Args>(args)...));

            // ID assigning implemented as a private method to allow RigelObject::OverrideID to remain internal
            const auto id = AssignIDToComponent(component);

            m_Components[id] = std::unique_ptr<Component>(component);

            return ComponentHandle<T>(static_cast<T*>(component), id, this->GetID(), m_Scene.GetID());
        }

        /**
         * Returns handle to the first component of type T attached to this GameObject
         */
        template<typename T> ComponentHandle<T> GetComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component))
                    return ComponentHandle<T>(cast, id, this->GetID(), m_Scene.GetID());
            }

            Debug::Error("Failed to retrieve a component of type " + static_cast<std::string>(typeid(T).name()));

            return ComponentHandle<T>(nullptr, NULL_ID, NULL_ID, NULL_ID);
        }

        template<typename T> NODISCARD bool HasComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from rge::Component");

            for (const auto& [id, component] : m_Components)
            {
                if (const auto cast = dynamic_cast<T*>(component))
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
