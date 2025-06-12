#pragma once

#include "Core.hpp"

namespace Rigel
{
    /**
     * Calls function F when the instance of ScopeGuard goes out of scope
     * @tparam F Function signature
     */
    template<typename F>
    class ScopeGuard
    {
    public:
        explicit ScopeGuard(F&& func) : m_Function(std::forward<F>(func)), m_Active(true) { }
        ~ScopeGuard() { if (m_Active) m_Function(); }

        void Dismiss() { m_Active = false; }
    private:
        F m_Function;
        bool m_Active;
    };
}