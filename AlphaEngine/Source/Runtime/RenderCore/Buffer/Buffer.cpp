#include "Buffer.h"
#include "RHI/RHIBuffer.h"

namespace Alpha
{
	Buffer::Buffer() = default;
	Buffer::~Buffer() = default;

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

	ERHIBufferBindFlags Buffer::GetFlags() const
	{
		return GetRHI()->GetFlags();
	}

	void Buffer::Upload(void* map_ptr, void* data, uint64_t data_size)
	{
		m_rhi->As<RHIBuffer>()->Upload(map_ptr, data, data_size);
	}

}
