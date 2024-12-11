#pragma once

#include "STD_PCH.hpp"

namespace rge
{
    class File
    {
    public:
        explicit File(std::filesystem::path path);
        ~File();
    private:
        std::filesystem::path m_Path;
        bool m_IsValid;
    };
}
