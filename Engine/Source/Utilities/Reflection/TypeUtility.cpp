#include "TypeUtility.hpp"

#include <regex>

#if defined(__GNUC__) || defined(__clang__)
    #include <cxxabi.h>
    #include <cstdlib>
#endif

namespace Rigel
{
    std::string TypeUtility::GetTypeName(const std::type_info& typeInfo)
    {
        return DemangleType(typeInfo);
    }

    std::string TypeUtility::DemangleType(const std::type_info& typeInfo)
    {
        #if defined(__GNUC__) || defined(__clang__)
            return Backend::TypeUtility::DemangleType_Impl_GCC_CLANG(typeInfo);
        #elif defined(_MSC_VER)
            return Backend::TypeUtility::DemangleType_Impl_MSVC(typeInfo);
        #else
        #error "Unsupported compiler: Rigel engine requires MSVC, GCC, or Clang."
        #endif
    }

    namespace Backend::TypeUtility
    {
        std::string DemangleType_Impl_GCC_CLANG(const std::type_info& typeInfo)
        {
            std::string result;

            #if defined(__GNUC__) || defined(__clang__)
            int status = 0;
            const char* rawName = typeInfo.name();
            char* demangled = abi::__cxa_demangle(rawName, nullptr, nullptr, &status);

            result = (status == 0 && demangled) ? demangled : rawName;
            std::free(demangled);
            #endif

            return result;
        }

        std::string DemangleType_Impl_MSVC(const std::type_info& typeInfo)
        {
            std::string name = typeInfo.name();
            name = std::regex_replace(name, std::regex(R"(class\s+|struct\s+|enum\s+|union\s+)"), "");
            return name;
        }
    }
}
