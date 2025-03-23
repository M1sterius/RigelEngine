#include "File.hpp"
#include "json.hpp"

#include <stdexcept>
#include <ostream>
#include <utility>
#include <iostream>

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

    std::vector<char> File::ReadBinary(const std::filesystem::path& path)
    {
        auto file = std::ifstream(path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("Failed to open file: " + path.string());

        const auto fileSize = static_cast<size_t>(file.tellg());
        auto buffer = std::vector<char>(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    File::File(std::filesystem::path path, const std::ios::openmode mode)
        :   m_Path(std::move(path)), m_CurrentOpenMode(mode)
    {
        OpenInMode(mode);
    }

    File::~File()
    {
        m_File.close();
    }

    void File::OpenInMode(const std::ios::openmode mode)
    {
        if (m_File.is_open() && (mode & m_CurrentOpenMode)) return;

        if (m_File.is_open())
        {
            m_File.clear();
            m_File.close();
        }

        m_File.open(m_Path, mode);

        if (!m_File.is_open())
            throw std::runtime_error("Failed to open file at path: " + m_Path.string());

        m_CurrentOpenMode = mode;
    }

    std::string File::ReadText()
    {
        OpenInMode(std::ios::in);
        m_File.seekg(0, std::ios::beg);
        return {std::istreambuf_iterator<char>(m_File),  std::istreambuf_iterator<char>()};
    }

    void File::WriteText(const std::string& text)
    {
        OpenInMode(std::ios::out);
        m_File.seekp(0, std::ios::end);
        m_File << text;
    }

    std::vector<char> File::ReadBinary()
    {
        // Maybe there is an issue with type conversion when reading bytes,
        // but generally it seems to be working. Check this function for errors in the future.

        OpenInMode(std::ios::in | std::ios::binary);
        m_File.seekg(0, std::ios::end);

        const size_t size = m_File.tellg();
        auto buffer = std::vector<char>(size, 0);

        m_File.seekg(0, std::ios::beg);
        m_File.read(buffer.data(), static_cast<std::streamsize>(size));

        return buffer;
    }

    void File::WriteBinary(const std::vector<char>& data)
    {
        OpenInMode(std::ios::out | std::ios::binary);
        m_File.seekp(0, std::ios::end);

        m_File.write(data.data(), static_cast<std::streamsize>(data.size()));
    }

    void File::Flush()
    {
        m_File.flush();
    }
}
