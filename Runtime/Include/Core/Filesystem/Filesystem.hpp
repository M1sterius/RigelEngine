#pragma once

#include "NODISCARD.hpp"
#include "Internal.hpp"

#include <filesystem>

namespace rge
{
    class Filesystem
    {
    public:
        Filesystem(const Filesystem& other) = delete;

        static void Startup();
        static void Shutdown();

        NODISCARD inline static Filesystem* Get() { return m_Instance; }
        NODISCARD inline static bool IsInitialized() { return m_Instance != nullptr; }

        NODISCARD inline std::filesystem::path GetWorkingDirectory() const { return m_WorkingDirectory; }
    private:
        Filesystem() = default;
        ~Filesystem() = default;

        static Filesystem* m_Instance;

        std::filesystem::path m_WorkingDirectory;
    };
}
