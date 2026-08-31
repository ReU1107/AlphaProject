#pragma once
#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Quaternion.h"

namespace Alpha
{
	struct TransformComponent
	{

	private:
		mutable Matrix4x4 m_local_matrix{ Matrix4x4::Identity };
		Matrix4x4 m_world_matrix{ Matrix4x4::Identity };

		Quaternion m_rotation{ Quaternion::Identity };

		Vector3 m_position{ Vector3::Zero };
		Vector3 m_scale{ Vector3::One };

		// ダーティパターン
		mutable bool m_is_dirty{ true };

	public:
		void SetLocalPosition(const Vector3& position);

		void SetLocalScale(const Vector3& scale);

		void SetLocalRotation(const Vector3& euler);
		void SetLocalRotation(const Quaternion& rotation);

		const Matrix4x4& GetLocalMatrix() const;

		// ローカル空間での平行移動
		void LocalTranslate(const Vector3& displacement);

		// ローカル空間での回転
		void LocalRotate(const Vector3& euler);
		void LocalRotate(const Vector3& axis, float angle);
		void LocalRotate(const Quaternion& rotation);

		// ターゲットの方向を見るように回転
		void LookAt(const Vector3& dir, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f));

		void RotateAround(const Vector3& point, const Vector3& axis, float radian);

		void SetWorldMatrix(const Matrix4x4& mat);
		const Matrix4x4& GetWorldMatrix() const;

	};
}