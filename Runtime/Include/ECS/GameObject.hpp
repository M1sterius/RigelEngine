#pragma once

#include "RigelHandle.hpp"
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

    class GOHandle final : public RigelHandle<GameObject>
    {
    public:
        NODISCARD bool IsValid() const override;
        NODISCARD bool IsNull() const override;
    INTERNAL:
        GOHandle(GameObject* ptr, const uid_t id);
    };
}
