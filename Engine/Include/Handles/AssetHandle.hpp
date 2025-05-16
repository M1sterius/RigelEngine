#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

#include <atomic>

namespace Rigel
{
    namespace Backend::AssetHandleRefCounter
    {

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

        AssetHandle() noexcept : RigelHandle<T>(nullptr, NULL_ID), m_RefCount(nullptr) { }

        AssetHandle(T* ptr, const uid_t id, std::atomic<uint32_t>* refCounter) noexcept
            : RigelHandle<T>(ptr, id), m_RefCount(refCounter) { }

        AssetHandle(const AssetHandle& other) noexcept
            : RigelHandle<T>(other)
        {
            m_RefCount = other.m_RefCount;
            ++(*m_RefCount);
        }

        AssetHandle& operator = (const AssetHandle& other) noexcept
        {
            if (this != &other)
            {
                RigelHandle<T>::operator = (other);

                m_RefCount = other.m_RefCount;
                ++(*m_RefCount);
            }

            return *this;
        }

        ~AssetHandle() override
        {
            if (m_RefCount && !IsNull())
            {
                if (--(*m_RefCount) == 0)
                {
                    Debug::Message("Should delete asset with ID {}!", this->GetID());
                }
            }
        }

        NODISCARD uint32_t GetRefCount() const { return *m_RefCount; }

        template<typename castT>
        NODISCARD AssetHandle<castT> Cast() const
        {
            static_assert(std::is_base_of_v<RigelAsset, castT>, "T must derive from Rigel::RigelAsset");
            return {static_cast<castT*>(this->m_Ptr), this->m_ID};
        }

        NODISCARD static AssetHandle Null() { return {nullptr, NULL_ID, nullptr}; }
        NODISCARD bool IsNull() const override { return this->m_Ptr == nullptr || this->m_ID == NULL_ID;}
        NODISCARD bool IsValid() const override
        {
            using namespace Backend::HandleValidation;
            return HandleValidator::Validate<HandleType::AssetHandle>(this->GetID());
        }
    private:
        std::atomic<uint32_t>* m_RefCount;
    };
}
