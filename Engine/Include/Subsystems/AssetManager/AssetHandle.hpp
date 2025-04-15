#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

namespace Rigel
{
    namespace Backend::HandleValidation
    {
        NODISCARD bool ValidateAssetHandleImpl(const uid_t id);
    }

    namespace Backend::RefCountingImpl
    {
        void IncrementRefCount(const uid_t id);
        void DecrementRefCount(const uid_t id);
    }

    class RigelAsset;

    // TODO: Implement reference counting logic, add asset path a field
    template<typename T> requires std::is_base_of_v<RigelAsset, T>
    class AssetHandle final : public RigelHandle<T>
    {
    public:
        AssetHandle() : RigelHandle<T>(nullptr, NULL_ID) { }
        AssetHandle(T* ptr, const uid_t id) : RigelHandle<T>(ptr, id)
        {
            Backend::RefCountingImpl::IncrementRefCount(id);
        }

        ~AssetHandle() override
        {
            Backend::RefCountingImpl::DecrementRefCount(this->GetID());
        }

        AssetHandle(const AssetHandle& handle) : RigelHandle<T>(handle)
        {
            Backend::RefCountingImpl::IncrementRefCount(this->GetID());
        }

        AssetHandle& operator = (const AssetHandle&)
        {
            Backend::RefCountingImpl::IncrementRefCount(this->GetID());
            return *this;
        }

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
            return Backend::HandleValidation::ValidateAssetHandleImpl(this->GetID());
        }
    };
}
