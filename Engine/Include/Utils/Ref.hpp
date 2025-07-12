#pragma once

#include "Assert.hpp"

namespace Rigel
{
    /**
     * A small non-owning wrapper around a raw pointer
     * @tparam T Pointer type
     */
    template<typename T>
    class Ref
    {
    public:
        Ref(T* ptr) : m_Ptr(ptr) { }
        Ref() : m_Ptr(nullptr) { }

        T* operator -> ()
        {
            ASSERT(m_Ptr, "Ref pointer was a nullptr!");
            return m_Ptr;
        }

        const T* operator -> () const
        {
            ASSERT(m_Ptr, "Ref pointer was a nullptr!");
            return m_Ptr;
        }

        T& operator * () const
        {
            ASSERT(m_Ptr, "Ref pointer was a nullptr!");
            return *m_Ptr;
        }

        [[nodiscard]] inline bool operator == (const Ref other) const
        {
            return this->m_Ptr == other.m_Ptr;
        }

        [[nodiscard]] inline bool operator != (const Ref other) const
        {
            return this->m_Ptr != other.m_Ptr;
        }

        [[nodiscard]] inline bool IsNull() const { return !m_Ptr; }
    private:
        T* m_Ptr = nullptr;
    };
}
