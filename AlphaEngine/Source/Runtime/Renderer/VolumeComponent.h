#pragma once
#include "Core/Common.h"
#include "Core/Math/AxisAlignedBoundingBox.h"

namespace Alpha
{
	struct VolumeComponent
	{
		float3 color{ 0.85f, 0.90f, 0.95f };
	private:
		float m_density{ 0.12f };
		float3 m_volume_size{ 60.0f, 45.0f, 50.0f };
		float m_base_cell_size{ 0.5f };

	public:
		void SetDensity(float density) { m_density = density; }
		float GetDensity() const { return m_density; }

		void SetSize(const float3& size) { m_volume_size = size; }
		const float3& GetSize() const { return m_volume_size; }

		AxisAlignedBoundingBox GetAABB() const;
	};

}