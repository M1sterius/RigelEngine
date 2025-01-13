#pragma once

#include "RigelObject.hpp"
#include "RigelHandle.hpp"
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

    class Scene final : public RigelObject
    {
    public:
        NODISCARD GOHandle AddGameObject();
    INTERNAL:
        explicit Scene(std::string name = "New Scene");
        ~Scene();

        NODISCARD bool CheckGOValidity(const uid_t id);

        void OnLoad();
        void OnUnload();
    private:
        std::string m_Name;

        std::unordered_map<uid_t, GameObject*> m_Objects;
    };

    class SceneHandle final : public RigelHandle<Scene>
    {
    public:
        NODISCARD bool IsValid() const override;
        NODISCARD bool IsNull() const override;
    INTERNAL:
        SceneHandle(Scene* ptr, const uid_t id);
    };
}
