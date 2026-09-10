#include "CameraComponent.h"
#include "Core/Math/MathUtil.h"
#include "Core/Math/MathF.h"


namespace Alpha::Renderer
{
	void CameraComponent::SetFOV(float fov)
	{
		m_fov = fov;
		m_is_projection_dirty = true;
	}

	void CameraComponent::SetAspectRatio(float aspect_ratio)
	{
		m_aspect_ratio = aspect_ratio;
		m_is_projection_dirty = true;
	}

	void CameraComponent::SetNearPlane(float near_plane)
	{
		m_near_plane = near_plane;
		m_is_projection_dirty = true;
	}

	void CameraComponent::SetFarPlane(float far_plane)
	{
		m_far_plane = far_plane;
		m_is_projection_dirty = true;
	}

	Matrix4x4 CameraComponent::GetProjectionMatrix() const
	{
		if (m_is_projection_dirty)
		{
			if (m_type == ProjectionType::Perspective)
			{
				m_projection_matrix = Matrix4x4PerspectiveFovLHReverseZ(m_fov, m_aspect_ratio, m_near_plane, m_far_plane);
			}
			else
			{
				//m_projection_matrix = Matrix4x4OrthographicLH(width, height, m_near_plane, m_far_plane);
			}
			m_is_projection_dirty = false;
		}

		return m_projection_matrix;
	}
}
/*
*  DirectXMathを使用して計算ライブラリがあってるか確認したやつ
* 		XMFLOAT3 a(eye.data());
		XMFLOAT3 b(Vector3(0, 0, 0).data());
		XMFLOAT3 c(Vector3(0, 1, 0).data());

		XMVECTOR eye_vec = XMLoadFloat3(&a);
		XMVECTOR focus_vec = XMLoadFloat3(&b);
		XMVECTOR up_vec = XMLoadFloat3(&c);

	
		//XMFLOAT4X4 a(view.data());

		XMMATRIX xm_view = XMMatrixLookAtLH(eye_vec, focus_vec, up_vec);

		XMMATRIX xm_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fov), m_aspect_ratio, m_near_plane, m_far_plane);

		XMFLOAT4X4 viewProjectionMatrix;;
		//XMStoreFloat4x4(&viewProjectionMatrix, XMMatrixMultiply(xm_view, xm_proj));


		XMStoreFloat4x4(&viewProjectionMatrix, xm_view);
				//Matrix4x4 abc(
		//	Vector4(viewProjectionMatrix._11, viewProjectionMatrix._12, viewProjectionMatrix._13, viewProjectionMatrix._14),
		//	Vector4(viewProjectionMatrix._21, viewProjectionMatrix._22, viewProjectionMatrix._23, viewProjectionMatrix._24),
		//	Vector4(viewProjectionMatrix._31, viewProjectionMatrix._32, viewProjectionMatrix._33, viewProjectionMatrix._34),
		//	Vector4(viewProjectionMatrix._41, viewProjectionMatrix._42, viewProjectionMatrix._43, viewProjectionMatrix._44)
		//);
*/