#include "RenderSystem.h"

#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "Definitions.h"
#include "TransformComponent.h"

#include "Core/Math/MathF.h"

#include "RenderScene.h"
#include "VertexComputePass.h"
#include "VBufferPass.h"

#include <imgui/imgui.h>
namespace Alpha
{
	using namespace Renderer;
	RenderSystem::~RenderSystem() = default;

	static TUnique<Shader> rg;
	static TUnique<Shader> ch;
	static TUnique<Shader> mi;
	static TUnique<Texture> result;

	bool RenderSystem::Initialize(const Args& args)
	{
		m_layout = RenderCore::Get().CreatePipelineLayout(
			[](PipelineLayout& layout)
			{
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
			}
		);

		m_scene = std::make_unique<RenderScene>();
		m_scene->Initialize({ args.render_core, m_layout.get() });

		m_vbuffer_pass = std::make_unique<VBufferPass>();
		m_vbuffer_pass->Initialize({ args.render_core, m_layout.get() });

		rg = RenderCore::Get().CreateShader(L"Asset/RayGenerate.hlsl", ERHIShaderType::Library, L"MyRayGen");
		mi = RenderCore::Get().CreateShader(L"Asset/Miss.hlsl", ERHIShaderType::Library, L"MyMiss");
		ch = RenderCore::Get().CreateShader(L"Asset/Hit.hlsl", ERHIShaderType::Library, L"MyClosestHit");

		TArray<HitGroupArgs> hit_groups{};

		HitGroupArgs hit_group = {
			.name = L"Test",
			.closest_hit_shader = ch.get(),
		};

		hit_groups.push_back(hit_group);

		TArray<Shader*> miss_shaders{};
		miss_shaders.push_back(mi.get());
		
		m_raytrace_pipeline = RenderCore::Get().CreateRayTracePipeline(
			{
				.pipeline_layout = m_layout.get(),
				.ray_gen_shader = rg.get(),
				.miss_shaders = miss_shaders,
				.hit_groups = hit_groups,
				.max_payload_size = 32,
			}
		);

		result = RenderCore::Get().CreateTexture(1920, 1080, ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess);

		return true;

	}

	void RenderSystem::Finalize()
	{

	}

	struct Constants
	{
		float4x4 view_proj{ float4x4::Identity };
		float4x4 proj_view{ float4x4::Identity };
		float4 camera_pos;
		float4 frustum_planes;

		UInt32 instance_count;
	};

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

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<Constants>(&constants);

			ctx.command_list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);

		}
		ctx.instance_count = constants.instance_count;
		m_vbuffer_pass->Execute(ctx);

		//ctx.command_list->SetRayTracePipeline(m_raytrace_pipeline.get());



		//{
		//	RHIRayTraceDesc desc = {};
		//	desc.shader_table_buffer = m_raytrace_pipeline->GetShaderTableBuffer()->GetRHI();
		//	desc.ray_gen_offset = m_raytrace_pipeline->GetRayGenerateTable().alloc.offset;
		//	desc.miss_offset = m_raytrace_pipeline->GetMissTable().alloc.offset;
		//	desc.hit_group_offset = m_raytrace_pipeline->GetHitGroupTable().alloc.offset;
		//	desc.width = 1920;
		//	desc.height = 1080;
		//	desc.depth = 1;


		//	ctx.command_list->DispatchRayTrace(desc);
		//}

		//ctx.command_list->UAVBarrier(result.get());
	}
}