#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

#include <atomic>

namespace Rigel
{
    namespace Backend::AssetHandleUtilityImpl
    {
        void OnRefCountReachZero(const uid_t id);
    }

    class RigelAsset;

    template<typename T> requires std::is_base_of_v<RigelAsset, T>
    class AssetHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD const char* GetTypeName() const override
        {
            return TypeUtility::GetTypeName<AssetHandle>().c_str();
        }

        AssetHandle() noexcept : RigelHandle<T>(nullptr, NULL_ID), m_RefCounter(nullptr) { }

        AssetHandle(T* ptr, const uid_t id, std::atomic<uint32_t>* refCounter) noexcept
            : RigelHandle<T>(ptr, id), m_RefCounter(refCounter) { }

        AssetHandle(const AssetHandle& other) noexcept
            : RigelHandle<T>(other)
        {
            m_RefCounter = other.m_RefCounter;
            if (m_RefCounter) ++(*m_RefCounter);
        }

        AssetHandle& operator = (const AssetHandle& other) noexcept
        {
            if (this != &other)
            {
                RigelHandle<T>::operator = (other);

                m_RefCounter = other.m_RefCounter;
                if (m_RefCounter) ++(*m_RefCounter);
            }

            return *this;
        }

        ~AssetHandle() override
        {
            if (m_RefCounter && !IsNull())
            {
                if (--(*m_RefCounter) == 0)
                {
                    Backend::AssetHandleUtilityImpl::OnRefCountReachZero(this->GetID());
                }
            }
        }

        T* operator -> () override
        {
            this->CheckHandle();
            return this->m_Ptr;
        }

        const T* operator -> () const override
        {
            this->CheckHandle();
            return this->m_Ptr;
        }

        NODISCARD uint32_t GetRefCount() const { return *m_RefCounter; }

        template<typename castT>
        NODISCARD AssetHandle<castT> Cast() const
        {
            // this method creates a new handle instance bypassing all copy constructors,
            // so we increment the ref counter by hand to keep it accurate

            static_assert(std::is_base_of_v<RigelAsset, castT>, "T must derive from Rigel::RigelAsset");
            if (m_RefCounter) ++(*m_RefCounter);
            return {static_cast<castT*>(this->m_Ptr), this->m_ID, this->m_RefCounter};
        }

        NODISCARD static AssetHandle Null() { return {nullptr, NULL_ID, nullptr}; }
        NODISCARD bool IsNull() const override { return this->m_Ptr == nullptr || this->m_ID == NULL_ID;}
        NODISCARD bool IsValid() const override
        {
            using namespace Backend::HandleValidation;
            return HandleValidator::Validate<HandleType::AssetHandle>(this->GetID());
        }
    private:
        std::atomic<uint32_t>* m_RefCounter;
    };

    using GenericAssetHandle = AssetHandle<RigelAsset>;
}
