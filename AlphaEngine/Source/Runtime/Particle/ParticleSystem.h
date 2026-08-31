#pragma once
#include "Core/Common.h"
#include "Context.h"
#include <entt/entt.hpp>
#include "Definitions.h"

namespace Alpha
{
	class RenderCore;

	class PersistentBufferAllocator;
	class TransientBufferAllocator;
	class Buffer;
	class PipelineLayout;
	class ComputePipeline;
	class Shader;

	class ParticleSystem
	{
	private:
		
		TUnique<TransientBufferAllocator> m_emit_command_allocator{ nullptr };
		TFixedArray<TUnique<PersistentBufferAllocator>, 2> m_particle_allocator{};
		TUnique<Buffer> m_particle_counter{ nullptr };

		TArray<ParticleEmitCommand> m_emit_commands{};

		TUnique<PipelineLayout> m_layout{ nullptr };

		TUnique<ComputePipeline> m_emit_pipeline{ nullptr };
		TUnique<Shader> m_emit_cs{ nullptr };

		TUnique<ComputePipeline> m_update_pipeline{ nullptr };
		TUnique<Shader> m_update_cs{ nullptr };

		TUnique<ComputePipeline> m_reset_pipeline{ nullptr };
		TUnique<Shader> m_reset_cs{ nullptr };

		UInt32 m_counter_index{ 0 };

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

		void Update(entt::registry& registry, ParticleContext& ctx);

	};
}