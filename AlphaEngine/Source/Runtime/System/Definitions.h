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

	struct VolumeMaterial
	{
		// 今後テクスチャ対応予定
		//Texture density_texture;

		//Texture noise_texture;

		float3 min{};
		float3 max{};

		float density{ 1.0f };

		float anisotropy{ 1.0f };

		float absorption{ 1.0f };

		float scattering{ 1.0f };
	};

	struct LightData
	{
		float3 position{};
		float radius{ 1.0f };
		float3 color{};
		float intensity{ 1.0f };

		float3 direction{};
		UInt32 type{ 0 };				// ライト種別 (0:Point, 1:Spot, 2:Directional)

		float inner_cone{ 1.0f };	// スポットライト内角のcos
		float outer_cone{ 0.7f };	// スポットライト外角のcos

	};

	struct DynamicBufferIndices
	{
		UInt32 instance{};
		UInt32 vertex_to_instance{};

		UInt32 current_position_srv{};      // 
		UInt32 current_position_uav{};      // 

		UInt32 override_material{};

		UInt32 light{};
		UInt32 volume{};

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

	struct Constants
	{
		float4x4 view{ float4x4::Identity };
		float4x4 view_proj{ float4x4::Identity };
		float4x4 proj_view{ float4x4::Identity };
		float4 camera_pos;
		float4 frustum_planes;

		UInt32 instance_count;
		UInt32 light_count;
	};

}