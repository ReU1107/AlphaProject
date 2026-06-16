#pragma once
#include "Types.h"
#include "SmartPtr.h"

#include "Allocator/LinearAllocator.h"

#include "String/String.h"

#include "Container/Array.h"
#include "Container/Map.h"

#include "Math/Color.h"
#include "Math/Matrix.h"
#include "Math/Rect.h"
#include "Math/Transform.h"
#include "Math/Vector.h"
#include "Math/Viewport.h"

#include "Literal.h"

#include "Assertion.h"

namespace Alpha
{
	template<typename T>
	using TUniqueArray = TArray<std::unique_ptr<T>>;

	template<typename T>
	using TSharedArray = TArray<std::shared_ptr<T>>;

	template<typename T>
	using TStringMap = TMap<String, T, StringHash, std::equal_to<>>;
}