#pragma once

#include "Utils/Internal.hpp"
#include "Utils/Types.hpp"
#include "Utils/NODISCARD.hpp"

#include <string>

namespace rge
{
    class Scene final
    {
    public:
        Scene(const Scene&) = delete;
        Scene operator = (const Scene&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
    INTERNAL:
        explicit Scene(std::string name, const uid_t uid);
        ~Scene();

        void OnLoad();
        void OnUnload();
    private:
        std::string m_Name;
        uid_t m_ID;
    };
}
