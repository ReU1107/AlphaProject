#include "VolumeComponent.h"
#include "Core/Math/MathUtil.h"
#include "Core/Math/MathF.h"

namespace Alpha
{
	void VolumeComponent::SetDensity(float density)
	{
		m_density = density;
		m_is_dirty = true;
	}

	void VolumeComponent::SetSize(const float3& size)
	{
		m_volume_size = size;
		m_is_dirty = true;
	}

	void VolumeComponent::SetBaseCellSize(float size)
	{
		m_base_cell_size = size;
		m_is_dirty = true;
	}

	uint3 VolumeComponent::CalculateCellGrid(float* out_cell_size) const
	{
		float cell_size = m_base_cell_size / Max(m_density, 0.1f);

		uint3 cell_count = uint3::Zero;
		cell_count.X() = static_cast<UInt32>(Ceil(m_volume_size.X() / cell_size));
		cell_count.Y() = static_cast<UInt32>(Ceil(m_volume_size.Y() / cell_size));
		cell_count.Z() = static_cast<UInt32>(Ceil(m_volume_size.Z() / cell_size));
		

		*out_cell_size = cell_size;

		return VectorClamp(cell_count, 1, 128);
	}

}