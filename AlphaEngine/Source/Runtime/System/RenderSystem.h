#pragma once
#include "Core/Common.h"
#include "Definitions.h"
#include <entt/entt.hpp>

namespace Alpha
{
	struct RenderContext;

	class RenderCore;

	class PipelineLayout;
	class GraphicsPipeline;
	class ComputePipeline;
	class RayTracePipeline;
	class Shader;
	class Texture;

	class RenderScene;
	class VBufferPass;
	class FroxelLightingPass;
	class FogPass;

	class RenderSystem
	{
	private:
		TUnique<RenderScene> m_scene{ nullptr };

		TUnique<PipelineLayout> m_layout{ nullptr };
		TUnique<RayTracePipeline> m_raytrace_pipeline{ nullptr };
		TUnique<VBufferPass> m_vbuffer_pass{ nullptr };
		TUnique<FroxelLightingPass> m_froxel_pass{ nullptr };
		TUnique<FogPass> m_fog_pass{ nullptr };





	public:
		~RenderSystem();
		struct Args
		{
			RenderCore* render_core{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Render(entt::registry& registry, RenderContext& ctx);


	};
}