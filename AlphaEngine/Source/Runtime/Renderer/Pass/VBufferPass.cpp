#include "VBufferPass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"


namespace Alpha::Renderer
{
	void AddVBufferPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		auto as = RenderCore::Get().CreateShader(L"Asset/VBufferAS.hlsl", ERHIShaderType::Amplification);
		auto ms = RenderCore::Get().CreateShader(L"Asset/VBufferMS.hlsl", ERHIShaderType::Mesh);
		auto ps = RenderCore::Get().CreateShader(L"Asset/VBufferPS.hlsl", ERHIShaderType::Pixel);

		auto visibility = graph->CreateTexture(L"Visibility",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::RenderTarget,
				ERHIResourceFormat::RGUInt
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

		auto pipeline = RenderCore::Get().CreateGraphicsPipeline(
			{
				.pipeline_layout = layout,
				.task_shader = as,
				.mesh_shader = ms,
				.pixel_shader = ps,
				.render_target_count = 1,
				.render_targets = {
					ERHIResourceFormat::RGUInt,
				},
				.blend_states = {
					DisabledBlend(),
				},
				.cull_mode = ERHICullMode::None,
				.depth_format = ERHIResourceFormat::Depth32,
				.depth_enable = true,
			}
			);


		graph->AddPass(L"VBuffer",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(visibility, ERHIResourceState::RenderTarget);
				builder.WriteTexture(depth, ERHIResourceState::DepthWrite);
				builder.UsePipeline(pipeline, ERHIPipelineType::Graphics);
			},
			[=](RenderGraphContext& ctx)
			{
				Texture* visibility_tex = ctx.graph->GetTexture(visibility);
				Texture* depth_tex = ctx.graph->GetTexture(depth);

				ctx.command_list->SetRenderTarget(visibility_tex, depth_tex);

				ctx.command_list->ClearRenderTarget(visibility_tex, Rect2D(0, 0, 1920, 1080), Color4::Gray);
				ctx.command_list->ClearDepthStencil(depth_tex, Rect2D(0, 0, 1920, 1080), 0.0f);
				ctx.command_list->SetViewport(Viewport(0, 0, (float)1920, (float)1080, 0.0f, 1.0f));

				ctx.command_list->SetScissorRect(0, 0, 1920, 1080);

				ctx.command_list->DispatchMesh(ctx.instance_count, 1, 1);
			}
		);
	}

	/*
	* Vbufferを使わないforwadパス
	* void AddVBufferPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		auto as = RenderCore::Get().CreateShader(L"Asset/TaskShader.hlsl", ERHIShaderType::Amplification);
		auto ms = RenderCore::Get().CreateShader(L"Asset/TestMeshShader.hlsl", ERHIShaderType::Mesh);
		auto ps = RenderCore::Get().CreateShader(L"Asset/TestPixelShader.hlsl", ERHIShaderType::Pixel);

		auto visibility = graph->CreateTexture(L"Visibility",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::RenderTarget,
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

		auto pipeline = RenderCore::Get().CreateGraphicsPipeline(
			{
				.pipeline_layout = layout,
				.task_shader = as,
				.mesh_shader = ms,
				.pixel_shader = ps,
				.render_target_count = 1,
				.render_targets = {
					ERHIResourceFormat::RGBAFloat,
				},
				.blend_states = {
					DisabledBlend(),
				},
				.cull_mode = ERHICullMode::None,
				.depth_format = ERHIResourceFormat::Depth32,
				.depth_enable = true,
			}
			);


		graph->AddPass(L"VBuffer",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(visibility, ERHIResourceState::RenderTarget);
				builder.WriteTexture(depth, ERHIResourceState::DepthWrite);
				builder.UsePipeline(pipeline, ERHIPipelineType::Graphics);
			},
			[=](RenderGraphContext& ctx)
			{
				Texture* visibility_tex = ctx.graph->GetTexture(visibility);
				Texture* depth_tex = ctx.graph->GetTexture(depth);

				ctx.command_list->SetRenderTarget(visibility_tex, depth_tex);

				ctx.command_list->ClearRenderTarget(visibility_tex, Rect2D(0, 0, 1920, 1080), Color4::Gray);
				ctx.command_list->ClearDepthStencil(depth_tex, Rect2D(0, 0, 1920, 1080), 0.0f);
				ctx.command_list->SetViewport(Viewport(0, 0, (float)1920, (float)1080, 0.0f, 1.0f));

				ctx.command_list->SetScissorRect(0, 0, 1920, 1080);

				ctx.command_list->DispatchMesh(ctx.instance_count, 1, 1);
			}
		);
	}
	*/

}