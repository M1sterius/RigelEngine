#pragma once

#include "GOHandle.hpp"
#include "Core.hpp"
#include "RigelObject.hpp"
#include "GameObject.hpp"
#include "ISerializable.hpp"

#include <memory>
#include <string>
#include <vector>

namespace rge
{
    class GOHandle;
    class GameObject;

    class Scene final : public RigelObject, public ISerializable
    {
    public:
        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD inline std::string GetName() const { return m_Name; }
        inline void SetName(std::string name) { m_Name = std::move(name); }

        NODISCARD inline bool IsLoaded() const { return m_IsLoaded; }
        NODISCARD bool IsGOHandleValid(const GOHandle& handle) const;

        GOHandle InstantiateGO(std::string name = "GameObject");
        void DestroyGO(const GOHandle& handle);
    INTERNAL:
        ~Scene() override = default;

        NODISCARD uid_t GetNextObjectID() { return m_NextObjectID++; }
    private:
        explicit Scene(const uid_t id, std::string name = "New scene");

        void OnLoad(); // Used for initialization logic. Will invoke OnLoad and OnStart for each game object.
        void OnUnload(); // Used for cleanup logic. Will invoke OnDestroy for each game object.

        std::string m_Name;
        bool m_IsLoaded = false;
        uid_t m_NextObjectID = 1;

        std::vector<std::unique_ptr<GameObject>> m_GameObjects; // Scene owns game objects

        friend class SceneManager;
    };
}
