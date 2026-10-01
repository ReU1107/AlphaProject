#pragma once
#include "Types.h"

namespace Alpha
{
	// 限りなく0に近い値
	static constexpr Float Epsilon = 1e-6f;
	// 無限小数
	static constexpr Float Infinity = 3.402823466e+38f;
	// 負の無限小数
	static constexpr Float NegativeInfinity = -Infinity;
	// 円周率
	static constexpr Float PI = 3.14159265359f;
	// 
	static constexpr Float PI2 = PI * 2.0f;
	// 
	static constexpr Float PIDiv2 = PI / 2.0f;

	static constexpr UInt32 MaxUInt32 = 0xffffffff;

	static constexpr UInt64 MaxUInt64 = 0xffffffffffffffffULL;

	inline constexpr UInt64 operator"" _B(unsigned long long n) { return static_cast<UInt64>(n); }
	inline constexpr UInt64 operator"" _KB(unsigned long long n) { return static_cast<UInt64>(n * 1024); }
	inline constexpr UInt64 operator"" _MB(unsigned long long n) { return static_cast<UInt64>(n * 1024 * 1024); }
	inline constexpr UInt64 operator"" _GB(unsigned long long n) { return static_cast<UInt64>(n * 1024 * 1024 * 1024); }

	inline constexpr Float operator"" _s(long double n) { return static_cast<Float>(n); }
	inline constexpr Float operator"" _s(unsigned long long n) { return static_cast<Float>(n); }

	inline constexpr Float operator"" _ms(long double n) { return static_cast<Float>(n) / 1000.0f; }
	inline constexpr Float operator"" _ms(unsigned long long n) { return static_cast<Float>(n) / 1000.0f; }

	inline constexpr Float operator"" _min(long double n) { return static_cast<Float>(n) * 60.0f; }
	inline constexpr Float operator"" _min(unsigned long long n) { return static_cast<Float>(n) * 60.0f; }

	inline constexpr Float operator"" _us(long double n) { return static_cast<Float>(n) / 1000000.0f; }
	inline constexpr Float operator"" _us(unsigned long long n) { return static_cast<Float>(n) / 1000000.0f; }

}