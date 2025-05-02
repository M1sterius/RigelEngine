#pragma once

#include "Core.hpp"
#include "ITypeRegistrable.hpp"

#include <memory>
#include <functional>

#define RIGEL_REGISTER_TYPE(Namespace, ClassName) \
    NODISCARD const char* GetTypeName() const override { return #Namespace "::" #ClassName; } \
    friend struct Rigel::TypeRegistry::Registrar<Namespace::ClassName>; \
    inline static Rigel::TypeRegistry::Registrar<Namespace::ClassName> ClassName##_registrar = \
        Rigel::TypeRegistry::Registrar<Namespace::ClassName>(#Namespace "::" #ClassName)

#define RIGEL_REGISTER_TYPE_NO_NAMESPACE(ClassName) \
    NODISCARD const char* GetTypeName() const override { return #ClassName; } \
    friend struct Rigel::TypeRegistry::Registrar<ClassName>; \
    inline static Rigel::TypeRegistry::Registrar<ClassName> ClassName##_registrar = \
        Rigel::TypeRegistry::Registrar<ClassName>(#ClassName)

namespace Rigel
{
    class TypeRegistry 
    {
    public:
        using RegisterTypeFunc = std::function<ITypeRegistrable*()>;

        template<typename T>
        struct Registrar
        {
            explicit Registrar(const std::string& name)
            {
                GetRegistry()[name] = []() -> ITypeRegistrable* { return new T(); };
            }
        };

        NODISCARD static std::unique_ptr<ITypeRegistrable> FindType(const std::string& type)
        {
            for (const auto& [name, instantiator] : GetRegistry())
            {
                if (name == type)
                    return std::unique_ptr<ITypeRegistrable>(instantiator());
            }

            return nullptr;
        }
    private:
        static std::unordered_map<std::string, RegisterTypeFunc>& GetRegistry()
        {
            static auto registry = std::unordered_map<std::string, RegisterTypeFunc>();
            return registry;
        }
    };
}
