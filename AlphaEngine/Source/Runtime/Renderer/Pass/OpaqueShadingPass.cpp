#include "OpaqueShadingPass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"

namespace Alpha::Renderer
{
	struct OpaqueShadingPassConstants
	{
		UInt32 result_index;
		UInt32 visibility;
		UInt32 depth;
	};

	void AddOpaqueShadingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/OpaqueShadingCS.hlsl", ERHIShaderType::Compute);

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
				ERHIResourceFormat::RGUInt
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

	/*
	* void AddOpaqueShadingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/OpaqueShadingCS.hlsl", ERHIShaderType::Compute);

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