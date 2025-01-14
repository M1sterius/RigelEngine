#pragma once

#include "RigelHandle.hpp"
#include "RigelObject.hpp"
#include "Utils/HeaderUtils.hpp"
#include "Scene.hpp"

#include <string>
#include <iostream>

namespace rge
{
    class RGE_API GameObject final : public RigelObject
    {
    public:
        NODISCARD inline std::string GetName() const { return m_Name; }
        inline void SetName(const std::string& name) { m_Name = name; }
    INTERNAL:
        explicit GameObject(std::string name = "Game Object");
        ~GameObject();

        void OnLoad();
        void OnUnload();
        void OnGameUpdate();
    private:
        std::string m_Name;
    };

    class GOHandle final : public RigelHandle<GameObject>
    {
    public:
        NODISCARD bool IsValid() const override;
        NODISCARD bool IsNull() const override;

        bool operator == (const RigelHandle& other) const override;
        bool operator != (const RigelHandle& other) const override;
    INTERNAL:
        GOHandle(GameObject* ptr, const uid_t id);
    };
}
