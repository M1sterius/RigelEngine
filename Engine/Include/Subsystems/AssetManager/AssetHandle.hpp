#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

namespace rge
{
    template<typename T>
    class AssetHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD bool IsNull() const override
        {
            return this->m_Ptr == nullptr || this->m_ID == NULL_ID;
        }

        NODISCARD bool IsValid() const override
        {
            return true; // TODO: Implement proper validation
        }

        AssetHandle(T* ptr, const uid_t id) : RigelHandle<T>(ptr, id) { }
    };
}
