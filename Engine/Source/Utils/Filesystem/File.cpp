#include "File.hpp"
#include "json.hpp"

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

    void File::WriteBinary(const std::filesystem::path& path, const std::vector<char>& data)
    {
        auto file = std::ofstream(path, std::ios::out | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file at path: " + path.string());
        }

        file.write(data.data(), data.size());
        file.close();
    }

    void File::WriteJSON(const std::filesystem::path& path, const nlohmann::json& json)
    {
        auto file = std::ofstream(path, std::ios::out | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Failed to open file for writing: " + path.string());

        try
        {
            // Convert to string with proper formatting and write directly
            std::string jsonStr = json.dump();
            file.write(jsonStr.c_str(), jsonStr.length());
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Failed to write JSON to file: " + std::string(e.what()));
        }

        file.close();
    }

    nlohmann::json File::ReadJSON(const std::filesystem::path& path)
    {
        auto file = std::ifstream(path, std::ios::in | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Failed to open file for reading: " + path.string());

        try
        {
            // Read the first few bytes to check for BOM
            std::vector<char> bom(3);
            file.read(bom.data(), 3);

            // Check for UTF-8 BOM (EF BB BF)
            bool hasBom = (unsigned char)bom[0] == 0xEF &&
                          (unsigned char)bom[1] == 0xBB &&
                          (unsigned char)bom[2] == 0xBF;

            // If no BOM was found, reset file pointer to beginning
            if (!hasBom) file.seekg(0);

            // Parse JSON directly from the stream
            nlohmann::json json;
            file >> json;
            return json;
        }
        catch (const nlohmann::json::parse_error& e) {
            throw std::runtime_error("Failed to parse JSON from file: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Error reading JSON file: " + std::string(e.what()));
        }
    }
}
