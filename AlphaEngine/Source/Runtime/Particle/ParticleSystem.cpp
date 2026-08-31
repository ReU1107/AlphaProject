#include "ParticleSystem.h"

#include "Behavior/Behavior.h"
#include "Emission/SphereSurfaceEmission.h"

#include "ParticleEmitterComponent.h"

#include "RenderCore/Module.h"

#include "Spatial/TransformComponent.h"

#include "Core/Math/MathUtil.h"

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
		RenderCore* render_core = args.render_core;
		m_particle_allocator[0] = CreatePersistentAllocator(args.render_core, sizeof(ParticleData), 12_MB, EBufferPresetType::RWStructured, L"Particle");
		m_particle_allocator[1] = CreatePersistentAllocator(args.render_core, sizeof(ParticleData), 12_MB, EBufferPresetType::RWStructured, L"Particle");

		m_emit_command_allocator = CreateTransientAllocator(args.render_core, sizeof(ParticleEmitCommand), 4_MB, EBufferPresetType::UploadStructured, L"ParticleEmitCommand");

		m_particle_counter = args.render_core->CreateBuffer(32, 64, EBufferPresetType::RWStructured);
		m_particle_counter->SetName(L"ParticleCounter");
		

		m_emit_cs = RenderCore::Get().CreateShader(L"Asset/ParticleEmitCS.hlsl", ERHIShaderType::Compute);
		m_update_cs = RenderCore::Get().CreateShader(L"Asset/ParticleUpdateCS.hlsl", ERHIShaderType::Compute);
		m_reset_cs = RenderCore::Get().CreateShader(L"Asset/ParticleResetCS.hlsl", ERHIShaderType::Compute);

		m_layout = RenderCore::Get().CreatePipelineLayout(
			[](PipelineLayout& layout)
			{
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
			}
		);


		m_emit_pipeline = RenderCore::Get().CreateComputePipeline(
			{
				.pipeline_layout = m_layout.get(),
				.compute_shader = m_emit_cs.get(),
			}
			);

		m_update_pipeline = RenderCore::Get().CreateComputePipeline(
			{
				.pipeline_layout = m_layout.get(),
				.compute_shader = m_update_cs.get(),
			}
			);

		m_reset_pipeline = RenderCore::Get().CreateComputePipeline(
			{
				.pipeline_layout = m_layout.get(),
				.compute_shader = m_reset_cs.get(),
			}
			);


		return true;
	}

	void ParticleSystem::Finalize()
	{
	}

	struct ParticleBufferIndices
	{
		UInt32 read_particle;
		UInt32 write_particle;

		UInt32 emit_command;
		UInt32 particle_counter;
	};

	struct ParticleFrameContext
	{
		UInt32 read_counter_index;
		UInt32 write_counter_index;

		UInt32 max_particle_count;
		UInt32 emit_command_count;

		float delta_time;
	};

	void ParticleSystem::Update(entt::registry& registry, ParticleContext& ctx)
	{
		m_emit_command_allocator->Reset(ctx.current_frame_index);
		m_emit_commands.clear();

		UInt32 read_index = m_counter_index;
		UInt32 write_index = SwapIndex(m_counter_index);

		auto read_allocator = m_particle_allocator[read_index].get();
		auto write_allocator = m_particle_allocator[write_index].get();

		ctx.command_list->SetPipelineLayout(m_layout.get());

		ParticleBufferIndices buffer_indices = {};
		buffer_indices.emit_command = m_emit_command_allocator->GetSRVIndex();
		buffer_indices.particle_counter = m_particle_counter->GetUAVHandle().index;
		buffer_indices.read_particle = read_allocator->GetSRVIndex();
		buffer_indices.write_particle = write_allocator->GetUAVIndex();

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&buffer_indices);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
		}

		auto emitter_view = registry.view<ParticleEmitterComponent, TransformComponent>();

		// このフレームで生成するエミッター情報だけを詰める
		for (auto [entity, emitter_com, transform] : emitter_view.each())
		{
			emitter_com.spawn_accumulator += ctx.delta_time * emitter_com.emission_rate;

			// 整数分だけ発生させる
			UInt32 spawn_count = static_cast<UInt32>(emitter_com.spawn_accumulator);

			// 小数点以下だけを残す
			emitter_com.spawn_accumulator -= spawn_count;

			if (spawn_count <= 0)
				continue;

			ParticleEmitCommand command = {};
			command.spawn_count = spawn_count;
			command.world_position = transform.GetWorldMatrix()._4();
			command.lifetime = emitter_com.lifetime;

			m_emit_commands.push_back(command);
		}

		m_emit_command_allocator->WriteData(m_emit_commands.data(), m_emit_commands.size());


		ParticleFrameContext constants = {};
		constants.read_counter_index = read_index;
		constants.write_counter_index = write_index;
		constants.max_particle_count = 10000;
		constants.emit_command_count = m_emit_commands.size();
		constants.delta_time = ctx.delta_time;

		// カウントリセット
		ctx.command_list->SetComputePipeline(m_reset_pipeline.get());


		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);
		}

		ctx.command_list->Transition(write_allocator->GetBuffer(), ERHIResourceState::UnorderedAccess);
		ctx.command_list->Transition(m_particle_counter.get(), ERHIResourceState::UnorderedAccess);

		ctx.command_list->Dispatch(1, 1, 1);

		ctx.command_list->UAVBarrier(write_allocator->GetBuffer());
		ctx.command_list->UAVBarrier(m_particle_counter.get());

		// 生存中のパーティクル更新
		ctx.command_list->SetComputePipeline(m_update_pipeline.get());

		// 更新パス(パーティクル単位)
		const UInt32 update_groups = RoundUp(constants.max_particle_count, 64);

		ctx.command_list->Dispatch(update_groups, 1, 1);

		ctx.command_list->UAVBarrier(write_allocator->GetBuffer());

		// 生成パス
		if (constants.emit_command_count > 0)
		{
			ctx.command_list->SetComputePipeline(m_emit_pipeline.get());

			const UInt32 dispatch_x = RoundUp(m_emit_commands.size(), 64);

			ctx.command_list->Dispatch(dispatch_x, 1, 1);

			ctx.command_list->UAVBarrier(write_allocator->GetBuffer());
		}

		// 描画用、かつ次のフレームのreadになる
		ctx.command_list->Transition(write_allocator->GetBuffer(), ERHIResourceState::NonPixelShaderResource);

	}
}