#pragma once
#include "RHI/Types.h"
#include "Core/Types.h"

namespace Alpha
{
	class RenderCore;
	class DescriptorHeap;

	struct RenderTarget
	{
		RenderTexture* texture{ nullptr };
		bool is_used{ false };

		bool IsUnused() const { return false; }



	};

	class RenderTargetPool
	{
	private:

		TUnique<DescriptorHeap> m_rtv_heap{ nullptr };
		TUnique<DescriptorHeap> m_dsv_heap{ nullptr };

		TArray<RenderTexture*> m_targets{};

	public:
		RenderTargetPool();
		~RenderTargetPool();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			uint32_t max_render_target_num{ 32 };
			uint32_t max_depth_stencil_num{ 8 };
		};

		void Initialize(const Args& args);

		void Finalize();

		RenderTexture* GetRenderTarget(int index) const { return m_targets[index]; }

		RenderTexture* Request(uint32_t width, uint32_t height, ERHIResourceFormat format);

	};
}