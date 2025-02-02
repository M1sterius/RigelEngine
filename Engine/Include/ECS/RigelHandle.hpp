#pragma once

#include "HeaderUtils.hpp"

#include <stdexcept>

namespace rge
{
    template<typename T>
    class RGE_API RigelHandle
    {
    public:
        inline T* operator -> ()
        {
            CheckHandle();
            return m_Ptr;
        }

        inline const T* operator -> () const
        {
            CheckHandle();
            return m_Ptr;
        }

        NODISCARD uid_t GetID() const { return m_ID; }

        ~RigelHandle() = default;
    protected:
        RigelHandle(T* ptr, const uid_t id) : m_Ptr(ptr), m_ID(id) { }

        NODISCARD virtual bool IsValid() const = 0;
        NODISCARD virtual bool IsNull() const = 0;

        T* m_Ptr = nullptr;
        uid_t m_ID = NULL_ID;
    private:
        void CheckHandle() const
        {
            const auto res = !IsNull() && IsValid();
            if (!res) throw std::runtime_error("Attempted to use an invalid RigelHandle instance!");
        }
    };
}
