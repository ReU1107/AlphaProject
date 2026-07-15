#pragma once
#include "Types.h"
#include "Allocator.h"
#include "Math/MathUtil.h"

#include <list>

namespace Alpha
{
    class FreeListAllocator
    {
    private:
        struct FreeBlock
        {
            UInt64 offset;
            UInt64 size;
        };

        std::list<FreeBlock> m_free_blocks{};
        UInt64               m_total_size{ 0 };

    public:
        bool Initialize(UInt64 total_size)
        {
            m_total_size = total_size;

            // 最初はプール全体が1つの空きブロック
            m_free_blocks.push_back({ 0, total_size });
            return true;
        }

        Allocation Allocate(UInt64 size, UInt64 alignment)
        {
            for (auto it = m_free_blocks.begin(); it != m_free_blocks.end(); ++it)
            {
                UInt64 aligned_offset = Alignment(it->offset, alignment);
                UInt64 padding = aligned_offset - it->offset;

                if (it->size >= size + padding)
                {
                    Allocation alloc{ aligned_offset, size, true };

                    UInt64 used_size = size + padding;
                    if (it->size > used_size)
                    {
                        it->offset += used_size;
                        it->size -= used_size;
                    }
                    else
                    {
                        m_free_blocks.erase(it);
                    }
                    return alloc;
                }
            }
            return Allocation{}; // メモリ不足
        }

        void Free(const Allocation& alloc)
        {
            if (!alloc.is_valid) return;

            FreeBlock new_block = { alloc.offset, alloc.size };

            auto it = std::upper_bound(m_free_blocks.begin(), m_free_blocks.end(), new_block.offset,
                [](UInt64 offset, const FreeBlock& block) { return offset < block.offset; });

            auto inserted = m_free_blocks.insert(it, new_block);

            // 後ろのブロックと合体
            auto next = std::next(inserted);
            if (next != m_free_blocks.end() && (inserted->offset + inserted->size == next->offset))
            {
                inserted->size += next->size;
                m_free_blocks.erase(next);
            }

            // 前のブロックと合体
            if (inserted != m_free_blocks.begin())
            {
                auto prev = std::prev(inserted);
                if (prev->offset + prev->size == inserted->offset)
                {
                    prev->size += inserted->size;
                    m_free_blocks.erase(inserted);
                }
            }
        }
    };

}