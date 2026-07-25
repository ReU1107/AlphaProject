#pragma once
#include "Core/Subsystem.h"
#include "Definitions.h"
#include <entt/entt.hpp>

namespace Alpha
{
	struct RenderContext;

	class RenderCore;
	class PersistentBufferAllocator;
	class TransientBufferAllocator;
	class PipelineLayout;
	class GraphicsPipeline;
	class ComputePipeline;

	class RenderSystem : public Subsystem<RenderSystem>
	{
	private:
		TUnique<TransientBufferAllocator> m_instance_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_vert_to_inst_allocator{ nullptr };

		TUnique<PersistentBufferAllocator> m_result_as_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_update_as_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_dynamic_vertex_allocator{ nullptr };

		TUnique<PipelineLayout> m_layout{ nullptr };
		TUnique<GraphicsPipeline> m_pipeline{ nullptr };
		TUnique<ComputePipeline> m_align_vertex_pipeline{ nullptr };
		TArray<InstanceData> m_instances{};
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