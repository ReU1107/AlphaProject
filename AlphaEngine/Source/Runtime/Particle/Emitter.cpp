#include "Emitter.h"
#include "Particle.h"

#include "Emission/Emission.h"
#include "Behavior/Behavior.h"

#include "RenderCore/RenderCore.h"
#include "RenderCore/CommandList.h"
#include "RenderCore/Buffer/RingBuffer.h"
#include "RenderCore/Buffer/StructuredBuffer.h"
#include "RenderCore/Buffer/ByteAddressBuffer.h"

#include "RenderCore/DescriptorTable.h"


namespace Alpha::Particle
{
	Emitter::Emitter()
	{
	}

	Emitter::~Emitter()
	{
	}

	void Emitter::Initialize(const Args& args)
	{
		m_particle_pool = args.core->CreateStructuredBuffer(sizeof(Unit), args.max_count, EResourceAccessFlags::ReadWrite);
		
		m_dead_list = args.core->CreateByteAddressBuffer(args.max_count + 1, EResourceAccessFlags::ReadWrite);

		m_max_num = args.max_count;

		m_table = args.core->CreateDescriptorTable(
			args.heap,
			[&](DescriptorTable& layout)
			{
				layout.AddUAV(m_particle_pool.get());
				layout.AddUAV(m_dead_list.get());
			}
		);
	}

	void Emitter::SetEmission(TUnique<Emission> emission)
	{
		m_emission = std::move(emission);
	}

	void Emitter::AddBehavior(TUnique<Behavior> behavior)
	{
		m_behaviors.push_back(std::move(behavior));
	}

	void Emitter::Update(Context& ctx)
	{
		m_spawn_accumulator += ctx.delta_time * m_spawn_rate;

		// 整数分だけ発生させる
		uint32_t spawn_num = static_cast<uint32_t>(m_spawn_accumulator);

		// 小数点以下だけを残す
		m_spawn_accumulator -= spawn_num;

		ctx.table = m_table.get();
		ctx.particle_pool = m_particle_pool.get();
		ctx.dead_list = m_dead_list.get();

		EmitContext emit_ctx = { ctx };
		emit_ctx.emit_max_num = m_max_num;
		emit_ctx.emit_num = spawn_num;
		emit_ctx.life_time = m_life_time;
		emit_ctx.color = m_base_color;


		// 生成モジュール実行
		if (spawn_num > 0)
			m_emission->Emit(emit_ctx);

		UpdateContext update_ctx = { ctx };
		update_ctx.particle_num = m_max_num;

		// 生成済みのモジュールの更新
		for (auto& behavior : m_behaviors)
		{
			behavior->Execute(update_ctx);

			// 一つ前のモジュールが完全に
			ctx.command_list->UAVBarrier(m_particle_pool.get());
			//ctx.command_list->UAVBarrier(m_dead_list);

		}
	}

	void Emitter::Play()
	{
	}

	void Emitter::Stop()
	{
	}

	void Emitter::Pause()
	{
	}

	void Emitter::Clear()
	{
	}

	void Emitter::Accept(IVisitor* visitor)
	{
	}
}