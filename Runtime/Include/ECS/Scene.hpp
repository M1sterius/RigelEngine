#pragma once

#include "Utils/Internal.hpp"
#include "Utils/Types.hpp"
#include "Utils/NODISCARD.hpp"

#include <string>
#include <memory>
#include <unordered_map>

namespace rge
{
    class GameObject;
    struct GOHandle;

    class Scene final
    {
    public:
        Scene(const Scene&) = delete;
        Scene operator = (const Scene&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }

        NODISCARD GOHandle AddGameObject();
    INTERNAL:
        Scene(std::string name, const uid_t uid);
        ~Scene();

        void OnLoad();
        void OnUnload();

        NODISCARD bool CheckGOValidity(const uid_t id);
    private:
        std::string m_Name;
        uid_t m_ID;

        std::unordered_map<uid_t, GameObject*> m_Objects;
    };

    struct SceneHandle final
    {
    public:
        Scene* operator -> () { return m_Ptr; }
        NODISCARD uid_t GetID() const { return m_ID; }
        NODISCARD bool IsValid();

        ~SceneHandle() = default;
    INTERNAL:
        SceneHandle(Scene* ptr, const uid_t id);
    private:
        Scene* m_Ptr;
        uid_t m_ID;
    };
}
