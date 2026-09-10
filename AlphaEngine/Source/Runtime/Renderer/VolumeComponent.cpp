#include "VolumeComponent.h"
#include "Core/Math/MathUtil.h"
#include "Core/Math/MathF.h"

namespace Alpha::Renderer
{
	AxisAlignedBoundingBox VolumeComponent::GetAABB() const
	{
		Vector3 half_size = GetSize() * 0.5f;
		AxisAlignedBoundingBox aabb = {};
		aabb.min = Vector3::Zero - half_size;
		aabb.max = Vector3::Zero + half_size;

		return aabb;
	}

}