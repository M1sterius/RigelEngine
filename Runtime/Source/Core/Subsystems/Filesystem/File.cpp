#include "File.hpp"

#include <utility>

namespace rge
{
    File::File(std::filesystem::path path)
        : m_Path(std::move(path)), m_IsValid(true)
    {
        std::ifstream f(path);
    }

    File::~File()
    {

    }
}
