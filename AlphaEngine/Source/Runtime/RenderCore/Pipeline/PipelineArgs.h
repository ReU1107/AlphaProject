#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"
#include "RHI/Definitions.h"
#include "VertexLayout.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class Shader;
	class RenderTexture;
	class PipelineLayout;


	struct GraphicsPipelineArgs
	{
		RenderHardwareInterface* rhi{ nullptr };
		VertexLayout vertex_layout;
		PipelineLayout* pipeline_layout;

		Shader* vertex_shader{ nullptr };
		Shader* pixel_shader{ nullptr };
		Shader* geometry_shader{ nullptr };
		Shader* hull_shader{ nullptr };
		Shader* domain_shader{ nullptr };

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

}