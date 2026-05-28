#include "Texture.h"
#include "RenderCore/RenderCore.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RenderCore/DescriptorHeap.h"

#include "RHI/RHIDescriptorHeap.h"
#include "RHI/RHITexture.h"

namespace Alpha
{
	Texture::Texture() = default;
	
	Texture::~Texture()
	{
		if (m_srv_handle.heap)
			m_srv_handle.heap->Free(m_srv_handle);
	}

	void Texture::Initialize(const Args& args)
	{
		RHITextureDesc desc = {};
		desc.width = args.width;
		desc.height = args.height;
		desc.format = args.format;
		desc.dimension = ERHIResourceDimension::Tex2D;
		m_rhi = args.rhi->CreateTexture(desc);

		if (!args.srv_heap)
			return;

		m_srv_handle = args.srv_heap->Allocate();

		RHIResourceViewDesc srv_desc = {};
		srv_desc.resource = m_rhi.get();
		srv_desc.heap_index = m_srv_handle.index;
		srv_desc.type = ERHIResourceViewType::ShaderResourceView;

		m_srv_handle.heap->GetRHI()->CreateView(srv_desc);
	}

	RHITexture* Texture::GetRHI() const
	{
		return m_rhi->As<RHITexture>();
	}

	void Texture::Upload(void* map_ptr, void* data)
	{
		m_rhi->As<RHITexture>()->Upload(map_ptr, data);
	}

	uint64_t Texture::GetSize() const
	{
		return m_rhi->As<RHITexture>()->GetUploadSize();
	}
}