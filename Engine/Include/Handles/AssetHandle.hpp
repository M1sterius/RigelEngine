#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

namespace Rigel
{
    namespace Backend::RefCountingImpl
    {
        void IncrementRefCount(const uid_t id);
        void DecrementRefCount(const uid_t id);
    }

    class RigelAsset;

    template<typename T> requires std::is_base_of_v<RigelAsset, T>
    class AssetHandle final : public RigelHandle<T>
    {
    public:
        AssetHandle() : RigelHandle<T>(nullptr, NULL_ID) { }
        AssetHandle(T* ptr, const uid_t id) : RigelHandle<T>(ptr, id)
        {
            Backend::RefCountingImpl::IncrementRefCount(id);
        }

        AssetHandle(const AssetHandle& other) : RigelHandle<T>(other)
        {
            Backend::RefCountingImpl::IncrementRefCount(other.m_ID);
        }

        AssetHandle& operator = (const AssetHandle& other)
        {
            if (this != &other) // checks that we are not trying to assign an object to itself
            {
                Backend::RefCountingImpl::IncrementRefCount(other.GetID());
                RigelHandle<T>::operator = (other);
            }
            return *this;
        }

        ~AssetHandle() override { Backend::RefCountingImpl::DecrementRefCount(this->GetID()); }

        NODISCARD static AssetHandle Null() { return {nullptr, NULL_ID}; }
        NODISCARD bool IsNull() const override { return this->m_Ptr == nullptr || this->m_ID == NULL_ID;}

        NODISCARD bool IsValid() const override
        {
            using namespace Backend::HandleValidation;
            return HandleValidator::Validate<HandleType::AssetHandle>(this->GetID());
        }
    };
}
