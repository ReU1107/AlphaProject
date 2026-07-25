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
        UInt64 size = num * m_alignment;
        Allocation alloc = m_allocator.Allocate(size, m_alignment);

        if (!alloc.is_valid)
            return false;

        *allocation = alloc;

        return true;
    }

    bool PersistentBufferAllocator::AllocateBySize(UInt32 size, Allocation* allocation)
    {
        Allocation alloc = m_allocator.Allocate(size, m_alignment);

        if (!alloc.is_valid)
            return false;

        *allocation = alloc;

        return true;
    }

    void PersistentBufferAllocator::Free(const Allocation& allocation)
    {
        m_allocator.Free(allocation);
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

    void PersistentBufferAllocator::SetName(StringView name)
    {
        m_buffer->SetName(name);
    }
}