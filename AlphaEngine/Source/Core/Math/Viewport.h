#pragma once
#include "Types.h"

namespace Alpha
{
	struct Viewport
	{
		Float x{ 0.0f };
		Float y{ 0.0f };
		Float width{ 1920.0f };
		Float height{ 1080.0f };
		Float min_depth{ 0.0f };
		Float max_depth{ 1.0f };
	};
}