#include "File.hpp"

#include "json.hpp"

#include <ostream>

namespace Rigel
{
    Result<std::string> File::ReadText(const std::filesystem::path& path)
    {
        auto file = std::ifstream(path);
        if (!file.is_open())
            return Result<std::string>::Error(ErrorCode::FAILED_TO_OPEN_FILE);

        const auto content = std::string(std::istreambuf_iterator<char>(file),  std::istreambuf_iterator<char>());
        file.close();

        return Result<std::string>::Ok(content);
    }

    Result<nlohmann::json> File::ReadJSON(const std::filesystem::path& path)
    {
        auto file = std::ifstream(path, std::ios::in | std::ios::binary);

        if (!file.is_open())
            return Result<nlohmann::json>::Error(ErrorCode::FAILED_TO_OPEN_FILE);

        try
        {
            // Read the first few bytes to check for BOM
            std::vector<char> bom(3);
            file.read(bom.data(), 3);

            // Check for UTF-8 BOM (EF BB BF)
            const auto hasBom = static_cast<unsigned char>(bom[0]) == 0xEF &&
                          static_cast<unsigned char>(bom[1]) == 0xBB &&
                          static_cast<unsigned char>(bom[2]) == 0xBF;

            // If no BOM was found, reset file pointer to beginning
            if (!hasBom) file.seekg(0);

            // Parse JSON directly from the stream
            nlohmann::json json;
            file >> json;

            return Result<nlohmann::json>::Ok(json);
        }
        catch (const nlohmann::json::parse_error&) {
            return Result<nlohmann::json>::Error(ErrorCode::NLOHMANN_JSON_PARSING_ERROR);
        }
        catch (const std::exception&) {
            return Result<nlohmann::json>::Error(ErrorCode::NLOHMANN_JSON_READING_ERROR);
        }
    }

    Result<std::vector<char>> File::ReadBinary(const std::filesystem::path& path)
    {
        auto file = std::ifstream(path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            return Result<std::vector<char>>::Error(ErrorCode::FAILED_TO_OPEN_FILE);

        const auto fileSize = static_cast<size_t>(file.tellg());
        auto buffer = std::vector<char>(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return Result<std::vector<char>>::Ok(buffer);
    }

    Result<void> File::WriteText(const std::filesystem::path& path, const std::string& text)
    {
        auto file = std::ofstream(path);
        if (!file.is_open())
            return Result<void>::Error(ErrorCode::FAILED_TO_OPEN_FILE);

        file << text;
        file.close();

        return Result<void>::Error(ErrorCode::OK);
    }

    Result<void> File::WriteBinary(const std::filesystem::path& path, const std::vector<char>& data)
    {
        auto file = std::ofstream(path, std::ios::out | std::ios::binary);
        if (!file.is_open())
            return Result<void>::Error(ErrorCode::FAILED_TO_OPEN_FILE);

        file.write(data.data(), data.size());
        file.close();

        return Result<void>::Error(ErrorCode::OK);
    }

    // std::future<std::string> File::ReadTextAsync(const std::filesystem::path& path)
    // {
    //     // We have to do this ridiculous cast because the compiler cannot handle overloads in std::async,
    //     // So it confuses static and non-static function with the same name
    //     const auto asyncFuncCast = static_cast<std::string(*)(const std::filesystem::path&)>(&File::ReadText);
    //     return std::async(std::launch::async, asyncFuncCast, path);
    // }
    //
    // std::future<std::vector<char>> File::ReadBinaryAsync(const std::filesystem::path& path)
    // {
    //     // We have to do this ridiculous cast because the compiler cannot handle overloads in std::async,
    //     // So it confuses static and non-static function with the same name
    //     const auto asyncFuncCast = static_cast<std::vector<char>(*)(const std::filesystem::path&)>(&File::ReadBinary);
    //     return std::async(std::launch::async, asyncFuncCast, path);
    // }
}
