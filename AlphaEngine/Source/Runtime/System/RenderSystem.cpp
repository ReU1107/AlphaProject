#include "RenderSystem.h"

#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "Definitions.h"
#include "Spatial/TransformComponent.h"

#include "Core/Math/MathF.h"

#include "RenderScene.h"
#include "VertexComputePass.h"
#include "VBufferPass.h"
#include "FroxelLightingPass.h"
#include "VolumetricPass.h"
#include "CompositePass.h"

#include <imgui/imgui.h>
namespace Alpha
{
	using namespace Renderer;
	RenderSystem::~RenderSystem() = default;

	static TUnique<Texture> test = nullptr;

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

		m_froxel_pass = std::make_unique<FroxelLightingPass>();
		m_froxel_pass->Initialize({ args.render_core, m_layout.get() });

		m_volumetric_pass = std::make_unique<VolumetricPass>();
		m_volumetric_pass->Initialize({ args.render_core, m_layout.get(), m_froxel_pass->GetResult(), m_vbuffer_pass->GetDepthTex() });


		
		test = RenderCore::Get().CreateTexture(
			1920, 1080,
			ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
			ERHIResourceFormat::RGBAFloat
		);

		m_composite_pass = std::make_unique<CompositePass>();
		m_composite_pass->Initialize({ args.render_core, m_layout.get(), test.get(), m_volumetric_pass->GetRadianceTex(), m_volumetric_pass->GetTransmittanceTex(),m_vbuffer_pass->GetVisibilityTex() });



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
		constants.view = camera.view_matrix;
		constants.view_proj = MatrixMultiply(camera.view_matrix, camera.GetProjectionMatrix());
		constants.inv_view_proj = Matrix4x4Inverse(constants.view_proj);
		constants.camera_pos = camera_t.GetLocalMatrix()._4();
		constants.instance_count = m_scene->GetInstanceCount();
		constants.light_count = m_scene->GetLightCount();

		ctx.inv_view_proj = constants.inv_view_proj;


		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<Constants>(&constants);

			ctx.command_list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);

			ctx.current_camera_offset = offset;

		}
		ctx.instance_count = constants.instance_count;

		m_vbuffer_pass->Execute(ctx);

		m_froxel_pass->Execute(ctx);

		m_volumetric_pass->Execute(ctx);

		m_composite_pass->Execute(ctx);

		ctx.test = test.get();



		ImGui::Begin("Composite Pass");

		ImGui::Image(test.get(), ImVec2(960, 540));

		ImGui::End();

	}
}