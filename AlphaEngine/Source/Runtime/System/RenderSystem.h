#pragma once
#include "Core/Subsystem.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
	struct RenderContext;

	class RenderCore;
	class PersistentBufferAllocator;
	class TransientBufferAllocator;

	class RenderSystem : public Subsystem<RenderSystem>
	{
	private:
		TUnique<TransientBufferAllocator> m_instance_allocator{ nullptr };
		TUnique<PersistentBufferAllocator> m_result_as_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_scratch_as_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_update_as_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_dynamic_vertex_allocator{ nullptr };


		TArray<Renderer::InstanceData> m_instances{};
	public:
		~RenderSystem();
		struct Args
		{
			RenderCore* render_core{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Render(RenderContext& ctx);


	};
}