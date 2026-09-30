#include "RenderSystem.h"

#include "RenderCore/Module.h"
#include "Renderer/Module.h"
#include "Renderer/Pass/VBufferPass.h"
#include "Renderer/Pass/FroxelLightingPass.h"
#include "Renderer/Pass/VolumetricPass.h"
#include "Renderer/Pass/CompositePass.h"
#include "Renderer/Pass/VertexComputePass.h"
#include "Renderer/Pass/OpaqueShadingPass.h"


#include "Definitions.h"
#include "Spatial/TransformComponent.h"

#include "Core/Math/MathF.h"

#include "RenderSceneBuilder.h"

#include "Particle/ParticleSystem.h"
#include "Particle/ParticleRender.h"


#include <imgui/imgui.h>
namespace Alpha
{
	using namespace Renderer;
	using namespace Particle;
	RenderSystem::~RenderSystem() = default;

	static TUnique<Texture> test = nullptr;

	bool RenderSystem::Initialize(const Args& args)
	{
		m_pipeline_layout = RenderCore::Get().CreatePipelineLayout(
			[](PipelineLayout& layout)
			{
				layout.AddRootCBV(10, ERHIShaderFlags::All);
				layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
			}
		);

		ParticleSystem::Args ps_args = {
			.render_core = RenderCore::GetPtr(),
			.layout = m_pipeline_layout,
			.max_particle_count = 100000,
		};
		m_particle_system = std::make_unique<ParticleSystem>();
		m_particle_system->Initialize(ps_args);

		m_scene = std::make_unique<RenderScene>();
		m_scene->Initialize({ args.render_core });

		m_scene_builder = std::make_unique<RenderSceneBuilder>();
		m_scene_builder->Initialize({ args.render_core, m_pipeline_layout });


		m_graph = std::make_unique<RenderGraph>();
		m_graph->Initialize({ args.render_core });

		AddVBufferPass(args.render_core, m_pipeline_layout, m_graph.get());
		AddFroxelLightingPass(args.render_core, m_pipeline_layout, m_graph.get());
		AddVolumetricPass(args.render_core, m_pipeline_layout, m_graph.get());
		AddOpaqueShadingPass(args.render_core, m_pipeline_layout, m_graph.get());
		AddParticleRenderPass(args.render_core, m_pipeline_layout, m_particle_system.get(), m_graph.get());
		AddCompositePass(args.render_core, m_pipeline_layout, m_graph.get());

		GUISystem::Args gui_args = {
			.render_core = RenderCore::GetPtr(),
			.graph = m_graph.get(),
		};
		GUISystem::Startup(gui_args);


		//DebugRenderer::Args debug_args = {
		//	.render_core = RenderCore::GetPtr(),
		//};
		//DebugRenderer::Startup(debug_args);

		return true;
	}

	void RenderSystem::Finalize()
	{

	}

	void RenderSystem::Render(entt::registry& registry, Renderer::RenderContext& ctx)
	{
		m_scene->Reset(ctx.current_frame_index);

		ctx.command_list->SetPipelineLayout(m_pipeline_layout);

		SystemBufferIndices indices =
		{
			.static_res = RenderResourceManager::Get().GetBufferIndices(),
			.dynamic_res = m_scene->GetBufferIndices(),
		};


		auto* transient = ctx.transient_buffer;

		UInt64 offset = transient->WriteData(&indices);

		ctx.command_list->SetComputeCBV(transient->GetBuffer(), offset, 0);

		ctx.command_list->SetGraphicsCBV(transient->GetBuffer(), offset, 0);

		auto camera_view = registry.view<CameraComponent, TransformComponent>();

		entt::entity camera_entity = camera_view.front();
		auto& camera = camera_view.get<CameraComponent>(camera_entity);
		auto& camera_t = camera_view.get<TransformComponent>(camera_entity);

		auto main_light_entity = registry.view<MainLight>().front();
		auto& maint_light = registry.get<MainLight>(main_light_entity);


		FrameRenderContext constants = {};
		constants.view = camera.view_matrix;
		constants.view_proj = MatrixMultiply(camera.view_matrix, camera.GetProjectionMatrix());
		constants.inv_view_proj = Matrix4x4Inverse(constants.view_proj);
		constants.camera_pos = camera_t.GetWorldMatrix()._4();
		constants.camera_right = Matrix4x4Right(camera_t.GetWorldMatrix());
		constants.camera_up = Matrix4x4Up(camera_t.GetWorldMatrix());
		constants.debug_value = ctx.debug_value;
		constants.main_light = maint_light.index;

		ctx.inv_view_proj = constants.inv_view_proj;

		m_scene_builder->Build(registry, ctx, *m_scene.get());


		constants.instance_count = m_scene->GetInstanceCount();
		constants.light_count = m_scene->GetLightCount();


		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);

			ctx.command_list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);

			ctx.current_camera_offset = offset;

		}
		ctx.instance_count = constants.instance_count;

		// ここで
		ParticleContext pctx = {};
		pctx.command_list = ctx.command_list;
		pctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();
		pctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
		pctx.delta_time = 1.0f / 60;

		m_particle_system->Update(registry, pctx);

		

		m_graph->Execute(ctx);


		ctx.command_list->Transition(ctx.current_back_buffer, ERHIResourceState::Present);


	}
}