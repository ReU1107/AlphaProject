#pragma once
#include "Core/Common.h"
#include "Core/Allocator/FreeListAllocator.h"
#include "RenderCore/Types.h"

namespace Alpha
{
    class RenderCore;
	class Buffer;

    class PersistentBufferAllocator
    {
    private:
        TUnique<Buffer> m_buffer{ nullptr };
        FreeListAllocator m_allocator{};
        UInt64 m_alignment{ 0 };

    public:
        ~PersistentBufferAllocator();

        struct Args
        {
            RenderCore* render_core{ nullptr };
            UInt64 alignment{ 4 };
            UInt64 max_size{};
            EBufferPresetType preset{ EBufferPresetType::ROByteAddress };
        };

        bool Initialize(const Args& args);

        bool AllocateByNum(UInt32 num, Allocation* allocation);

        bool AllocateBySize(UInt32 size, Allocation* allocation);

        void Free(const Allocation& allocation);

        Buffer* GetBuffer() const { return m_buffer.get(); }
    };
}
