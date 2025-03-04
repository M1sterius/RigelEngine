#include "Exceptions.hpp"
#include "Debug.hpp"

namespace rge
{
    RigelError::RigelError(const std::string& what) : runtime_error(what)
    {
        Debug::Error(what);
    }
}
