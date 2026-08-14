#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class GraphicsPipeline;
	class PipelineLayout;
	class Shader;
	class Texture;

	struct RenderContext;


	class VBufferPass
	{
	private:
		TUnique<GraphicsPipeline> m_pipeline{ nullptr };
		TUnique<Shader> m_as{ nullptr };
		TUnique<Shader> m_ms{ nullptr };
		TUnique<Shader> m_ps{ nullptr };
		TUnique<Texture> m_visibility{};
		TUnique<Texture> m_depth{};

	public:

		~VBufferPass();
		struct Args
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* layout{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Execute(RenderContext& ctx);

		Texture* GetVisibilityTex() const { return m_visibility.get(); }

		Texture* GetDepthTex() const { return m_depth.get(); }

	};
}