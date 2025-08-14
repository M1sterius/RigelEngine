#include "Utilities/Filesystem/Directory.hpp"
#include "Debug.hpp"

namespace fs = std::filesystem;

namespace Rigel
{
    std::filesystem::path Directory::WorkingDirectory()
    {
        return fs::current_path();
    }

    std::vector<fs::path> Directory::Search(const fs::path& directory, const uint32_t args, const std::string& extension)
    {
        auto contents = std::vector<fs::path>();

        if (!fs::exists(directory))
        {
            Debug::Error("Unable to find directory: " + directory.string());
            return contents;
        }

        if (!fs::is_directory(directory))
        {
            Debug::Error("Specified path " + directory.string() + "is not a directory!");
            return contents;
        }

        if ((args & SearchArgs::Recursive) != 0)
        {
            for (const auto& entry : fs::recursive_directory_iterator(directory))
                contents.push_back((args & SearchArgs::LocalPath) != 0 ? fs::relative(entry.path(), directory) : directory);
        }
        else
        {
            for (const auto& entry : fs::directory_iterator(directory))
                contents.push_back((args & SearchArgs::LocalPath) != 0 ? fs::relative(entry.path(), directory) : directory);
        }

        return contents;
    }

    std::filesystem::path Directory::LogsDirectory()
    {
        return Directory::WorkingDirectory().concat("/Logs");
    }
}
