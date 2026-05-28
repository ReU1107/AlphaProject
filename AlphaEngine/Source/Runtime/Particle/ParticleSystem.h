#pragma once
#include "Core/Subsystem.h"
#include "Core/Types.h"
#include "Context.h"

namespace Alpha
{
	class RenderCore;
	class CommandQueue;
	class CommandList;
	class Timeline;
	class PipelineLayout;
	class RingBuffer;

	namespace Particle
	{
		class Emitter;
	}

	class ParticleSystem : public Subsystem<ParticleSystem>
	{
	private:
		TUnique<CommandQueue> m_command_queue{};
		TUnique<CommandList> m_command_list{};
		TUnique<Timeline> m_timeline{};
		TUnique<PipelineLayout> m_shape_pipeline_layout{};
		TUnique<RingBuffer> m_dynamic_CB{};

		std::vector<Particle::Emitter*> m_emitters{};

	public:
		ParticleSystem();
		~ParticleSystem();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			uint32_t max_particle_count{ 1024 };
		};

		bool Initialize(const Args& args);
		void Finalize();

		Particle::Emitter* CreateEmitter();

		// パーティクルシステムの更新
		void Update(Particle::Context& context);

	};
}