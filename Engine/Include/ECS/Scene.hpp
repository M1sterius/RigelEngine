#pragma once

#include <GOHandle.hpp>

#include "Core.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"

#include <string>
#include <vector>

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
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD inline std::string GetName() const { return m_Name; }

        NODISCARD inline bool IsLoaded() const { return m_IsLoaded; }
        NODISCARD bool IsGOHandleValid(const GOHandle& handle) const;

        GOHandle InstantiateGO(std::string name = "GameObject");
        void DestroyGO(const GOHandle& handle);
    INTERNAL:
        NODISCARD uid_t GetNextObjectID() { return m_NextObjectID++; }
    private:
        explicit Scene(const uid_t id, std::string name = "New scene");
        ~Scene() override;

        void OnLoad(); // Called when the scene is loaded. Used for initialization logic.
        void OnUnload(); // Called when the scene is unloaded. Used for cleanup logic.

        bool m_IsLoaded = false;
        std::string m_Name;
        uid_t m_NextObjectID = 1;

        std::vector<GameObject*> m_Objects;

        friend class SceneManager;
    };
}
