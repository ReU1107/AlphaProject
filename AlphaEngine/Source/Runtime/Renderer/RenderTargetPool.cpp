#include "RenderTargetPool.h"
#include "RenderCore/RenderCore.h"

#include "RenderCore/DescriptorHeap.h"

namespace Alpha
{
	RenderTargetPool::RenderTargetPool()
	{
	}

	RenderTargetPool::~RenderTargetPool()
	{
	}

	void RenderTargetPool::Initialize(const Args& args)
	{
		m_rtv_heap = args.render_core->CreateDescriptorHeap(args.max_render_target_num, ERHIDescriptorType::RTV);
		m_dsv_heap = args.render_core->CreateDescriptorHeap(args.max_depth_stencil_num, ERHIDescriptorType::DSV);
	}

	void RenderTargetPool::Finalize()
	{
		// RenderTextureの解放
		for (auto target : m_targets)
		{
		}

		m_rtv_heap.reset();
		m_dsv_heap.reset();

	}

	RenderTexture* RenderTargetPool::Request(uint32_t width, uint32_t height, ERHIResourceFormat format)
	{
		// 使われていないレンダーターゲットがあればそれを返す

		// なければ新規作成して返す

		// とりあえず未実装

		return nullptr;
	}
}