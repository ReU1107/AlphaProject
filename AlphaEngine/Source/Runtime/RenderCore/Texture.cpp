#include "Texture.h"
#include "RenderCore.h"
#include "RenderHardwareInterface.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

#include "RHI/RHITexture.h"

namespace Alpha
{
	Texture::Texture() = default;
	
	Texture::~Texture() = default;

	void Texture::Initialize(const Args& args)
	{
		RHITextureDesc desc = {};

		if (args.swap_chain)
		{
			desc.swap_chain = args.swap_chain->GetRHI();
			desc.back_buffer_index = args.buffer_index;
		}
		else
		{
			desc.width = args.width;
			desc.height = args.height;
			desc.depth = args.depth;
			desc.mip_level = args.mip_level;
			desc.format = args.format;
			desc.dimension = args.dimension;
			desc.flags = args.bind_flags;
		}
		m_rhi = args.rhi->CreateTexture(desc);

		if (EnumHasFlag(args.bind_flags, ERHIResourceBindFlags::ShaderResource))
		{
			m_srv_handle = args.resource_heap->Allocate(this, ERHIResourceViewType::ShaderResourceView);
		}
		if (EnumHasFlag(args.bind_flags, ERHIResourceBindFlags::UnorderedAccess))
		{
			m_uav_handle = args.resource_heap->Allocate(this, ERHIResourceViewType::UnorderedAccessView);
		}
		if (EnumHasFlag(args.bind_flags, ERHIResourceBindFlags::RenderTarget))
		{
			m_rtv_handle = args.rtv_heap->Allocate(this, ERHIResourceViewType::RenderTargetView);
		}
		if (EnumHasFlag(args.bind_flags, ERHIResourceBindFlags::DepthStencil))
		{
			m_dsv_handle = args.dsv_heap->Allocate(this, ERHIResourceViewType::DepthStencilView);
		}

	}

	void Texture::Destroy(UInt32 current_frame_index)
	{
		if (m_srv_handle.IsActive())
		{
			m_srv_handle.Free(current_frame_index);
		}

		if (m_uav_handle.IsActive())
		{
			m_uav_handle.Free(current_frame_index);
		}

		if (m_rtv_handle.IsActive())
		{
			m_rtv_handle.Free(current_frame_index);
		}

		if (m_dsv_handle.IsActive())
		{
			m_dsv_handle.Free(current_frame_index);
		}
	}

	RHITexture* Texture::GetRHI() const
	{
		return m_rhi->As<RHITexture>();
	}

	void Texture::Upload(void* map_ptr, const void* data)
	{
		m_rhi->As<RHITexture>()->Upload(map_ptr, data);
	}

	UInt64 Texture::GetSize() const
	{
		return m_rhi->As<RHITexture>()->GetUploadSize();
	}

	ERHIResourceFormat Texture::GetFormat() const
	{
		return m_rhi->As<RHITexture>()->GetFormat();
	}
}