#pragma once

#include "Core.hpp"

#include <filesystem>
#include <vector>

namespace rge
{
    enum SearchArgs : uint32_t
    {
        None = 0,
        Recursive = 1,
        LocalPath = 2,

    };

    class Directory
    {
    public:
        NODISCARD static std::filesystem::path WorkingDirectory();

        NODISCARD static std::vector<std::filesystem::path> Search(const std::filesystem::path& directory,
            const uint32_t args = SearchArgs::None, const std::string& extension = "");
    };
}