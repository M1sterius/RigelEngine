#pragma once

#include "Utils/Types.hpp"
#include "Utils/NODISCARD.hpp"

#include <stdexcept>

namespace rge
{
    template<typename T>
    class RigelHandle
    {
    public:
        inline T* operator -> ()
        {
            if (IsNull())
                throw std::runtime_error("A RigelHandle was a null handle!");
            return m_Ptr;
        }

        NODISCARD inline uid_t GetID() const { return m_ObjectID; }

        ~RigelHandle() = default;
    protected:
        T* m_Ptr = nullptr;
        uid_t m_ObjectID = NULL_ID;

        RigelHandle(T* ptr, const uid_t id) : m_Ptr(ptr), m_ObjectID(id) { }

        NODISCARD virtual bool IsValid() const = 0;
        NODISCARD virtual bool IsNull() const = 0;
    };
}
