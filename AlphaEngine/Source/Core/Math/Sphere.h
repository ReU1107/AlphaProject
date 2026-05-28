#pragma once
#include "Vector.h"

namespace Alpha
{
	struct Sphere
	{
		Vector3 center{ Vector3::Zero };
		Float radius{ 1.0f };
	};
}