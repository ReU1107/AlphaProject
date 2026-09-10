#pragma once
#include "Core/Common.h"
#include "Definitions.h"
#include <entt/entt.hpp>

namespace Alpha::Renderer
{
	class RenderScene;
	class RenderGraph;
	struct RenderContext;

}

namespace Alpha
{

	class RenderCore;

	class RenderSceneBuilder;

	class RenderSystem
	{
	private:
		TUnique<Renderer::RenderScene> m_scene{ nullptr };
		TUnique<Renderer::RenderGraph> m_graph{ nullptr };
		PipelineLayout* m_pipeline_layout{ nullptr };
		TUnique<RenderSceneBuilder> m_scene_builder{ nullptr };

	public:
		~RenderSystem();
		struct Args
		{
			RenderCore* render_core{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Render(entt::registry& registry, Renderer::RenderContext& ctx);

		// 本当はよくないんだよな
		Renderer::RenderGraph* GetGraph() const { return m_graph.get(); };

	};
}