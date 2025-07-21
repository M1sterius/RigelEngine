#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"
#include "AssetDeleter.hpp"

#include <memory>

namespace Rigel
{
    class RigelAsset;

    template<typename T> requires std::is_base_of_v<RigelAsset, T>
    class AssetHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD const char* GetTypeName() const override
        {
            return TypeUtility::GetTypeName<AssetHandle>().c_str();
        }

        AssetHandle() noexcept
            : RigelHandle<T>(nullptr, NULL_ID), m_RefCounter(nullptr) { }

        AssetHandle(T* ptr, const uid_t id, const std::shared_ptr<Backend::AssetDeleter>& refCounter) noexcept
            : RigelHandle<T>(ptr, id), m_RefCounter(refCounter) { }

        ~AssetHandle() override = default;

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

        template<typename castT>
        NODISCARD AssetHandle<castT> Cast() const
        {
            static_assert(std::is_base_of_v<RigelAsset, castT>, "T must derive from Rigel::RigelAsset");
            return {static_cast<castT*>(this->m_Ptr), this->m_ID, this->m_RefCounter};
        }

        NODISCARD AssetHandle<RigelAsset> ToGeneric() const
        {
            return this->Cast<RigelAsset>();
        }

        NODISCARD static AssetHandle Null() { return {nullptr, NULL_ID, nullptr}; }
        NODISCARD bool IsNull() const override { return this->m_Ptr == nullptr || this->m_ID == NULL_ID || this->m_RefCounter == nullptr; }
        NODISCARD bool IsValid() const override
        {
            using namespace Backend::HandleValidation;
            return HandleValidator::Validate<HandleType::AssetHandle>(this->GetID());
        }
    private:
        std::shared_ptr<Backend::AssetDeleter> m_RefCounter;
    };

    using GenericAssetHandle = AssetHandle<RigelAsset>;
}
