#pragma once
#include "Types.h"
#include <array>
#include <vector>

namespace Alpha
{
	template<typename T, UInt32 N>
	using TFixedArray = std::array<T, N>;

	template<typename T>
	using TArray = std::vector<T>;

}