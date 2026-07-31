#include "TransformComponent.h"
#include "Core/Math/MathF.h"

namespace Alpha
{
	void Alpha::TransformComponent::SetLocalPosition(const Vector3& pos)
	{
		m_position = pos;
		m_is_dirty = true;
	}

	void Alpha::TransformComponent::SetLocalScale(const Vector3& scale_)
	{
		m_scale = scale_;
		m_is_dirty = true;
	}

	void Alpha::TransformComponent::SetLocalRotation(const Vector3& euler)
	{
		m_rotation = QuaternionRollPitchYaw(euler);
		m_is_dirty = true;
	}

	void Alpha::TransformComponent::SetLocalRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_is_dirty = true;
	}

	const Matrix4x4& TransformComponent::GetLocalToWorld() const
	{
		if (m_is_dirty)
		{
			m_local_to_world = MatrixMultiply(Matrix4x4Scaling(m_scale), Matrix4x4Rotation(m_rotation));

			m_local_to_world._4() = m_position;

			m_is_dirty = false;
		}

		return m_local_to_world;
	}

	void TransformComponent::LocalTranslate(const Vector3& displacement)
	{
		m_position += displacement;
		m_is_dirty = true;
	}

	void TransformComponent::LocalRotate(const Vector3& euler)
	{
		m_rotation *= QuaternionRollPitchYaw(euler);
		m_is_dirty = true;
	}

	void TransformComponent::LocalRotate(const Vector3& axis, float angle)
	{
		m_rotation *= QuaternionRotationAxis(axis, angle);
		m_is_dirty = true;
	}

	void TransformComponent::LocalRotate(const Quaternion& rotation)
	{
		m_rotation *= rotation;
		m_is_dirty = true;
	}

	void TransformComponent::LookAt(const Vector3& dir, const Vector3& up)
	{
		m_rotation = QuaternionRotationLookAt(dir, up);
		m_is_dirty = true;
	}

	void TransformComponent::RotateAround(const Vector3& point, const Vector3& axis, float radian)
	{
	}

	Vector3 TransformComponent::Forward() const
	{
		return GetLocalToWorld()._3();
	}

	Vector3 TransformComponent::Backward() const
	{
		return GetLocalToWorld()._3() * -1.0f;
	}

	Vector3 TransformComponent::Right() const
	{
		return GetLocalToWorld()._1();
	}

	Vector3 TransformComponent::Left() const
	{
		return GetLocalToWorld()._1() * -1.0f;
	}

	Vector3 TransformComponent::Up() const
	{
		return GetLocalToWorld()._2();
	}

	Vector3 TransformComponent::Down() const
	{
		return GetLocalToWorld()._2() * -1.0f;
	}
}

