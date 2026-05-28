#include "RingBuffer.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RHI/RHIBuffer.h"
#include "Core/Math/MathUtil.h"

namespace Alpha
{
	RingBuffer::RingBuffer() = default;
	RingBuffer::~RingBuffer() = default;

	void RingBuffer::Initialize(const Args& args)
	{
		RHIBufferDesc buffer_desc = {};
		buffer_desc.size = Alignment(args.base.stride, args.alignment) * args.base.element_count;
		buffer_desc.access = ERHIBufferAccess::Dynamic;
		buffer_desc.stride = args.base.stride;

		m_rhi = args.base.rhi->CreateBuffer(buffer_desc);

		m_map_ptr = GetRHI()->Map(0, GetSize());

	}

	UInt64 RingBuffer::WriteData(const void* data, UInt64 size)
	{
		UInt64 alignment_size = Alignment(size, 256);

		memcpy(m_map_ptr + m_current_offset, data, alignment_size);

		UInt64 offset = m_current_offset;
		m_current_offset += alignment_size;

		return offset;
	}

	UInt8* RingBuffer::Allocate(UInt64 size, UInt64 alignment, UInt64* out_offset)
	{
		UInt64 alignment_size = Alignment(size, alignment);
		// TODO: バッファ末尾を超えた場合のラップアラウンド処理
		*out_offset = m_current_offset;

		UInt8* dest = m_map_ptr + m_current_offset;
		m_current_offset += alignment_size;

		return dest;
	}

	void RingBuffer::Reset()
	{
		m_current_offset = 0;
	}
}