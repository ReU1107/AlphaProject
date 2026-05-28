#include "SphereSurfaceEmission.h"
#include "Particle/Context.h"

#include "RenderCore/Module.h"
#include "Core/Math/Sphere.h"

namespace Alpha::Particle
{
	static bool registered = []()
	{
		EmissionRegistry::GetRegistry().push_back([]()
			{
				return std::make_unique<SphereSurfaceEmission>();
			}
		);
		return true;
	}();

	static TUnique<Shader> s_sphere_shader = nullptr;
	static TUnique<ComputePipeline> s_sphere_pipeline = nullptr;

	static bool IsModuleInitialized = false;

	void SphereSurfaceEmission::Setup(const Settings& settings)
	{
		// 初期化済み
		if (IsModuleInitialized)
			return;

		auto core = settings.render_core;

		s_sphere_shader = core->CreateShader(L"Asset/SphereSurfaceEmit.hlsl", ERHIShaderFlags::Compute);

		s_sphere_pipeline = core->CreateComputePipeline(
			{
				.pipeline_layout = settings.pipeline_layout,
				.compute_shader = s_sphere_shader.get(),
			}
			);

		IsModuleInitialized = true;
	}

	void SphereSurfaceEmission::Cleanup()
	{
		s_sphere_pipeline.reset();
		s_sphere_shader.reset();
		IsModuleInitialized = false;
	}

	void SphereSurfaceEmission::Emit(EmitContext& ctx)
	{
		CommandList* list = ctx.base.command_list;

		list->SetComputePipeline(s_sphere_pipeline.get());

		list->SetComputeDescriptorTable(ctx.base.table, 0);

		{
			EmitInfo info = {};
			info.life_time = ctx.life_time;
			info.color = ctx.color;
			info.max_num = ctx.emit_max_num;
			uint64_t offset = ctx.base.ring_buffer->WriteData(&info, sizeof(EmitInfo));
			list->SetComputeCBV(ctx.base.ring_buffer, offset, 1);
		}

		{
			Sphere sphere = {};
			sphere.radius = m_radius;

			uint64_t offset = ctx.base.ring_buffer->WriteData(&sphere, sizeof(Sphere));
			list->SetComputeCBV(ctx.base.ring_buffer, offset, 2);
		}

		uint32_t thread_groups = (ctx.emit_num + 63) / 64;


		list->Dispatch(thread_groups, 1, 1);


		// 生成が終わるまで待機
		list->UAVBarrier(ctx.base.particle_pool);
		list->UAVBarrier(ctx.base.dead_list);
	}
}
