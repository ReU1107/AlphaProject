#pragma once
#include "Core/Common.h"
#include "Definitions.h"
#include <entt/entt.hpp>

namespace Alpha
{
	class RenderCore;
	class TransientBufferAllocator;
	class VertexComputePass;
	class PipelineLayout;

	struct RenderContext;

	class RenderScene
	{
	private:
		TUnique<TransientBufferAllocator> m_instance_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_vert_to_inst_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_instance_tlas_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_scratch_tlas_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_result_tlas_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_result_blas_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_dynamic_vertex_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_override_material_allocator{ nullptr };

		TArray<InstanceData> m_instances{};

		TUnique<VertexComputePass> m_vertex_compute_pass{ nullptr };

	public:

		~RenderScene();
		struct Args
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* layout{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Build(entt::registry& registry, RenderContext& ctx);

		DynamicBufferIndices GetBufferIndices() const;

		UInt32 GetInstanceCount() const { return static_cast<UInt32>(m_instances.size()); }

	};
}