#pragma once
#include "Core/Subsystem.h"
#include "Definitions.h"

namespace Alpha
{
	struct RenderContext;

	class RenderCore;
	class TransientBufferAllocator;

	class RenderSystem : public Subsystem<RenderSystem>
	{
	private:
		TUnique<TransientBufferAllocator> m_instance_allocator{ nullptr };
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