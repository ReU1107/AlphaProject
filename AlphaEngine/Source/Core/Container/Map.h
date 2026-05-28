#pragma once
#include "Types.h"
#include <unordered_map>

namespace Alpha
{
	template<typename K, typename V, typename Hash = std::hash<K>, typename Eq = std::equal_to<K>>
	using TMap = std::unordered_map<K, V, Hash, Eq>;

}