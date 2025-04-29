#pragma once

#include "Core.hpp"

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace Rigel
{
    class Component;

    /**
     * Helper class for deducing types of components from strings
     *
     * Every component based on Rigel::Component must be registered in declaration by using REGISTER_COMPONENT macro
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

        NODISCARD static std::unique_ptr<Component> FindType(const std::string& type);
    private:
        inline static std::unordered_map<std::string, RegisterTypeFunc>& GetRegistry()
        {
            static auto registry = std::unordered_map<std::string, RegisterTypeFunc>();
            return registry;
        }
    };

// TODO: Make it work with namespaces
#define RIGEL_REGISTER_COMPONENT(ClassName) \
    friend class Rigel::GameObject; \
    NODISCARD const char* GetTypeName() const override { return #ClassName; } \
    friend struct Rigel::ComponentTypeRegistry::Registrar<ClassName>; \
    inline static Rigel::ComponentTypeRegistry::Registrar<ClassName> ClassName##_registrar = Rigel::ComponentTypeRegistry::Registrar<ClassName>(#ClassName)
}
