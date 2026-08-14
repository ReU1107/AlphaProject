#pragma once
#include "Core/Common.h"

namespace Alpha
{
	struct VolumeComponent
	{
		float3 color{ 0.85f, 0.90f, 0.95f };
	private:
		float m_density{ 0.12f };
		float3 m_volume_size{ 60.0f, 45.0f, 50.0f };
		float m_base_cell_size{ 0.5f };


		bool m_is_dirty{ true };
	public:
		void SetDensity(float density);
		float GetDensity() const { return m_density; }

		void SetSize(const float3& size);
		const float3& GetSize() const { return m_volume_size; }

		void SetBaseCellSize(float size);

		bool IsDirty() const { return m_is_dirty; }

		uint3 CalculateCellGrid(float* out_cell_size) const;
	};

}