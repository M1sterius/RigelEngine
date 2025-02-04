#pragma once

#include "HeaderUtils.hpp"
#include "RigelObject.hpp"

#include <string>
#include <unordered_map>

namespace rge
{
    class GOHandle;
    class GameObject;

    class Scene final : public RigelObject
    {
    public:
        Scene(const Scene&) = delete;
        Scene operator = (const Scene&) = delete;

        NODISCARD inline std::string GetName() const { return m_Name; }

        NODISCARD bool IsGOHandleValid(const GOHandle& handle) const;

        GOHandle AddGameObject(std::string name = "Game Object");
    private:
        explicit Scene(std::string name = "New scene");
        ~Scene() override;

        void OnLoad(); // Called when the scene is loaded. Used for initialization logic.
        void OnUnload(); // Called when the scene is unloaded. Used for cleanup logic.

        std::string m_Name;

        std::unordered_map<uid_t, GameObject*> m_Objects;

        friend class SceneManager;
    };
}
