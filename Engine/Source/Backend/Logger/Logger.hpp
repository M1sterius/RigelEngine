#pragma once

#include "HeaderUtils.hpp"

#include <cstdint>
#include <string>

namespace rge
{
    enum LOG_TYPE : uint8_t
    {
        LOG_MESSAGE = 1,
        LOG_WARNING = 2,
        LOG_ERROR = 4,
        LOG_VERBOSE_MESSAGE = 8
    };

    class Logger
    {
    public:
        Logger();
        ~Logger();

        bool EnableConsoleColors = true;

        void Log(const std::string& log, const LOG_TYPE type) const;
        void VerboseMessage(const std::string& message);
        void Message(const std::string& message) const;
        void Warning(const std::string& warning) const;
        void Error(const std::string& error) const;

        void ChangeLogsVisibilityMask(const LOG_TYPE type, const bool visibility);

        NODISCARD inline uint8_t GetVisibilityMask() const { return m_VisibilityMask; }
    private:
        NODISCARD static std::string GetFormattedTime();
        NODISCARD static const char* GetColorCode(const LOG_TYPE type);
        uint8_t m_VisibilityMask;

        static const char* CONSOLE_COLOR_DEFAULT;
        static const char* CONSOLE_COLOR_GREEN;
        static const char* CONSOLE_COLOR_YELLOW;
        static const char* CONSOLE_COLOR_RED;

        static void ChangeConsoleTextColor(const char* colorCode);
    };

}