#include "TransformComponent.h"
namespace Alpha
{
	void Alpha::TransformComponent::SetLocalPosition(const Vector3& pos)
	{
		position = pos;
		is_dirty = true;
	}

	void Alpha::TransformComponent::SetWorldPosition(const Vector3& position)
	{
	}

	void Alpha::TransformComponent::SetLocalScale(const Vector3& scale)
	{
	}

	void Alpha::TransformComponent::SetLocalRotation(const Vector3& euler)
	{
	}

	void Alpha::TransformComponent::SetLocalRotation(const Quaternion& rotation)
	{
	}
	const Matrix4x4& TransformComponent::GetLocalToWorld() const
	{

		local_to_world._4() = position;

		return local_to_world;
	}
}

