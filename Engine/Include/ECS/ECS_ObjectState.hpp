#pragma once

namespace Rigel::Backend
{
    enum class ECS_ObjectState : int8_t
    {
        None = -1,
        Instantiated = 0,
        Loaded = 1,
        Ready = 2
    };
}
