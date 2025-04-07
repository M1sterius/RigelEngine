#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

namespace rge
{
    namespace backend
    {
        NODISCARD bool ValidateAssetHandleImpl(const uid_t id);
    }

    class RigelAsset;

    // TODO: Implement reference counting logic
    template<typename T> requires std::is_base_of_v<RigelAsset, T>
    class AssetHandle final : public RigelHandle<T>
    {
    public:
        AssetHandle() : RigelHandle<T>(nullptr, NULL_ID) { }
        AssetHandle(T* ptr, const uid_t id) : RigelHandle<T>(ptr, id) { }

        NODISCARD static AssetHandle Null()
        {
            return {nullptr, NULL_ID};
        }

        NODISCARD bool IsNull() const override
        {
            return this->m_Ptr == nullptr || this->m_ID == NULL_ID;
        }

        NODISCARD bool IsValid() const override
        {
            return backend::ValidateAssetHandleImpl(this->GetID());
        }
    };
}
