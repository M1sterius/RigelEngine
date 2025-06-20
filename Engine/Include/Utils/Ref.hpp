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
        Ref(T* ptr) : m_Ptr(ptr)
        {
            ASSERT(m_Ptr, "Rigel::Ref cannot contain nullptr!");
        }

        T* operator -> () { return m_Ptr; }
        const T* operator -> () const { return m_Ptr; }
        T& operator * () const { return *m_Ptr; }
    private:
        T* m_Ptr = nullptr;
    };
}