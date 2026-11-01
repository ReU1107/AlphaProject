#include "OpaqueShadingPass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"

namespace Alpha::Renderer
{
	struct OpaqueShadingPassConstants
	{
		uint result_index;
		uint visibility;
		uint depth;
		uint cluster;
		uint light_cluster;
		uint light_indices;
	};

	void AddOpaqueShadingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph, const ClusterLightResources& cluster_resources, const VBufferResources& vbuffer_resources)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/Shader/OpaqueShadingCS.hlsl", ERHIShaderType::Compute);

		ComputePipeline* pipeline = RenderCore::Get().CreateComputePipeline({
			.pipeline_layout = layout,
			.compute_shader = shader
			}
		);

		auto scene_color = graph->CreateTexture(L"SceneColor",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::RenderTarget | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		auto depth = vbuffer_resources.depth_texture;

		auto visibility = vbuffer_resources.visibility_texture;

		auto cluster = cluster_resources.cluster_texture;

		auto light_cluster = cluster_resources.cluster_buffer;

		auto light_indices = cluster_resources.light_indices;

		graph->AddPass(L"OpaqueShading",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(scene_color, ERHIResourceState::UnorderedAccess);
				builder.ReadTexture(depth, ERHIResourceState::AllShaderResource);
				builder.ReadTexture(visibility, ERHIResourceState::AllShaderResource);

				builder.ReadTexture(cluster, ERHIResourceState::AllShaderResource);
				builder.ReadBuffer(light_cluster, ERHIResourceState::AllShaderResource);
				builder.ReadBuffer(light_indices, ERHIResourceState::AllShaderResource);

				builder.UsePipeline(pipeline, ERHIPipelineType::Compute);
			},
			[=](RenderGraphContext& ctx)
			{
				auto* result_tex = ctx.graph->GetTexture(scene_color);
				auto* depth_tex = ctx.graph->GetTexture(depth);
				auto* visibility_tex = ctx.graph->GetTexture(visibility);

				auto* cluster_tex = ctx.graph->GetTexture(cluster);
				auto* light_cluster_buffer = ctx.graph->GetBuffer(light_cluster);
				auto* light_indices_buffer = ctx.graph->GetBuffer(light_indices);

				OpaqueShadingPassConstants constants = {};
				constants.result_index = result_tex->GetUAVHandle().index;
				constants.visibility = visibility_tex->GetSRVHandle().index;
				constants.depth = depth_tex->GetSRVHandle().index;

				constants.cluster = cluster_tex->GetSRVHandle().index;
				constants.light_cluster = light_cluster_buffer->GetSRVHandle().index;
				constants.light_indices = light_indices_buffer->GetSRVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

				UInt32 group_x = RoundUp(1920, 8);
				UInt32 group_y = RoundUp(1080, 8);
				ctx.command_list->Dispatch(group_x, group_y, 1);
			}
		);
	}

	/*
	* void AddOpaqueShadingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/Shader/OpaqueShadingCS.hlsl", ERHIShaderType::Compute);

		ComputePipeline* pipeline = RenderCore::Get().CreateComputePipeline({
			.pipeline_layout = layout,
			.compute_shader = shader
			}
		);

		auto scene_color = graph->CreateTexture(L"SceneColor",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::RenderTarget | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		auto depth = graph->CreateTexture(L"Depth",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::DepthStencil,
				ERHIResourceFormat::Depth32
			}
		);

		auto visibility = graph->CreateTexture(L"Visibility",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::RenderTarget,
				ERHIResourceFormat::RGBAFloat
			}
		);

		graph->AddPass(L"OpaqueShading",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(scene_color, ERHIResourceState::UnorderedAccess);
				builder.ReadTexture(depth, ERHIResourceState::AllShaderResource);
				builder.ReadTexture(visibility, ERHIResourceState::AllShaderResource);

				builder.UsePipeline(pipeline, ERHIPipelineType::Compute);
			},
			[=](RenderGraphContext& ctx)
			{
				Texture* result_tex = ctx.graph->GetTexture(scene_color);
				Texture* depth_tex = ctx.graph->GetTexture(depth);
				Texture* visibility_tex = ctx.graph->GetTexture(visibility);

				OpaqueShadingPassConstants constants = {};
				constants.result_index = result_tex->GetUAVHandle().index;
				constants.visibility = visibility_tex->GetSRVHandle().index;
				constants.depth = depth_tex->GetSRVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

				UInt32 group_x = RoundUp(1920, 8);
				UInt32 group_y = RoundUp(1080, 8);
				ctx.command_list->Dispatch(group_x, group_y, 1);
			}
		);
	}
	*/
}