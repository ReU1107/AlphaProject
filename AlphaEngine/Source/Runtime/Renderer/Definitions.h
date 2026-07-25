#pragma once
#include "Core/Common.h"
#include "Core/Allocator/Allocator.h"
#include "Types.h"

namespace Alpha
{
	class Texture;
}

namespace Alpha::Renderer
{
	/*
	* GPU側に贈る時のデータ構造
	* element_offset:要素単位
	* byte_offset:バイト単位
	*/

	struct MaterialProperties
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
	};

	struct MaterialData
	{
		MaterialProperties props{};
		UInt32 base_color_tex_idx{ 0xFFFFFFFF };
		UInt32 normal_tex_idx{ 0xFFFFFFFF };
		UInt32 metallic_roughness_tex_idx{ 0xFFFFFFFF };
		UInt32 emissive_tex_idx{ 0xFFFFFFFF };

		UInt32 occlusion_tex_idx{ 0xFFFFFFFF };
		UInt32 toon_tex_idx{ 0xFFFFFFFF };
		UInt32 sphere_tex_idx{ 0xFFFFFFFF };
		UInt32 flags{ 0 }; // マテリアルの種類（不透明・半透明など）を判別するフラグ用
	};

	struct Meshlet
	{
		UInt32 vertex_element_offset{};
		UInt32 triangle_element_offset{};
		UInt32 vertex_count{};
		UInt32 triangle_count{};

		UInt32 material_index{};
	};

	struct MeshData
	{
		UInt32 vertex_position_element_offset{};
		UInt32 vertex_attribute_byte_offset{};

		UInt32 meshlet_element_offset{};
		UInt32 meshlet_count{};

		UInt32 vertex_element_offset{};
		UInt32 triangle_element_offset{};

		UInt32 attribute_flags{};	// このモデルが持つ頂点属性のフラグ
		UInt32 vertex_stride{};	// 頂点属性のバイト数
	};

	/*
	* ResourceManagerで管理するやつ
	* Allocationは基本Free用の為に持っている
	*/

	struct Material
	{
		Allocation alloc{};
		MaterialData data{};
	};

	struct MaterialTable
	{
		Allocation alloc{};
		UInt32 material_indices[64]{};
		UInt32 material_num{ 0 };
	};

	struct TextureResource
	{
		TUnique<Texture> tex{ nullptr };
	};

	struct SubMesh
	{
		UInt32 offset{ 0 };
		UInt32 count{ 0 };
	};

	struct AccelerationStructure
	{
		Allocation source_alloc{};
		UInt64 size{ 0 };
		UInt64 scratch_size{ 0 };
	};

	struct Mesh
	{
		Allocation vertex_position_alloc{};
		Allocation vertex_attribute_alloc{};

		Allocation index_alloc{};

		Allocation vertex_inidices_alloc{};
		Allocation triangle_indices_alloc{};
		Allocation meshlet_alloc{};

		Allocation data_alloc{};

		AccelerationStructure as{};

		TArray<SubMesh> submeshes{};

		MeshData data{};
		
		UInt32 vertex_count{};
		UInt32 default_table_index{};
	};
}