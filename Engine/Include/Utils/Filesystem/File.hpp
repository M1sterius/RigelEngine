#pragma once

#include "Core.hpp"
#include "json_fwd.hpp"

#include <filesystem>
#include <string>

namespace rge
{
    class File
    {
    public:
        NODISCARD static std::string ReadText(const std::filesystem::path& path);
        static void WriteText(const std::filesystem::path& path, const std::string& text);

        static void WriteBinary(const std::filesystem::path& path, const std::vector<char>& data);
    };
}