#pragma once
#include "Core/Common.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
	// ラスタライズ用
	struct InstanceData
	{
		Matrix4x4 world_matrix{ Matrix4x4::Identity };
		// 属性フラグ
		// 行列パレットの場所を指すindex

		UInt32 mesh_offset{};
		UINT32 vertex_element_offset{};
		UInt32 override_material_element_offset{ 0xFFFFFFFF };	// 使わない場合は0xFFFFFFFF
	};

	// レイトレ用
	struct ASInstanceData
	{
		Matrix3x4 transform{ Matrix3x3::Identity };
		UInt32 instance_id : 24{ 0 };
		UInt32 instance_mask : 8{ 0xff };
		UInt32 hit_group_offset : 24{ 0 };
		UInt32 flags : 8{ 0 };
		UInt64 blas_gpu_address{ 0 };
	};


	struct LightData
	{
		float3 position{};

	};

	struct DynamicBufferIndices
	{
		UInt32 instance{};
		UInt32 vertex_to_instance{};

		UInt32 current_position_srv{};      // 
		UInt32 current_position_uav{};      // 

		UInt32 override_material{};

		UInt32 rt_structure{};		// TLASのSRV
	};

	struct SystemBufferIndices
	{
		StaticBufferIndices static_res{};
		DynamicBufferIndices dynamic_res{};

	};

	struct SystemTextureIndices
	{
		UInt32 result{};
		UInt32 visibility{};
		UInt32 normal{};
		UInt32 depth{};
	};

	enum class ESystemRootSlot
	{
		Buffers,
		Textures,
		Constants,
	};

}