#include "ParticleSystem.h"

#include "ParticleEmitterComponent.h"

#include "RenderCore/Module.h"
#include "Renderer/MeshComponent.h"

#include "Spatial/TransformComponent.h"

#include "Core/Math/MathUtil.h"

namespace Alpha::Particle
{
	ParticleSystem::ParticleSystem() = default;;
	ParticleSystem::~ParticleSystem() = default;;

	bool ParticleSystem::Initialize(const Args& args)
	{
		RenderCore* render_core = args.render_core;
		m_particle_allocator[0] = CreatePersistentAllocator(args.render_core, sizeof(ParticleData), 12_MB, EBufferPresetType::RWStructured, L"ParticleA");
		m_particle_allocator[1] = CreatePersistentAllocator(args.render_core, sizeof(ParticleData), 12_MB, EBufferPresetType::RWStructured, L"ParticleB");

		m_emit_command_allocator = CreateTransientAllocator(args.render_core, sizeof(ParticleEmitCommand), 4_MB, EBufferPresetType::UploadStructured, L"ParticleEmitCommand");

		m_particle_counter = args.render_core->CreateBuffer(sizeof(UInt32), sizeof(UInt64), EBufferPresetType::RWStructured);
		m_particle_counter->SetName(L"ParticleCounter");
		
		auto emit_cs = args.render_core->CreateShader(L"Asset/ParticleEmitCS.hlsl", ERHIShaderType::Compute);
		auto update_cs = args.render_core->CreateShader(L"Asset/ParticleUpdateCS.hlsl", ERHIShaderType::Compute);
		auto reset_cs = args.render_core->CreateShader(L"Asset/ParticleResetCS.hlsl", ERHIShaderType::Compute);

		m_emit_pipeline = args.render_core->CreateComputePipeline(
			{
				.pipeline_layout = args.layout,
				.compute_shader = emit_cs,
			}
		);

		m_update_pipeline = args.render_core->CreateComputePipeline(
			{
				.pipeline_layout = args.layout,
				.compute_shader = update_cs,
			}
		);

		m_reset_pipeline = args.render_core->CreateComputePipeline(
			{
				.pipeline_layout = args.layout,
				.compute_shader = reset_cs,
			}
		);

		m_max_particle_count = args.max_particle_count;


		return true;
	}

	void ParticleSystem::Finalize()
	{
	}


	void ParticleSystem::Update(entt::registry& registry, ParticleContext& ctx)
	{
		m_emit_command_allocator->Reset(ctx.current_frame_index);
		m_emit_commands.clear();

		UInt32 read_index = m_counter_index;
		UInt32 write_index = SwapIndex(m_counter_index);

		auto read_allocator = m_particle_allocator[read_index].get();
		auto write_allocator = m_particle_allocator[write_index].get();

		ParticleBufferIndices buffer_indices = {};
		buffer_indices.emit_command = m_emit_command_allocator->GetSRVIndex();
		buffer_indices.particle_counter = m_particle_counter->GetUAVHandle().index;
		buffer_indices.read_particle = read_allocator->GetSRVIndex();
		buffer_indices.write_particle = write_allocator->GetUAVIndex();
		buffer_indices.render_particle = write_allocator->GetSRVIndex();

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&buffer_indices);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

			m_buffer_indices_offset = offset;
		}

		auto emitter_view = registry.view<ParticleEmitterComponent, TransformComponent>();

		constexpr UInt32 invalid_index = 0xFFFFFFFF;

		// このフレームで生成するエミッター情報だけを詰める
		for (auto [entity, emitter, transform] : emitter_view.each())
		{
			if (!emitter.emitting)
				continue;

			emitter.spawn_accumulator += ctx.delta_time * emitter.emission_rate;

			const UInt32 spawn_count = static_cast<UInt32>(emitter.spawn_accumulator);

			emitter.spawn_accumulator -= spawn_count;

			if (spawn_count == 0)
				continue;

			ParticleEmitCommand command{};
			command.emitter_world = transform.GetWorldMatrix();
			command.shape_params = emitter.shape_params;

			command.initial_velocity = emitter.initial_velocity;
			command.velocity_random = emitter.velocity_random;
			command.random_seed = emitter.random_seed;

			command.start_color = emitter.start_color;
			command.end_color = emitter.end_color;
			command.start_size = emitter.start_size;
			command.end_size = emitter.end_size;
			command.lifetime = emitter.lifetime;
			command.spawn_count = spawn_count;

			command.source_shape = static_cast<UInt32>(emitter.source_shape);
			command.distribution = static_cast<UInt32>(emitter.distribution);
			command.source_instance_index = invalid_index;
			command.material_index = invalid_index;
			command.flags = 0;


			if (emitter.source_shape == SourceShape::Mesh)
			{
				const auto* mesh = registry.try_get<Renderer::MeshComponent>(entity);

				// このフレームの生成元インスタンスがない場合は生成しない。
				if (!mesh || mesh->render_instance_index == invalid_index)
					continue;

				command.source_instance_index = mesh->render_instance_index;
			}

			m_emit_commands.push_back(command);
		}

		m_emit_command_allocator->WriteData(m_emit_commands.data(), m_emit_commands.size());

		m_time += ctx.delta_time;
		ParticleUpdateConstants constants = {};
		constants.read_counter_index = read_index;
		constants.write_counter_index = write_index;
		constants.max_particle_count = m_max_particle_count;
		constants.emit_command_count = m_emit_commands.size();
		constants.time = m_time;
		constants.delta_time = ctx.delta_time;
		constants.simulation_step = m_simulation_step++;
		// カウントリセット
		ctx.command_list->SetComputePipeline(m_reset_pipeline);


		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 3);

			m_update_constants_offset = offset;
		}

		ctx.command_list->Transition(write_allocator->GetBuffer(), ERHIResourceState::UnorderedAccess);
		ctx.command_list->Transition(m_particle_counter.get(), ERHIResourceState::UnorderedAccess);

		ctx.command_list->Dispatch(1, 1, 1);

		ctx.command_list->UAVBarrier(write_allocator->GetBuffer());
		ctx.command_list->UAVBarrier(m_particle_counter.get());

		{
			auto handle = ctx.profiler->Begin(L"ParticleUpdate", ctx.command_list);

			// 生存中のパーティクル更新
			ctx.command_list->SetComputePipeline(m_update_pipeline);

			// 更新パス(パーティクル単位)
			const UInt32 update_groups = RoundUp(constants.max_particle_count, 64);

			ctx.command_list->Dispatch(update_groups, 1, 1);

			ctx.command_list->UAVBarrier(write_allocator->GetBuffer());
			ctx.command_list->UAVBarrier(m_particle_counter.get());


			ctx.profiler->End(handle, ctx.command_list);
		}


		// 生成パス
		if (constants.emit_command_count > 0)
		{
			auto handle = ctx.profiler->Begin(L"ParticleEmit", ctx.command_list);
			ctx.command_list->SetComputePipeline(m_emit_pipeline);

			const UInt32 dispatch_x = RoundUp(m_emit_commands.size(), 64);

			ctx.command_list->Dispatch(dispatch_x, 1, 1);

			ctx.command_list->UAVBarrier(write_allocator->GetBuffer());
			ctx.command_list->UAVBarrier(m_particle_counter.get());

			ctx.profiler->End(handle, ctx.command_list);
		}

		// 描画用、かつ次のフレームのreadになる
		ctx.command_list->Transition(write_allocator->GetBuffer(), ERHIResourceState::NonPixelShaderResource);

	}
}