#pragma once

#include "Core.hpp"
#include "json_fwd.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <future>

namespace Rigel
{
    class File
    {
    public:
        NODISCARD static std::string ReadText(const std::filesystem::path& path);
        NODISCARD static nlohmann::json ReadJSON(const std::filesystem::path& path);
        NODISCARD static std::vector<char> ReadBinary(const std::filesystem::path& path);

        NODISCARD static std::future<std::string> ReadTextAsync(const std::filesystem::path& path);
        NODISCARD static std::future<std::vector<char>> ReadBinaryAsync(const std::filesystem::path& path);

        static void WriteText(const std::filesystem::path& path, const std::string& text);
        static void WriteBinary(const std::filesystem::path& path, const std::vector<char>& data);

        explicit File(std::filesystem::path path, const std::ios::openmode mode = std::ios::in);
        ~File();

        File(const File&) = delete;
        File operator = (const File&) = delete;

        void Flush();

        std::string ReadText();
        std::vector<char> ReadBinary();

        void WriteBinary(const std::vector<char>& data);
        void WriteText(const std::string& text);
    private:
        void OpenInMode(const std::ios::openmode mode);

        const std::filesystem::path m_Path;
        std::fstream m_File;
        std::ios::openmode m_CurrentOpenMode;
    };
}