#include "PersistentBufferAllocator.h"
#include "RenderCore/RenderCore.h"
#include "RenderCore/Buffer.h"
namespace Alpha
{
    PersistentBufferAllocator::~PersistentBufferAllocator() = default;

    bool PersistentBufferAllocator::Initialize(const Args& args)
    {
        m_buffer = args.render_core->CreateBuffer(args.alignment, args.max_size, args.preset);
        if (!m_buffer)
            return false;

        m_allocator.Initialize(args.max_size);

        m_alignment = args.alignment;  // 

        return true;
    }

    bool PersistentBufferAllocator::Allocate(UInt32 num, UInt64* out_offset)
    {
        UInt64 size = num * m_alignment;
        FreeListAllocator::Allocation alloc = m_allocator.Allocate(size, m_alignment);
        *out_offset = alloc.offset;

        return true;
    }

    bool PersistentBufferAllocator::AllocateBySize(UInt32 size, UInt64* out_offset)
    {
        FreeListAllocator::Allocation alloc = m_allocator.Allocate(size, m_alignment);
        *out_offset = alloc.offset;

        return true;
    }
}