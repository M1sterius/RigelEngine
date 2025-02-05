#pragma once

#include "Event.hpp"

namespace rge
{
    class IEventListener
    {
    public:
        virtual ~IEventListener() = default;
        virtual void OnEvent(const Event& event) = 0;
    };

}
