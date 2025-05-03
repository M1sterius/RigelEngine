#pragma once

#include "Core.hpp"
#include "ITypeRegistrable.hpp"

#include <memory>
#include <functional>

/**
 * @brief Registers a type with the Rigel type reflection system.
 *
 * This macro must be used inside the class declaration. Access qualifier doesn't matter.
 * A class using this macro must have a default constructor and be derived from ITypeRegistrable interface.
 *
 * @param Type The fully qualified, including namespaces, type to register.
 */
#define RIGEL_REGISTER_TYPE(Type) \
    NODISCARD const char* GetTypeName() const override { return #Type; } \
    friend struct Rigel::TypeRegistry::Registrar<Type>; \
    inline static Rigel::TypeRegistry::Registrar<Type> _type_registrar_ = \
    Rigel::TypeRegistry::Registrar<Type>(#Type)

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
