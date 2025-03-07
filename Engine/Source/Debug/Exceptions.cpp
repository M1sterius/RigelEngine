#include "Exceptions.hpp"
#include "Debug.hpp"

#include <iostream>

namespace rge
{
    RigelException::RigelException(std::string what)
        : m_Description(std::move(what))
    {
        // Ensures that the exception description gets logged properly
        Debug::Error(m_Description);
    }

    const char* RigelException::what() const noexcept
    {
        return m_Description.c_str();
    }
}
