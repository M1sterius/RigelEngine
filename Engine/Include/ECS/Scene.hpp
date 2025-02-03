#pragma once

#include "HeaderUtils.hpp"
#include "RigelHandle.hpp"
#include "GameObject.hpp"

#include <string>
#include <unordered_map>

namespace rge
{
    class RGE_API Scene final
    {
    public:
        Scene(const Scene&) = delete;
        Scene operator = (const Scene&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
        NODISCARD inline std::string GetName() const { return m_Name; }

        NODISCARD bool IsGOHandleValid(const GOHandle& handle) const;

        NODISCARD GOHandle AddGameObject(std::string name = "Game Object");
    private:
        explicit Scene(std::string name = "New scene");
        ~Scene();

        void OnLoad();
        void OnUnload();

        uid_t m_ID = NULL_ID;
        std::string m_Name;

        std::unordered_map<uid_t, GameObject*> m_Objects;

        friend class SceneManager;
    };

    class RGE_API SceneHandle final : public RigelHandle<Scene>
    {
    public:
        NODISCARD bool IsNull() const override;
        NODISCARD bool IsValid() const override;
    INTERNAL:
        SceneHandle(Scene* ptr, const uid_t id);
    };
}
