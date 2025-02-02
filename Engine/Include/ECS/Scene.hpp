#pragma once

#include "HeaderUtils.hpp"
#include "RigelHandle.hpp"

#include <string>

namespace rge
{
    class RGE_API Scene final
    {
    public:
        Scene(const Scene&) = delete;
        Scene operator = (const Scene&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
        NODISCARD inline std::string GetName() const { return m_Name; }
    private:
        Scene(std::string name = "New scene");
        ~Scene();

        void OnLoad();
        void OnUnload();

        uid_t m_ID = NULL_ID;
        std::string m_Name;

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
