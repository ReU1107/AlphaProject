#include "ParticleRender.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "ParticleSystem.h"

using namespace Alpha::Renderer;

namespace Alpha::Particle
{

	struct PrerenderPassConstants
	{
		UInt32 dispatch_arguments;

	};

	void AddParticleRenderPass(RenderCore* render_core, PipelineLayout* layout, ParticleSystem* system, RenderGraph* graph)
	{
		auto command_layout = render_core->CreateCommandLayout(
			{
				.arguments = {
					{
						.type = ERHIIndirectArgType::DispatchMesh,
					},
				},
			}
		);

		auto prerender_cs = render_core->CreateShader(L"Asset/ParticlePrerenderCS.hlsl", ERHIShaderType::Compute);

		auto particle_command = graph->CreateBuffer(L"ParticleRenderCommand",
			{
				sizeof(DispatchMeshArgs), sizeof(DispatchMeshArgs), EBufferPresetType::RWStructured
			}
		);

		auto* pipeline = RenderCore::Get().CreateComputePipeline(
			{
				.pipeline_layout = layout,
				.compute_shader = prerender_cs
			}
		);

		graph->AddPass(L"ParticlePrerender",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteBuffer(particle_command, ERHIResourceState::UnorderedAccess);
				builder.UsePipeline(pipeline, ERHIPipelineType::Compute);
			},
			[=](RenderGraphContext& ctx)
			{
				auto* command_buffer = ctx.graph->GetBuffer(particle_command);
				PrerenderPassConstants constants = {};
				constants.dispatch_arguments = command_buffer->GetUAVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 4);

				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), system->GetBufferIndicesOffset(), 2);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), system->GetConstantsOffset(), 3);


				ctx.command_list->Dispatch(1, 1, 1);
			}
		);

		auto sprite_ms = render_core->CreateShader(L"Asset/ParticleSpriteMS.hlsl", ERHIShaderType::Mesh);

		auto sprite_ps = render_core->CreateShader(L"Asset/ParticleSpritePS.hlsl", ERHIShaderType::Pixel);

		auto sprite_pipeline = render_core->CreateGraphicsPipeline(
			{
				.pipeline_layout = layout,
				.task_shader = nullptr,
				.mesh_shader = sprite_ms,
				.pixel_shader = sprite_ps,

				.render_target_count = 1,
				.render_targets = {
					ERHIResourceFormat::RGBAFloat,
				},
				.blend_states = {
					DefaultAlphaBlend(),
				},

				.cull_mode = ERHICullMode::None,
				.depth_format = ERHIResourceFormat::Depth32,
				.depth_enable = true,
				.depth_write_enable = false,
				.depth_compare_op = ERHICompareOp::GreaterEqual,
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

		graph->AddPass(L"ParticleSpriteRender",
			[=](RenderGraphBuilder& builder)
			{
				builder.ReadBuffer(particle_command, ERHIResourceState::IndirectArgument);
				builder.WriteTexture(scene_color, ERHIResourceState::RenderTarget);
				builder.WriteTexture(depth, ERHIResourceState::DepthWrite);
				builder.UsePipeline(sprite_pipeline, ERHIPipelineType::Graphics);
			},
			[=](RenderGraphContext& ctx)
			{
				auto* command_buffer = ctx.graph->GetBuffer(particle_command);
				auto* color_texture = ctx.graph->GetTexture(scene_color);
				auto* depth_texture = ctx.graph->GetTexture(depth);

				auto* transient = render_core->GetTransientAllocator();

				ctx.command_list->SetGraphicsCBV(transient->GetBuffer(), system->GetBufferIndicesOffset(), 2);

				ctx.command_list->SetRenderTarget(color_texture, depth_texture);
				ctx.command_list->SetViewport(Viewport(0, 0, 1920.0f, 1080.0f, 0.0f, 1.0f));
				ctx.command_list->SetScissorRect(0, 0, 1920, 1080);

				ctx.command_list->ExecuteIndirect(command_layout, command_buffer, 1);
			}
		);

	}

}