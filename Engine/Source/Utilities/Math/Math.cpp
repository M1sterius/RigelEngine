#include "Math.hpp"

#include <functional>

namespace Rigel
{
    uint64_t Math::Hash(const std::string& string)
    {
        static auto hashFunc = std::hash<std::string>();
        return hashFunc(string);
    }

    uint64_t Math::Hash(const std::filesystem::path& path)
    {
        return Hash(path.string());
    }
}
