#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "Debug.hpp"

#ifdef RIGEL_PLATFORM_WINDOWS
    // this define removes global legacy windows min/max macros that break everything when used with pch
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include "Windows.h"
#endif
