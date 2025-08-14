#pragma once

#include "Core.hpp"

namespace Rigel
{
    class ITypeRegistrable
    {
    public:
        ITypeRegistrable() = default;
        virtual ~ITypeRegistrable() = default;

        NODISCARD virtual const char* GetTypeName() const = 0;
    };
}