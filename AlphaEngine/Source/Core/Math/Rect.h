#pragma once
#include "Types.h"

namespace Alpha
{
	struct Rect2D
	{
		Float x{ 0 };
		Float y{ 0 };
		Float width{ 1920 };
		Float height{ 1080 };
	};

	struct Rect3D
	{
		Float x{ 0 };
		Float y{ 0 };
		Float width{ 1920 };
		Float height{ 1080 };
		Float min_depth{ 0.0f };
		Float max_depth{ 1.0f };
	};
}