#pragma once

#include "Core.hpp"
#include "json_fwd.hpp"

#include <filesystem>
#include <string>

namespace Rigel
{
    class File
    {
    public:
        NODISCARD static Result<std::string> ReadText(const std::filesystem::path& path);
        NODISCARD static Result<nlohmann::json> ReadJSON(const std::filesystem::path& path);
        NODISCARD static Result<std::vector<char>> ReadBinary(const std::filesystem::path& path);

        // TODO: implement async file io
        // NODISCARD static std::future<std::string> ReadTextAsync(const std::filesystem::path& path);
        // NODISCARD static std::future<std::vector<char>> ReadBinaryAsync(const std::filesystem::path& path);

        static Result<void> WriteText(const std::filesystem::path& path, const std::string& text);
        static Result<void> WriteBinary(const std::filesystem::path& path, const std::vector<byte_t>& data);
    };
}