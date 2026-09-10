#pragma once
#include "Core/Types.h"
#include <functional>
#include <type_traits>

namespace Alpha
{
    template <typename T>
    inline void HashCombine(UInt64& seed, const T& value)
    {
        const UInt64 hash = static_cast<UInt64>(std::hash<T>{}(value));
        seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename T>
    inline UInt64 HashEnum(T value)
    {
        static_assert(std::is_enum_v<T>);

        using UnderlyingType = std::underlying_type_t<T>;
        return static_cast<UInt64>(static_cast<UnderlyingType>(value));
    }

}