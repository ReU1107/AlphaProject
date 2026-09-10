#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class ParticleSystem;

	class Buffer;
	class PipelineLayout;
	class CommandLayout;
	class ComputePipeline;
	class Shader;
	class RenderResourceManager;

	class ParticleRender
	{
	private:
		TUnique<ComputePipeline> m_prerender_pipeline{ nullptr };
		TUnique<Shader> m_prerender_cs{ nullptr };
		TUnique<CommandLayout> m_command_layout{ nullptr };
		TUnique<Buffer> m_command_buffer{ nullptr };

	public:
		ParticleRender();
		~ParticleRender();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			ParticleSystem* particle_system{ nullptr };
		};

		bool Initialize(const Args& args);
		void Finalize();


		void Render(RenderContext& ctx);

	};
}