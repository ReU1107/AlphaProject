#pragma once
#include "Vector.h"

namespace Alpha
{
	struct AxisAlignedBoundingBox
	{
		float3 min{};
		float3 max{};

        float3 GetCenter() const
        {
            return (min + max) * 0.5f;
        }

        float3 GetExtents() const
        {
            return (max - min) * 0.5f;
        }
	};
}