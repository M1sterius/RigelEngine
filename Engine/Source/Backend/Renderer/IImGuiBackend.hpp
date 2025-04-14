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

        virtual void RenderFrame(void* optData) = 0;
        virtual void BeginNewFrame() = 0;
    protected:
        IImGuiBackend() = default;
    };
}
