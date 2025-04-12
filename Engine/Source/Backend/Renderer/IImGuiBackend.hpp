#pragma once

#include "Core.hpp"

namespace Rigel::Backend
{
    class IImGuiBackend
    {
    public:
        virtual ~IImGuiBackend() = default;

        IImGuiBackend(const IImGuiBackend& other) = delete;
        IImGuiBackend& operator = (const IImGuiBackend&) = delete;
    protected:
        IImGuiBackend() = default;
    };
}
