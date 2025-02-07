#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"

#include <string>
#include <unordered_map>

namespace rge
{
    class GOHandle;
    class GameObject;

    class Scene final : public RigelObject, public ISerializable
    {
    public:
        Scene(const Scene&) = delete;
        Scene operator = (const Scene&) = delete;

        NODISCARD nlohmann::json Serialize() const override;
        void Deserialize(const nlohmann::json& json) override;

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
