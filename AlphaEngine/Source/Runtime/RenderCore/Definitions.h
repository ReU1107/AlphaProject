#pragma once
#include "Core/Common.h"
#include "Core/Allocator/Allocator.h"
#include "RHI/Types.h"
#include "RHI/Definitions.h"
#include "VertexLayout.h"


namespace Alpha
{
	class RenderHardwareInterface;
	class Shader;
	class PipelineLayout;
	class Buffer;


	struct GraphicsPipelineArgs
	{
		RenderHardwareInterface* rhi{ nullptr };
		VertexLayout vertex_layout;
		PipelineLayout* pipeline_layout;

		Shader* vertex_shader{ nullptr };
		Shader* geometry_shader{ nullptr };
		Shader* hull_shader{ nullptr };
		Shader* domain_shader{ nullptr };

		Shader* task_shader{ nullptr };
		Shader* mesh_shader{ nullptr };

		Shader* pixel_shader{ nullptr };

		UInt32 render_target_count{ 0 };
		TFixedArray<ERHIResourceFormat, 8> render_targets{};
		TFixedArray<RHIBlendStateDesc, 8> blend_states{};

		ERHICullMode cull_mode{ ERHICullMode::Back };
		ERHIPrimitiveTopology primitive_topology{ ERHIPrimitiveTopology::TriangleList };
		ERHIResourceFormat depth_format{ ERHIResourceFormat::Depth24Stencil8 };
		bool depth_enable{ false };
	};

	static RHIBlendStateDesc DefaultAlphaBlend()
	{
		RHIBlendStateDesc desc = {};
		desc.blend_enable = true;
		desc.logic_op_enable = false;

		desc.src_blend = ERHIBlend::SrcAlpha;
		desc.dest_blend = ERHIBlend::InvSrcAlpha;
		desc.blend_op = ERHIBlendOp::Add;

		desc.src_blend_alpha = ERHIBlend::One;
		desc.dest_blend_alpha = ERHIBlend::InvSrcAlpha;
		desc.blend_op_alpha = ERHIBlendOp::Add;

		return desc;
	}

	struct ComputePipelineArgs
	{
		RenderHardwareInterface* rhi{ nullptr };
		PipelineLayout* pipeline_layout{ nullptr };
		Shader* compute_shader{ nullptr };

	};

	struct HitGroupArgs
	{
		String name{};
		ERHIHitGroupType type{ ERHIHitGroupType::Triangles };

		Shader* any_hit_shader{ nullptr };
		Shader* closest_hit_shader{ nullptr };
		Shader* intersection_shader{ nullptr };
	};

	struct RayTracePipelineArgs
	{
		RenderHardwareInterface* rhi{ nullptr };
		PipelineLayout* pipeline_layout{ nullptr };

		TArray<Shader*> libraries{}; // 複数エントリポイントを含む共有ライブラリがあれば追加

		Shader* ray_gen_shader{ nullptr };
		TArray<Shader*> miss_shaders{};
		TArray<HitGroupArgs> hit_groups{};

		UInt32 max_payload_size{ 0 };
		UInt32 max_attribute_size{ 8 };
		UInt32 max_recursion_depth{ 1 };
	};

	struct ASBuildSize
	{
		UInt64 result{ 0 };
		UInt64 scratch{ 0 };
		UInt64 update{ 0 };
	};

	struct BLASRequest
	{
		Buffer* vertex_buffer{ nullptr };
		UInt64 vertex_offset{ 0 };

		Buffer* index_buffer{ nullptr };
		UInt64 index_offset{ 0 };

		Buffer* result_buffer{ nullptr };
		UInt64 result_offset{ 0 };

		Buffer* source_as_buffer{ nullptr };
		UInt64 source_as_offset{ 0 };

		UInt64 scratch_size{ 0 };

		struct Sub
		{
			UInt64 index_byte_offset{};
			UInt32 index_count{};
			bool is_opaque{ false };
		};

		TArray<Sub> geometries{};

		UInt32 vertex_count{ 0 };
	};

	struct TLASRequest
	{
		Buffer* instance_desc_buffer{ nullptr };

		Buffer* result_buffer{ nullptr };

		UInt64 scratch_size{ 0 };
		UInt32 instance_count{ 0 };
	};

	struct ShaderTable
	{
		Allocation alloc = {};
		TArray<String> export_names{};
		TArray<UInt8> stream{};
	};


}