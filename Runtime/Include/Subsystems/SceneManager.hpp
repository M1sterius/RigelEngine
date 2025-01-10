#pragma once

#include "SubsystemBase.hpp"

namespace rge
{
    class SceneManager final : public SubsystemBase
    {
    public:
        void Foo();

        NODISCARD inline static SceneManager* Get() { return m_GlobalInstance; }
    INTERNAL:
        void Startup() override;
        void Shutdown() override;

        static void AssignGlobalInstance(SceneManager* instance) { m_GlobalInstance = instance; }
    private:
        static SceneManager* m_GlobalInstance;
    };
}
