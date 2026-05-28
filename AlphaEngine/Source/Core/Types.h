#pragma once
#include <cstdint>
#include <cstddef>

namespace Alpha
{
    // --- 整数型 (Fixed-width integers) ---
    using UInt8 = uint8_t;
    using UInt16 = uint16_t;
    using UInt32 = uint32_t;
    using UInt64 = uint64_t;

    using Int8 = int8_t;
    using Int16 = int16_t;
    using Int32 = int32_t;
    using Int64 = int64_t;

    // --- 浮動小数点型 (Floating-point types) ---
    // 浮動小数点はdouble使うことないと思うし
    using Float = float;

    using Bool = bool;

    using USize = size_t;
}