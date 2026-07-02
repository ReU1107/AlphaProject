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
		m_alignment = args.alignment;
	}

	UInt8* TransientBufferAllocator::Allocate(UInt64 size, UInt64* out_offset)
	{
		UInt64 aligned_offset = Alignment(m_current_offset, m_alignment);

		UInt64 aligned_size = Alignment(size, m_alignment);

		*out_offset = aligned_offset;

		UInt8* dest = m_map_ptr + aligned_offset;
		// alignmentしたサイズ分までは結局使うので安全のために0クリアだけしておく
		std::memset(dest, 0, aligned_size);

		m_current_offset = aligned_size + aligned_offset;

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