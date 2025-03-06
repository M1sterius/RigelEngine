#include "Exceptions.hpp"
#include "Debug.hpp"

#include <iostream>

namespace rge
{
    RigelError::RigelError(const std::string& what)
        : m_Description("RigelError: " + what) { }

    const char* RigelError::what() const noexcept
    {
        return m_Description.c_str();
    }
}
