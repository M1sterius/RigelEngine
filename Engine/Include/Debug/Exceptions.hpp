#pragma once

#include <stdexcept>
#include <string>

namespace rge
{
    class RigelError : public std::runtime_error
    {
    public:
        explicit RigelError(const std::string& what);
    };
}
