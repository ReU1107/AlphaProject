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

        bool Allocate(UInt32 num, UInt64* out_offset);

        bool AllocateBySize(UInt32 size, UInt64* out_offset);

        Buffer* GetBuffer() const { return m_buffer.get(); }
    };
}
