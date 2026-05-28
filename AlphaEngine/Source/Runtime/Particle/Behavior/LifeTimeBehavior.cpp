#include "LifeTimeBehavior.h"
#include "Particle/Context.h"

#include "RenderCore/Module.h"

namespace Alpha::Particle
{
	static bool registered = []()
		{
			BehaviorRegistry::GetRegistry().push_back([]()
				{
					return std::make_unique<LifeTimeBehavior>();
				}
			);
			return true;
		}();
	static TUnique<Shader> s_shader = nullptr;
	static TUnique<PipelineLayout> s_pipeline_layout = nullptr;
	static TUnique<ComputePipeline> s_pipeline = nullptr;

	static bool IsModuleInitialized = false;

	LifeTimeBehavior::~LifeTimeBehavior()
	{
	}

	void LifeTimeBehavior::Setup(const Settings& settings)
	{
		// 初期化済み
		if (IsModuleInitialized)
			return;

		auto core = settings.render_core;

		s_shader = core->CreateShader(L"Asset/ParticleLifecycle.hlsl", ERHIShaderFlags::Compute);

		s_pipeline_layout = core->CreatePipelineLayout(
			[](auto& layout)
			{
				layout.BeginDescriptorTable();
				layout.AddDescriptorUAV(2);
				layout.EndDescriptorTable();
			}
		);

		s_pipeline = core->CreateComputePipeline(
			{
				.pipeline_layout = s_pipeline_layout.get(),
				.compute_shader = s_shader.get(),
			}
		);

		IsModuleInitialized = true;
	}

	void LifeTimeBehavior::Cleanup()
	{
	}

	void LifeTimeBehavior::Execute(UpdateContext& ctx)
	{
		CommandList* list = ctx.base.command_list;

		list->SetComputePipeline(s_pipeline.get());

		list->SetComputeDescriptorTable(ctx.base.table, 0);

		uint32_t thread_groups = (ctx.particle_num + 63) / 64;

		list->Dispatch(thread_groups, 1, 1);


		// 生成が終わるまで待機
		list->UAVBarrier(ctx.base.particle_pool);
		list->UAVBarrier(ctx.base.dead_list);

	}
}