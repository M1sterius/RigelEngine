#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"
#include "HandleValidator.hpp"

namespace Rigel
{
    class Component;

    template<typename T> requires std::is_base_of_v<Component, T>
    class ComponentHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD const char* GetTypeName() const override
        {
            return TypeUtility::GetTypeName<RigelHandle<T>>().c_str();
        }

        ComponentHandle() : RigelHandle<T>(nullptr, NULL_ID) { }
        ComponentHandle(T* ptr, const uid_t id) : RigelHandle<T>(ptr, id) { }

        NODISCARD static ComponentHandle Null()
        {
            return {nullptr, NULL_ID};
        }

        NODISCARD bool IsNull() const override
        {
            return this->m_Ptr == nullptr || this->m_ID == NULL_ID;
        }

        NODISCARD bool IsValid() const override
        {
            using namespace Backend::HandleValidation;
            return HandleValidator::Validate<HandleType::ComponentHandle>(this->GetID());
        }
    };
}
