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
        NODISCARD static nlohmann::json ReadJSON(const std::filesystem::path& path);

        static void WriteText(const std::filesystem::path& path, const std::string& text);
        static void WriteJSON(const std::filesystem::path& path, const nlohmann::json& json);
        static void WriteBinary(const std::filesystem::path& path, const std::vector<char>& data);
    };
}