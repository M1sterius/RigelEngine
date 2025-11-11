#pragma once

#include "Core.hpp"
#include "GameObject.hpp"
#include "RigelObject.hpp"
#include "Handles/GOHandle.hpp"
#include "Utilities/Serialization/ISerializable.hpp"

#include "plf/plf_colony.h"

#include <memory>
#include <string>
#include <queue>

namespace Rigel
{
    class GOHandle;
    class GameObject;

    class Scene final : public RigelObject, public ISerializable
    {
    public:
        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD std::string GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = std::move(name); }

        NODISCARD size_t GetSize() const { return m_GameObjects.size(); }

        NODISCARD bool IsLoaded() const { return m_Loaded; }

        GOHandle Instantiate(std::string name = "GameObject");
        void Destroy(const GOHandle& handle);

        /**
         * Destroys the game object immediately upon invocation of this method.
         * Using this method may come with serious performance penalty. DO NOT
         * use it unless absolutely necessary. Always prefer regular 'Destroy'!
         * @param handle The GameObject to be destroyed
         */
        void DestroyImmediately(const GOHandle& handle) { DestroyGOImpl(handle.GetID()); }

        /**
         * Searches objects on the scene by given conditional function
         * @param condition Custom function that determines what objects are selected
         * @param depthLimit Limits how many iteration the search loop will go through. Unlimited by default.
         * @return A plf::colony of handles to selected objects
         */
        NODISCARD plf::colony<GOHandle> Search(const std::function<bool(GOHandle&)>& condition, const size_t depthLimit = std::numeric_limits<size_t>::max()) const;

        NODISCARD GOHandle FindGameObjectByID(const uid_t id) const;

        /**
         * Finds and returns all components of the specified type in the scene.
         * This method searches through all active GameObjects and retrieves components
         * that match the provided type.
         * @tparam T The type of the components to find.
         * @return A list of components of the specified type found in the scene.
         */
        template<ComponentConcept T>
        NODISCARD std::vector<ComponentHandle<T>> FindComponentsOfType(const size_t maxComponents = std::numeric_limits<size_t>::max()) const
        {
            auto components = std::vector<ComponentHandle<T>>();

            for (const auto& go : m_GameObjects)
            {
                if (go->HasComponent<T>())
                {
                    if (const auto cmp = go->GetComponent<T>(); cmp->IsActive())
                        components.push_back(cmp);
                }

                if (components.size() > maxComponents)
                    return components;
            }

            return components;
        }

        NODISCARD GenericComponentHandle FindComponentByID(const uid_t id) const
        {
            for (const auto& go : m_GameObjects)
            {
                for (const auto& component : go->GetComponents())
                {
                    if (component.GetID() == id)
                        return component;
                }
            }

            return GenericComponentHandle::Null();
        }
    INTERNAL:
        ~Scene() override;

        // use to assign unique IDs to game objects and components
        NODISCARD uid_t GetNextObjectID() { return m_NextObjectID++; }
    private:
        explicit Scene(const uid_t id, std::string name = "New scene");

        void OnLoad(); // Called by SceneManager::Load
        void OnUnload(); // Called by SceneManager

        void OnEndOfFrame(); // Used to process GO deletion queue

        void DestroyGOImpl(const uid_t id); // the actual GO destroy logic

        // Defines whether loading logic for GOs/Components should be executed,
        // will be set to true in OnLoad method when this scene gets loaded via SceneManager::Load
        bool m_Loaded = false;

        std::string m_Name;
        uid_t m_NextObjectID = 1;
        uid_t m_EndOfFrameCallbackID = NULL_ID;

        plf::colony<std::unique_ptr<GameObject>> m_GameObjects;
        std::queue<GOHandle> m_DestroyQueue;

        friend class SceneManager;
    };
}
