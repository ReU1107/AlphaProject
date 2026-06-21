#include "TransientBufferAllocator.h"
#include "RenderCore/RenderCore.h"
#include "RenderCore/Buffer.h"
#include "Core/Math/MathUtil.h"

namespace Alpha
{
	TransientBufferAllocator::TransientBufferAllocator()
	{
	}

	TransientBufferAllocator::~TransientBufferAllocator()
	{
	}

	void TransientBufferAllocator::Initialize(const Args& args)
	{
		m_buffer = args.render_core->CreateBuffer(args.stride, args.size, EBufferPresetType::Upload);

		m_map_ptr = m_buffer->GetMappedPtr();

	}

	UInt64 TransientBufferAllocator::WriteData(const void* data, UInt64 size)
	{
		UInt64 alignment_size = Alignment(size, 256);

		memcpy(m_map_ptr + m_current_offset, data, alignment_size);

		UInt64 offset = m_current_offset;
		m_current_offset += alignment_size;

		return offset;
	}

	UInt8* TransientBufferAllocator::Allocate(UInt64 size, UInt64 alignment, UInt64* out_offset)
	{
		UInt64 alignment_size = Alignment(size, alignment);
		*out_offset = m_current_offset;

		UInt8* dest = m_map_ptr + m_current_offset;
		m_current_offset += alignment_size;

		return dest;
	}

	Buffer* TransientBufferAllocator::GetBuffer() const
	{
		return m_buffer.get();
	}

	void TransientBufferAllocator::Reset()
	{
		m_current_offset = 0;
	}
}