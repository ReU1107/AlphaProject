#include "PersistentBufferAllocator.h"
#include "RenderCore.h"
#include "Buffer.h"
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

    bool PersistentBufferAllocator::AllocateByNum(UInt32 num, Allocation* allocation)
    {
        return AllocateBySize(num * m_alignment, allocation);
    }

    bool PersistentBufferAllocator::AllocateBySize(UInt32 size, Allocation* allocation)
    {
        Allocation alloc = m_allocator.Allocate(size, m_alignment);

        if (!alloc.is_valid)
            return false;

        *allocation = alloc;

        m_used_size += alloc.size;

        return true;
    }

    void PersistentBufferAllocator::Free(const Allocation& allocation)
    {
        UInt64 size = allocation.size;
        m_allocator.Free(allocation);
        m_used_size -= size;
    }

    UInt32 PersistentBufferAllocator::GetSRVIndex() const
    {
        return m_buffer->GetSRVHandle().index;
    }

    UInt32 PersistentBufferAllocator::GetUAVIndex() const
    {
        // 必ずBufferがWrite系であることを確認してから使う

        return m_buffer->GetUAVHandle().index;
    }

    UInt64 PersistentBufferAllocator::GetCapacity() const
    {
        return m_allocator.GetTotalSize();
    }

    UInt64 PersistentBufferAllocator::GetUsedSize() const
    {
        return m_used_size;
    }

    UInt64 PersistentBufferAllocator::GetRemainingSize() const
    {
        return GetCapacity() - GetUsedSize();
    }

    void PersistentBufferAllocator::SetName(StringView name)
    {
        m_buffer->SetName(name);
    }
}