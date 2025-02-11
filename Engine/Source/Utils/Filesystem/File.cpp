#include "File.hpp"

#include <stdexcept>
#include <fstream>
#include <ostream>

namespace rge
{
    std::string File::ReadText(const std::filesystem::path& path)
    {
        auto file = std::ifstream(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to load file at path: " + path.string());
        }

        const auto content = std::string(std::istreambuf_iterator<char>(file),  std::istreambuf_iterator<char>());
        file.close();

        return content;
    }

    void File::WriteText(const std::filesystem::path& path, const std::string& text)
    {
        auto file = std::ofstream(path, std::ios::out);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file at path: " + path.string());
        }

        file << text;
        file.close();
    }
}
