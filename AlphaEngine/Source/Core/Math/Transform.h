#pragma once
#include "Quaternion.h"
#include "Matrix.h"

namespace Alpha
{
	struct Transform
	{
		Vector3 position{ Vector3::Zero };
		Quaternion quaternion{ Quaternion::Identity };
		Vector3 scale{ Vector3::One };
	};
}