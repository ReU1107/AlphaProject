#pragma once
#include "Core/Types.h"
#include "Types.h"

namespace Alpha::Renderer
{
	struct StaticVertex
	{
		float3 position{ float3::Zero };
		float3 normal{ float3::YAxis };
		float2 texcoord{ float2::Zero };
		color4 color{ color4::White };
	};


	struct SkeletalVertex
	{
		float3 position{ float3::Zero };
		float3 normal{ float3::YAxis };
		float2 texcoord{ float2::Zero };
		uint4 bone_index{ uint4::Zero };
		float4 bone_weight{ float4::Zero };

	};

	// PBRに合わせる
	struct MaterialData
	{
		float3 base_color{ 1.0f, 1.0f, 1.0f };		// 
		float  opacity{ 1.0f };						// 不透明度

		float  metallic{ 0.0f };					// 金属感 (0:絶縁体, 1:金属)
		float  roughness{ 0.5f };					// 粗さ (0:ツルツル, 1:ザラザラ)
		float  emissive_intensity{ 0.0f };			// 自発光の強さ
		float  occlusion{ 1.0f };					// アンビエントオクルージョン

		float3 emissive_color{ 0.0f, 0.0f, 0.0f };	// 自発光の色
		float  refraction_index{ 1.5f };			// 屈折率 (IOR)

		float3 specular_tint{ 1.0f, 1.0f, 1.0f };	// 鏡面反射の微調整用
		float  padding{ 0.0f };						// 16バイト境界合わせ

		bool operator==(const MaterialData&) const = default;
	};


}