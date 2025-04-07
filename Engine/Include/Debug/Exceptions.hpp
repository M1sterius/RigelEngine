#pragma once

#include "Core.hpp"

#include <stdexcept>
#include <string>

namespace Rigel
{
    /**
     * Base class for all exceptions thrown by Rigel engine
     */
    class RigelException : public std::exception
    {
    public:
        explicit RigelException(std::string what);

        [[nodiscard]] const char* what() const noexcept override;
    private:
        const std::string m_Description;
    };
}
