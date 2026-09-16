#pragma once
#include "Core/Common.h"
#include <entt/entt.hpp>
#include "Definitions.h"

namespace Alpha
{
	class RenderCore;

	class PersistentBufferAllocator;
	class TransientBufferAllocator;
	class Buffer;
	class PipelineLayout;
	class CommandLayout;
	class ComputePipeline;
	class Shader;

	class GraphicsPipeline;
	class Texture;
}

namespace Alpha::Renderer
{
	class RenderResourceManager;
}

namespace Alpha::Particle
{

	class ParticleSystem
	{
	private:
		
		TUnique<TransientBufferAllocator> m_emit_command_allocator{ nullptr };
		TFixedArray<TUnique<PersistentBufferAllocator>, 2> m_particle_allocator{};
		TUnique<Buffer> m_particle_counter{ nullptr };

		TArray<ParticleEmitCommand> m_emit_commands{};

		ComputePipeline* m_emit_pipeline{ nullptr };

		ComputePipeline* m_update_pipeline{ nullptr };

		ComputePipeline* m_reset_pipeline{ nullptr };

		UInt32 m_buffer_indices_offset{ 0 };
		UInt32 m_update_constants_offset{ 0 };

		UInt32 m_counter_index{ 0 };

		UInt32 m_max_particle_count{ 10000 };

	public:
		ParticleSystem();
		~ParticleSystem();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			
			PipelineLayout* layout{ nullptr };

			UInt32 max_particle_count{ 10000 };

		};

		bool Initialize(const Args& args);
		void Finalize();

		void Update(entt::registry& registry, ParticleContext& ctx);

		UInt32 GetBufferIndicesOffset() const { return m_buffer_indices_offset; }
		UInt32 GetConstantsOffset() const { return m_update_constants_offset; }


	};
}