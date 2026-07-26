#pragma once
#include "Core/Common.h"


namespace Alpha
{

	struct InstanceData
	{
		Matrix4x4 world_matrix{ Matrix4x4::Identity };
		// 属性フラグ
		// 行列パレットの場所を指すindex

		UInt32 material_table_offset{};
		UInt32 mesh_offset{};
		UINT32 vertex_element_offset{};
	};

	struct LightData
	{
		float3 position{};

	};

}