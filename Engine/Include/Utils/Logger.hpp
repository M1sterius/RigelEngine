#pragma once

#include "NODISCARD.hpp"
#include "RGE_API.hpp"

#include <cstdint>
#include <string>

namespace rge
{
    enum LOG_TYPE : uint8_t
    {
        VERBOSE_MESSAGE = 1,
        MESSAGE = 2,
        WARNING = 4,
        ERROR = 8
    };

    class RGE_API Logger
    {
    public:
        static void Log(const std::string& log, const LOG_TYPE type);
        static void VerboseMessage(const std::string& message);
        static void Message(const std::string& message);
        static void Warning(const std::string& warning);
        static void Error(const std::string& error);

        static void ChangeLogsVisibilityMask(const LOG_TYPE type, const bool visibility);
        static void SetConsoleColorsVisibility(const bool visibility);

        NODISCARD static inline uint8_t GetVisibilityMask() { return VisibilityMask; }
    private:
        NODISCARD static std::string GetFormattedTime();
        NODISCARD static const char* GetColorCode(const LOG_TYPE type);
        static uint8_t VisibilityMask;
        static bool EnableConsoleColors;

        static const char* CONSOLE_COLOR_DEFAULT;
        static const char* CONSOLE_COLOR_GREEN;
        static const char* CONSOLE_COLOR_YELLOW;
        static const char* CONSOLE_COLOR_RED;

        static void ChangeConsoleTextColor(const char* colorCode);
    };
}
