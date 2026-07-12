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
	* ↓GPU側に贈る時のデータ構造
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
		float  padding{ 0.0f };						// 16バイト境界合わせ

		bool operator==(const MaterialProperties&) const = default;
	};

	struct Meshlet
	{
		UInt32 vertex_offset{};
		UInt32 triangle_offset{};
		UInt32 vertex_count{};
		UInt32 triangle_count{};


		UInt32 material_index{};
		UInt32 mesh_index{};		// 
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

	struct MeshData
	{
		UInt32 vertex_position_offset{};
		UInt32 vertex_attribute_offset{};

		UInt32 attribute_flags{};	// このモデルが持つ頂点属性のフラグ
		UInt32 vertex_stride{};	// 頂点属性のバイト数
	};

	struct InstanceData
	{
		Matrix4x4 world_matrix{};
		// 属性フラグ
		// 行列パレットの場所を指すindex
		UInt32 material_table_index{};
	};

	struct MaterialTable
	{
		UInt32 material_indices[16];
	};

	/*
	* ↓CPU側で持つべきデータ構造
	*/

	struct Mesh
	{
		Allocation vertex_position_alloc{};
		Allocation vertex_attribute_alloc{};
		Allocation vertex_inidices_alloc{};
		Allocation triangle_indices_alloc{};
		Allocation meshlet_alloc{};
		Allocation data_alloc{};

		TArray<Meshlet> meshlets{};     
		TArray<UInt32> vertex_indices{}; 
		TArray<UInt32> triangle_indices{};

		MeshData data{};
	};

	struct Material
	{
		Allocation alloc{};
		MaterialData data{};
		UInt32 material_offset{};	// 一時変数がつかえないからおいてる(ちょっとした最適化をするならMaterialPropertiesの無駄なpaddingをハックしたっていいw)
	};

	struct TextureResource
	{
		TUnique<Texture> tex{ nullptr };
	};



}