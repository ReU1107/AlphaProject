#pragma once
#include "Core/Common.h"
#include "Core/Allocator/Allocator.h"
#include "RHI/Types.h"
#include "Types.h"

#include "RenderCore/GraphicsPipeline.h"
#include "RenderCore/ComputePipeline.h"
#include "RenderCore/RayTracePipeline.h"
#include "RenderCore/Types.h"

#include <variant>

namespace Alpha
{
	class Texture;
	class CommandList;
	class GraphicsPipeline;
	class ComputePipeline;
	class RayTracePipeline;

	class TransientBufferAllocator;

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

	struct TextureBindingData
	{
		UInt32 texture_index{ 0xFFFFFFFF };
		UInt32 sampler_index;
	};

	struct MaterialData
	{
		MaterialProperties props{};

		TextureBindingData base_color{};
		TextureBindingData normal{};
		TextureBindingData metallic_roughness{};
		TextureBindingData emissive{};
		TextureBindingData occlusion{};
	};

	struct Meshlet
	{
		UInt32 vertex_element_offset{};
		UInt32 triangle_element_offset{};
		UInt32 vertex_count{};
		UInt32 triangle_count{};

		UInt32 material_index{};
	};

	struct Submesh
	{
		UInt32 index_element_offset{ 0 };
		UInt32 index_count{ 0 };
	};

	struct MeshData
	{
		UInt32 vertex_position_element_offset{};
		UInt32 vertex_attribute_byte_offset{};

		UInt32 index_element_offset{};
		UInt32 submesh_element_offset{};

		UInt32 meshlet_element_offset{};
		UInt32 meshlet_count{};

		UInt32 vertex_element_offset{};
		UInt32 triangle_element_offset{};

		UInt32 material_table_element_offset{};

		UInt32 attribute_flags{};	// このモデルが持つ頂点属性のフラグ
		UInt32 vertex_stride{};	// 頂点属性のバイト数
		UInt32 index_count{};
	};

	// ラスタライズ用
	struct InstanceData
	{
		Matrix4x4 world_matrix{ Matrix4x4::Identity };
		// 属性フラグ
		// 行列パレットの場所を指すindex

		UInt32 mesh_offset{};
		UINT32 vertex_element_offset{};
		UInt32 override_material_element_offset{ 0xFFFFFFFF };	// 使わない場合は0xFFFFFFFF
		UInt32 matrix_palette_offset{ 0xFFFFFFFF };
		UInt32 visible{ 1 };
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



	/*
	* ResourceManagerで管理するやつ
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
		TextureResource() = default;
		~TextureResource();

		TextureResource(TextureResource&&) noexcept;
		TextureResource& operator=(TextureResource&&) noexcept;

		TextureResource(const TextureResource&) = delete;
		TextureResource& operator=(const TextureResource&) = delete;

		TUnique<Texture> tex{ nullptr };
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
		Allocation submesh_alloc{};

		Allocation vertex_inidices_alloc{};
		Allocation triangle_indices_alloc{};
		Allocation meshlet_alloc{};

		Allocation data_alloc{};

		AccelerationStructure as{};

		TArray<Submesh> submeshes{};

		MeshData data{};
		
		UInt32 vertex_count{};
		UInt32 default_table_index{};
	};

	/*
	* 
	*/

	struct StaticBufferIndices
	{
		UInt32 position{};
		UInt32 attribute{};
		UInt32 index{};
		UInt32 vertex_indices{};
		UInt32 triangle_indices{};
		UInt32 meshlet{};
		UInt32 mesh{};
		UInt32 submesh{};
		UInt32 material{};
		UInt32 material_table{};
	};

	struct DynamicBufferIndices
	{
		UInt32 instance{};
		UInt32 vertex_to_instance{};

		UInt32 current_position_srv{};      // 
		UInt32 current_position_uav{};      // 

		UInt32 matrix_palette{};

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

	struct RGTextureDesc
	{
		UInt32 width{ 1 };
		UInt32 height{ 1 };
		UInt32 depth{ 1 };
		ERHIResourceDimension dimension{ ERHIResourceDimension::Tex2D };
		ERHIResourceBindFlags flags{ ERHIResourceBindFlags::ShaderResource };
		ERHIResourceFormat format{ ERHIResourceFormat::RGBAUnorm };

		bool operator==(const RGTextureDesc& other) const
		{
			return width == other.width &&
				height == other.height &&
				depth == other.depth &&
				dimension == other.dimension &&
				format == other.format &&
				flags == other.flags;
		}
	};

	struct RGBufferDesc
	{
		UInt32 stride{ sizeof(UInt32) };
		UInt64 size{ sizeof(UInt32) };
		EBufferPresetType type{ EBufferPresetType::RWStructured };

		bool operator==(const RGBufferDesc& other) const
		{
			return stride == other.stride &&
				size == other.size &&
				type == other.type;
		}
	};

	struct RGTextureResource
	{
		String name{};
		RGTextureDesc desc{};
		Texture* target{ nullptr };
		bool imported{ false };
		ERHIResourceState current_state{ ERHIResourceState::Common };
	};

	struct RGBufferResource
	{
		String name{};
		RGBufferDesc desc{};
		Buffer* target{ nullptr };
		bool imported{ false };
		ERHIResourceState current_state{ ERHIResourceState::Common };
	};

	struct RGTextureHandle
	{
		static constexpr UInt32 InvalidIndex = UINT32_MAX;

		UInt32 index{ InvalidIndex };

		bool IsValid() const
		{
			return index != InvalidIndex;
		}

		explicit operator bool() const
		{
			return IsValid();
		}
	};

	struct RGBufferHandle
	{
		static constexpr UInt32 InvalidIndex = UINT32_MAX;

		UInt32 index{ InvalidIndex };

		bool IsValid() const
		{
			return index != InvalidIndex;
		}

		explicit operator bool() const
		{
			return IsValid();
		}
	};

	class RenderGraph;

	struct RenderGraphContext
	{
		CommandList* command_list{ nullptr };
		Texture* current_back_buffer{ nullptr };

		UInt32 instance_count{ 0 };
		UInt32 current_frame_index{ 0 };
		// 一旦ね
		RenderGraph* graph{ nullptr };
	};

	struct RGTextureUsage
	{
		RGTextureHandle handle{};    // このテクスチャを
		ERHIResourceState state{};    // このステートで使用する
	};

	struct RGBufferUsage
	{
		RGBufferHandle handle{};    // このテクスチャを
		ERHIResourceState state{};    // このステートで使用する
	};

	using RGPipelineHandle = std::variant<
		std::monostate,
		GraphicsPipeline*,
		ComputePipeline*,
		RayTracePipeline*
	>;

	struct RGPipelineUsage
	{
		RGPipelineHandle handle;
		ERHIPipelineType type{};

	};

	struct RenderContext
	{
		CommandList* command_list{ nullptr };
		TransientBufferAllocator* transient_buffer{ nullptr };
		Texture* current_back_buffer{ nullptr };
		UInt32 current_frame_index{ 0 };

		UInt64 current_camera_offset{ 0 };

		UInt32 instance_count{ 0 };
		Matrix4x4 inv_view_proj{ Matrix4x4::Identity };

		UInt32 width{ 0 };
		UInt32 height{ 0 };

		UInt32 debug_value{ 0 };

	};


}