#pragma once
#include "Core/Common.h"

namespace Alpha::Renderer
{
	// 投影法
	enum class ProjectionType
	{
		Orthographic,	// 平行投影法
		Perspective		// 透視投影法
	};

	struct CameraComponent
	{
		Matrix4x4 view_matrix{ matrix4x4::Identity };

	private:
		mutable Matrix4x4 m_projection_matrix{ matrix4x4::Identity };

		float m_fov{ 60.0f };
		float m_aspect_ratio{ 1920.0f / 1080.0f };
		float m_near_plane{ 0.1f };
		float m_far_plane{ 1000.0f };
		ProjectionType m_type{ ProjectionType::Perspective };
		mutable bool m_is_projection_dirty{ true };

	public:

		void SetFOV(float fov);
		void SetAspectRatio(float aspect_ratio);
		void SetNearPlane(float near_plane);
		void SetFarPlane(float far_plane);

		Matrix4x4 GetProjectionMatrix() const;


	};
}