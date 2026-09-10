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

namespace Alpha
{

	class ParticleSystem
	{
	private:
		
		TUnique<TransientBufferAllocator> m_emit_command_allocator{ nullptr };
		TFixedArray<TUnique<PersistentBufferAllocator>, 2> m_particle_allocator{};
		TUnique<Buffer> m_particle_counter{ nullptr };
		TUnique<Buffer> m_command_buffer{ nullptr };

		TArray<ParticleEmitCommand> m_emit_commands{};

		TUnique<PipelineLayout> m_layout{ nullptr };
		TUnique<CommandLayout> m_command_layout{ nullptr };

		TUnique<ComputePipeline> m_emit_pipeline{ nullptr };
		TUnique<Shader> m_emit_cs{ nullptr };

		TUnique<ComputePipeline> m_update_pipeline{ nullptr };
		TUnique<Shader> m_update_cs{ nullptr };

		TUnique<ComputePipeline> m_reset_pipeline{ nullptr };
		TUnique<Shader> m_reset_cs{ nullptr };

		TUnique<ComputePipeline> m_prerender_pipeline{ nullptr };
		TUnique<Shader> m_prerender_cs{ nullptr };


		TUnique<GraphicsPipeline> m_sprite_pipeline{ nullptr };
		TUnique<Shader> m_sprite_ms{ nullptr };
		TUnique<Shader> m_sprite_ps{ nullptr };


		UInt32 m_counter_index{ 0 };

		UInt32 m_max_particle_count{ 10000 };

	public:
		ParticleSystem();
		~ParticleSystem();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			
			Texture* render_target{ nullptr };
			Texture* depth_target{ nullptr };

			UInt32 max_particle_count{ 10000 };

		};

		bool Initialize(const Args& args);
		void Finalize();

		void Update(entt::registry& registry, ParticleContext& ctx);

		//Buffer* GetParticleBuffer() const { return m_particle_allocator[m_counter_index]->GetBuffer(); }
		//Buffer* GetCommandBuffer() const { return m_command_buffer.get(); }

		//CommandLayout* GetCommandLayout() const { return m_command_layout.get(); }

	};
}