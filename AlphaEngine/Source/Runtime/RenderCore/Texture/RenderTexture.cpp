#include "RenderTexture.h"
#include "RenderCore/DescriptorHeap.h"
#include "RenderCore/SwapChain.h"
#include "RenderCore/RenderHardwareInterface.h"

#include "RHI/RHIDescriptorHeap.h"
#include "RHI/RHITexture.h"

namespace Alpha
{
	RenderTexture::RenderTexture() = default;

	RenderTexture::~RenderTexture()
	{
		if (m_rtv_handle.heap)
			m_rtv_handle.heap->Free(m_rtv_handle);

	}

	void RenderTexture::InitializeFromSwapChain(const Args& args)
	{
		RHITextureDesc desc = {};
		desc.swap_chain = args.swap_chain->GetRHI();
		desc.back_buffer_index = args.buffer_index;
		m_rhi = args.base.rhi->CreateTexture(desc);

		m_rtv_handle = args.rtv_heap->Allocate();

		// RTV 作成
		RHIResourceViewDesc rtv_desc = {};
		rtv_desc.resource = m_rhi.get();
		rtv_desc.heap_index = m_rtv_handle.index;
		rtv_desc.type = ERHIResourceViewType::RenderTargetView;

		m_rtv_handle.heap->GetRHI()->CreateView(rtv_desc);
	}

	void RenderTexture::Initialize(const Args& args)
	{
		RHITextureDesc desc = {};
		desc.width = args.base.width;
		desc.height = args.base.height;
		desc.format = args.base.format;
		desc.dimension = ERHIResourceDimension::Tex2D;
		desc.flags |= ERHIResourceFlags::AllowRenderTarget;
		m_rhi = args.base.rhi->CreateTexture(desc);

		m_rtv_handle = args.rtv_heap->Allocate();

		// RTV 作成
		RHIResourceViewDesc rtv_desc = {};
		rtv_desc.resource = m_rhi.get();
		rtv_desc.heap_index = m_rtv_handle.index;
		rtv_desc.type = ERHIResourceViewType::RenderTargetView;

		m_rtv_handle.heap->GetRHI()->CreateView(rtv_desc);
	}

}