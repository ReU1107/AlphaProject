#include "DepthTexture.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RenderCore/DescriptorHeap.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIDescriptorHeap.h"

namespace Alpha
{
	DepthTexture::DepthTexture() = default;

	DepthTexture::~DepthTexture()
	{
		if (m_dsv_handle.heap)
			m_dsv_handle.heap->Free(m_dsv_handle);
	}

	void DepthTexture::Initialize(const Args& args)
	{
		RHITextureDesc desc = {};
		desc.width = args.base.width;
		desc.height = args.base.height;
		desc.format = args.base.format;
		desc.dimension = ERHIResourceDimension::Tex2D;
		desc.flags |= ERHIResourceFlags::AllowDepthStencil;
		m_rhi = args.base.rhi->CreateTexture(desc);
		
		m_dsv_handle = args.dsv_heap->Allocate();

		// DSVの作成
		RHIResourceViewDesc dsv_desc = {};
		dsv_desc.resource = m_rhi.get();
		dsv_desc.heap_index = m_dsv_handle.index;
		dsv_desc.type = ERHIResourceViewType::DepthStencilView;

		m_dsv_handle.heap->GetRHI()->CreateView(dsv_desc);
	}
}