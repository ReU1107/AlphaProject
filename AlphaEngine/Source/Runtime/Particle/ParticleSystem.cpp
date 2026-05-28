#include "ParticleSystem.h"

#include "Behavior/Behavior.h"
#include "Emission/SphereSurfaceEmission.h"


#include "Particle.h"

#include "Emitter.h"


#include "RenderCore/Module.h"

namespace Alpha
{
	using namespace Particle;

	ParticleSystem::ParticleSystem()
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	bool ParticleSystem::Initialize(const Args& args)
	{
		//m_particle_buffer = args.render_core->CreateStructuredBuffer(sizeof(Particle), args.max_particle_count);

		RenderCore* render_core = args.render_core;

		m_command_list = render_core->CreateCommandList(ERHICommandType::Compute);
		m_command_queue = render_core->CreateCommandQueue(ERHICommandType::Compute);
		m_timeline = render_core->CreateTimeline();
		
		m_dynamic_CB = render_core->CreateRingBuffer(1_MB);

		m_shape_pipeline_layout = render_core->CreatePipelineLayout(
			[](auto& layout)
			{
				layout.BeginDescriptorTable();
				layout.AddDescriptorUAV(2);
				layout.EndDescriptorTable();
				layout.AddRootCBV(2);
			}
		);

		EmissionRegistry::SetupAll(
			{
				.render_core = render_core,
				.pipeline_layout = m_shape_pipeline_layout.get(),
			}
		);

		BehaviorRegistry::SetupAll(
			{
				.render_core = render_core
			}
		);

		return true;
	}

	void ParticleSystem::Finalize()
	{
	}

	Particle::Emitter* ParticleSystem::CreateEmitter()
	{
		auto emitter = new Emitter();
		Emitter::Args args = {};
		args.core = RenderCore::GetPtr();
		args.heap = RenderCore::Get().GetBindlessHeap();
		emitter->Initialize(args);
		m_emitters.push_back(emitter);
		return emitter;
	}

	void ParticleSystem::Update(Context& ctx)
	{
		m_timeline->WaitCPU();

		m_command_list->Begin(0);

		m_command_list->SetDescriptorHeap(RenderCore::Get().GetBindlessHeap(),RenderCore::Get().GetSamplerHeap());

		ctx.command_list = m_command_list.get();

		m_dynamic_CB->Reset();

		ctx.ring_buffer = m_dynamic_CB.get();

		for (auto& emitter : m_emitters)
		{
			emitter->Update(ctx);
		}

		m_command_list->End();

		m_command_queue->Execute(m_command_list.get());

		m_command_queue->Signal(m_timeline.get());

	}

}