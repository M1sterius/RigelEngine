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
        Filesystem& operator =(Filesystem&&) = delete;
    INTERNAL:
        Filesystem() = default;
        ~Filesystem() = default;

        void Startup();
        void Shutdown();
    };
}
