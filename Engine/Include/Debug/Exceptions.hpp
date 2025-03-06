#pragma once

#include "Core.hpp"

#include <stdexcept>
#include <string>

namespace rge
{
    /**
     * Base class for all exceptions thrown by Rigel engine
     */
    class RigelError : public std::exception
    {
    public:
        explicit RigelError(const std::string& what);
        NODISCARD const char* what() const noexcept override;
    private:
        const std::string m_Description;
    };
}
