#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"
#include "HandleValidator.hpp"

namespace Rigel
{
    class Component;

    template<typename T>
    class ComponentHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD const char* GetTypeName() const override { return TypeUtility::GetTypeName<ComponentHandle>().c_str(); }

        ComponentHandle() : RigelHandle<T>(nullptr, NULL_ID)
        {
            // we can't use 'requires std::is_base_of_v<Component, T>' because it would make using
            // handles of type T inside T declaration impossible
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Rigel::Component");
        }
        ComponentHandle(T* ptr, const uid_t id) : RigelHandle<T>(ptr, id)
        {
            // we can't use 'requires std::is_base_of_v<Component, T>' because it would make using
            // handles of type T inside T declaration impossible
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Rigel::Component");
        }

        //
        template<typename castT>
        NODISCARD ComponentHandle<castT> Cast() const
        {
            static_assert(std::is_base_of_v<Component, castT>, "T must derive from Rigel::Component");
            return {static_cast<castT*>(this->m_Ptr), this->m_ID};
        }

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

    // Handle to a component of any type
    using GenericComponentHandle = ComponentHandle<Component>;
}
