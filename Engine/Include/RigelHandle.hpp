#pragma once

#include "Core.hpp"

namespace Rigel
{
    /**
     * Base class for handles to objects managed by Rigel engine and it's subsystems
     * @tparam T The types of the underlying object pointer
     */
    template<typename T>
    class RigelHandle
    {
    public:
        T* operator -> ()
        {
            CheckHandle();
            return m_Ptr;
        }

        const T* operator -> () const
        {
            CheckHandle();
            return m_Ptr;
        }

        NODISCARD uid_t GetID() const { return m_ID; }

        virtual ~RigelHandle() = default;
    protected:
        RigelHandle(T* ptr, const uid_t id) : m_Ptr(ptr), m_ID(id) { }

        /**
        * Returns true if at least one of the handle members (e.g. the underlying pointer,
        * UID or any other data specific to a certain handle type) is null.
        */
        NODISCARD virtual bool IsNull() const = 0;

        /**
        * Will be false if a handle is left in an invalid state that cannot be detected by nullity.
        *
        * The exact conditions that can put a handle in an invalid state may vary drastically
        * depending on its type. Validating may be costly for some types (especially so for Rigel::Component and Rigel::GameObject)
        */
        NODISCARD virtual bool IsValid() const = 0;

        T* m_Ptr = nullptr;
        uid_t m_ID = NULL_ID;
    private:
        void CheckHandle() const
        {
            if (IsNull())
                throw RigelException("Attempted to dereference a null Rigel handle!");

            #ifdef RGE_ENABLE_HANDLE_VALIDATION
            if (!IsValid())
                throw RigelException("Attempted to dereference an invalid Rigel handle!");
            #endif
        }
    };
}
