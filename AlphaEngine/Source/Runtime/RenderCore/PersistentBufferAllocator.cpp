#include "PooledBuffer.h"
#include "RenderCore/RenderCore.h"

namespace Alpha
{
	bool PooledBuffer::Initialize(UInt64 stride, UInt64 total_size, EBufferPresetType preset)
    {
        m_buffer = RenderCore::Get().CreateBuffer(stride, total_size, preset);
        if (!m_buffer) 
            return false;

        m_allocator.Initialize(total_size);

        return true;
    }
}