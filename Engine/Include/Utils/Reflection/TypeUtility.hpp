#pragma once

#define TYPE_EQUAL(T1, T2) (typeid(T1) == typeid(T2))
#define TYPE_INDEX(T) (std::type_index(typeid(T)))

#include <string>
#include <typeinfo>

namespace Rigel
{
    class TypeUtility
    {
    public:
        static std::string GetTypeName(const std::type_info& typeInfo);

        template<typename T>
        static std::string GetTypeName()
        {
            return GetTypeName(typeid(T));
        }

        static std::string DemangleType(const std::type_info& typeInfo);
    };

    namespace Backend::TypeUtility
    {
        std::string DemangleType_Impl_GCC_CLANG(const std::type_info& typeInfo);
        std::string DemangleType_Impl_MSVC(const std::type_info& typeInfo);
    }
}
