#pragma once

#include "NODISCARD.hpp"
#include "Internal.hpp"
#include "Subsystems/Subsystem.hpp"

#include <filesystem>

namespace rge
{
    class Filesystem final : public rge::Subsystem
    {
    public:
        NODISCARD const std::filesystem::path& GetWorkingDirectory() const;
    INTERNAL:
        void Startup() override;
        void Shutdown() override;

        ~Filesystem() override = default;
    private:
        std::filesystem::path m_WorkingDirectory;
    };
}
