#include "Random.hpp"

namespace Rigel
{
    std::string Random::UUIDv4()
    {
        const auto alphabet = "0123456789abcdef";
        constexpr bool dashes[] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

        std::string res;
        for (uint32_t i = 0; i < 16; i++)
        {
            if (dashes[i])
                res += '-';
            res += alphabet[Numeric(0, 15)];
            res += alphabet[Numeric(0, 15)];
        }

        // setting uuid version (v4) and variant
        res[14] = '4';
        res[19] = 'a';

        return res;
    }
}
