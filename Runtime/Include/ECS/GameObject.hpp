#pragma once

#include "RigelObject.hpp"
#include "Utils/Types.hpp"
#include "Utils/Internal.hpp"
#include "Utils/NODISCARD.hpp"

#include <string>
#include <iostream>

namespace rge
{
    class GameObject final : public RigelObject
    {
    public:
        inline void Bruh() { std::cout << "Bruh\n"; }
    INTERNAL:
        explicit GameObject(std::string name = "Game Object");
        ~GameObject();
    private:
        std::string m_Name;
    };

    struct GOHandle final
    {
    public:
        GameObject* operator -> () { return m_Ptr; }
        NODISCARD inline uid_t GetID() const { return m_GameObjectID; }
        NODISCARD inline uid_t GetSceneID() const { return m_SceneID; }
        NODISCARD bool IsValid() const;
        NODISCARD bool IsNull() const;

        static GOHandle NULL_HANDLE();

        ~GOHandle() = default;
    INTERNAL:
        GOHandle(GameObject* ptr, const uid_t sceneID, const uid_t goID);
    private:
        GameObject* m_Ptr;
        uid_t m_GameObjectID;
        uid_t m_SceneID;
    };
}
