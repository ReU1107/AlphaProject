#include "RenderSystem.h"

#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "Definitions.h"
#include "TransformComponent.h"

#include "Core/Math/MathF.h"

#include "RenderScene.h"
#include "VertexComputePass.h"
#include "VBufferPass.h"
#include "FroxelLightingPass.h"
#include "FogPass.h"

#include <imgui/imgui.h>
namespace Alpha
{
	using namespace Renderer;
	RenderSystem::~RenderSystem() = default;

	bool RenderSystem::Initialize(const Args& args)
	{
		m_layout = RenderCore::Get().CreatePipelineLayout(
			[](PipelineLayout& layout)
			{
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
			}
		);

		m_scene = std::make_unique<RenderScene>();
		m_scene->Initialize({ args.render_core, m_layout.get() });

		m_vbuffer_pass = std::make_unique<VBufferPass>();
		m_vbuffer_pass->Initialize({ args.render_core, m_layout.get() });

		m_fog_pass = std::make_unique<FogPass>();
		//m_fog_pass->Initialize({ args.render_core, m_layout.get() });

		m_froxel_pass = std::make_unique<FroxelLightingPass>();
		m_froxel_pass->Initialize({ args.render_core, m_layout.get() });
		
		return true;
	}

	void RenderSystem::Finalize()
	{

	}

	void RenderSystem::Render(entt::registry& registry, RenderContext& ctx)
	{

		ctx.command_list->SetPipelineLayout(m_layout.get());

		m_scene->Build(registry, ctx);

		auto camera_view = registry.view<CameraComponent, TransformComponent>();

		entt::entity camera_entity = camera_view.front();
		auto& camera = camera_view.get<CameraComponent>(camera_entity);
		auto& camera_t = camera_view.get<TransformComponent>(camera_entity);


		Constants constants = {};
		constants.view_proj = MatrixMultiply(camera.view_matrix, camera.GetProjectionMatrix());
		constants.proj_view = Matrix4x4Inverse(constants.view_proj);
		constants.camera_pos = camera_t.GetLocalToWorld()._4();
		constants.instance_count = m_scene->GetInstanceCount();
		constants.light_count = m_scene->GetLightCount();

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<Constants>(&constants);

			ctx.command_list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);

		}
		ctx.instance_count = constants.instance_count;
		m_vbuffer_pass->Execute(ctx);

		m_froxel_pass->Execute(ctx);


		//m_fog_pass->Execute(ctx);

	}
}