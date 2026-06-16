#include "Buffer.h"
#include "RenderHardwareInterface.h"
#include "DescriptorHeap.h"

#include "RHI/RHIBuffer.h"

namespace Alpha
{
	Buffer::Buffer() = default;
	Buffer::~Buffer()
	{
		if (m_srv_handle.IsActive())
		{
			m_srv_handle.Free();
		}
		if (m_uav_handle.IsActive())
		{
			m_uav_handle.Free();
		}
	}

	void Buffer::Initialize(const Args& args)
	{
		RHIBufferDesc buffer_desc = {};
		buffer_desc.stride = args.stride;
		buffer_desc.size = args.size;
		buffer_desc.access = args.access_type;
		buffer_desc.flags = args.bind_flags;

		m_rhi = args.rhi->CreateBuffer(buffer_desc);

		if (EnumHasFlag(args.bind_flags, ERHIResourceBindFlags::ShaderResource))
		{
			m_srv_handle = args.resource_heap->Allocate(this, ERHIResourceViewType::ShaderResourceView);
		}
		if (EnumHasFlag(args.bind_flags, ERHIResourceBindFlags::UnorderedAccess))
		{
			m_uav_handle = args.resource_heap->Allocate(this, ERHIResourceViewType::UnorderedAccessView);
		}

	}

	RHIBuffer* Buffer::GetRHI() const
	{
		return m_rhi->As<RHIBuffer>();
	}

	uint64_t Buffer::GetStride() const
	{
		return m_rhi->As<RHIBuffer>()->GetStride();
	}

	uint64_t Buffer::GetSize() const
	{
		return m_rhi->As<RHIBuffer>()->GetSize();
	}

	UInt8* Buffer::GetMappedPtr() const
	{
		return GetRHI()->Map(0, GetSize());;
	}
}
