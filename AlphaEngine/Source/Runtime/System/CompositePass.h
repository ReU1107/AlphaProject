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

	class CompositePass
	{
	private:
		TUnique<ComputePipeline> m_pipeline{ nullptr };
		TUnique<Shader> m_shader{ nullptr };
		Texture* m_result{ nullptr };
		Texture* m_volumetric{ nullptr };
		Texture* m_transmittance{ nullptr };
		Texture* m_base{ nullptr };

	public:

		~CompositePass();
		struct Args
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* layout{ nullptr };

			Texture* result{ nullptr };
			Texture* volumetric{ nullptr };
			Texture* transmittance{ nullptr };
			Texture* base{ nullptr };

		};
		bool Initialize(const Args& args);
		void Finalize();

		void Execute(RenderContext& ctx);

	};
}