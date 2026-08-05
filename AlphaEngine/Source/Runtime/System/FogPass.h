#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class PipelineLayout;
	class Shader;
	class Texture;
	class RayTracePipeline;

	struct RenderContext;

	class FogPass
	{
	private:
		TUnique<RayTracePipeline> m_pipeline{ nullptr };
		TUnique<Shader> m_shader{ nullptr };
		TUnique<Texture> m_result{};

	public:

		~FogPass();
		struct Args
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* layout{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Execute(RenderContext& ctx);


	};
}