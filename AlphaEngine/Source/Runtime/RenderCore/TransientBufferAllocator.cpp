#include "TransientBufferAllocator.h"
#include "RenderCore.h"
#include "Buffer.h"
#include "Core/Math/MathUtil.h"

namespace Alpha
{
	TransientBufferAllocator::~TransientBufferAllocator() = default;

	void TransientBufferAllocator::Initialize(const Args& args)
	{
		m_blocks.resize(args.block_num);
		for (UInt32 i = 0; i < args.block_num; ++i)
		{
			auto& block = m_blocks[i];
			block.buffer = args.render_core->CreateBuffer(args.stride, args.size, args.type);
			block.map_ptr = block.buffer->GetMappedPtr();

		}

		m_alignment = args.alignment;
	}

	UInt8* TransientBufferAllocator::Allocate(UInt64 size, UInt64* out_offset)
	{
		auto& block = m_blocks[m_current_block_index];

		UInt64 aligned_offset = Alignment(block.current_offset, m_alignment);

		UInt64 aligned_size = Alignment(size, m_alignment);

		*out_offset = aligned_offset;

		UInt8* dest = block.map_ptr + aligned_offset;
		// alignmentしたサイズ分までは結局使うので安全のために0クリアだけしておく
		std::memset(dest, 0, aligned_size);

		block.current_offset = aligned_size + aligned_offset;

		return dest;
	}

	UInt64 TransientBufferAllocator::AllocateOffset(UInt64 size)
	{
		auto& block = m_blocks[m_current_block_index];

		UInt64 aligned_offset = Alignment(block.current_offset, m_alignment);

		UInt64 aligned_size = Alignment(size, m_alignment);

		block.current_offset = aligned_offset + aligned_size;

		return aligned_offset;
	}

	Buffer* TransientBufferAllocator::GetBuffer() const
	{
		return m_blocks[m_current_block_index].buffer.get();
	}

	UInt32 TransientBufferAllocator::GetSRVIndex() const
	{
		return GetBuffer()->GetSRVHandle().index;
	}

	UInt32 TransientBufferAllocator::GetUAVIndex() const
	{
		return GetBuffer()->GetUAVHandle().index;
	}

	void TransientBufferAllocator::Reset(UInt32 current_buffer_index)
	{
		m_blocks[current_buffer_index].current_offset = 0;
		m_current_block_index = current_buffer_index;
	}

	void TransientBufferAllocator::SetName(StringView name)
	{
		for (auto& block : m_blocks)
		{
			block.buffer->SetName(name);
		}
	}
}