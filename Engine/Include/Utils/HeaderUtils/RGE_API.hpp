#pragma once

#ifdef RIGEL_INTERNAL
    #define RGE_API __declspec(dllexport)
#else
    #define RGE_API __declspec(dllimport)
#endif
