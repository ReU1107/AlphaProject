#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class PipelineLayout;
	class Shader;
	class Texture;
	class ComputePipeline;

	struct RenderContext;

	class FroxelLightingPass
	{
	private:
		TUnique<ComputePipeline> m_pipeline{ nullptr };
		TUnique<Shader> m_shader{ nullptr };
		TUnique<Texture> m_result{};

	public:

		~FroxelLightingPass();
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
