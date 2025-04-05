#pragma once

#include "Core.hpp"

#include <string>
#include <functional>
#include <unordered_map>

namespace rge
{
    class Component;

    /**
     * Helper class for deducing types of components from strings
     *
     * Every component based on rge::Component must be registered in declaration by using REGISTER_COMPONENT macro
     */
    class ComponentTypeRegistry final
    {
    public:
        using RegisterTypeFunc = std::function<Component*()>;

        template<typename T>
        struct Registrar
        {
            explicit Registrar(const std::string& name)
            {
                GetRegistry()[name] = []() -> Component* { return new T(); };
            }
        };

        static Component* FindType(const std::string& type);
    private:
        inline static std::unordered_map<std::string, RegisterTypeFunc>& GetRegistry()
        {
            static auto registry = std::unordered_map<std::string, RegisterTypeFunc>();
            return registry;
        }
    };

#define RGE_REGISTER_COMPONENT(ClassName) \
    NODISCARD const char* GetTypeName() const override { return #ClassName; } \
    friend struct rge::ComponentTypeRegistry::Registrar<ClassName>; \
    inline static rge::ComponentTypeRegistry::Registrar<ClassName> ClassName##_registrar = rge::ComponentTypeRegistry::Registrar<ClassName>(#ClassName);
}
