#pragma once
#include "Core/Common.h"

namespace Alpha
{
	// 投影法
	enum class ProjectionType
	{
		Orthographic,	// 平行投影法
		Perspective		// 透視投影法
	};

	// 一旦全部持たせる　いずれcomponentにデータを逃がす

	class Camera
	{
	private:
		mutable Matrix4x4 m_view_matrix{ matrix4x4::Identity };
		mutable Matrix4x4 m_projection_matrix{ matrix4x4::Identity };
		Vector3 m_position{ 0.0f, 0.0f, 0.0f };
		Vector3 m_rotation{ 0.0f, 0.0f, 0.0f };
		float m_fov{ 60.0f };
		float m_aspect_ratio{ 1920.0f / 1080.0f };
		float m_near_plane{ 0.1f };
		float m_far_plane{ 1000.0f };

	public:
		Camera() = default;
		~Camera() = default;

		void SetPosition(const Vector3& position) { m_position = position; }
		void SetRotation(const Vector3& rotation) { m_rotation = rotation; }
		void SetFOV(float fov) { m_fov = fov; }
		void SetAspectRatio(float aspect_ratio) { m_aspect_ratio = aspect_ratio; }
		void SetNearPlane(float near_plane) { m_near_plane = near_plane; }
		void SetFarPlane(float far_plane) { m_far_plane = far_plane; }

		Matrix4x4 GetViewProjectionMatrix() const;


	};
}