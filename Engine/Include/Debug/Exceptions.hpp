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

/**
* A macro that helps to throw RigelError while simultaneously
* properly logging it via rge::Logger
*/
#define RGE_THROW_ERROR(Text) \
    rge::Logger::Get().Log(Text, rge::LogType::Error); \
    throw rge::RigelError(Text)
}
