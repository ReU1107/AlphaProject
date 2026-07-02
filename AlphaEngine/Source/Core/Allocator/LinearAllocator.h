#pragma once
#include "Types.h"
#include "Math/MathUtil.h"

namespace Alpha
{
	class LinearAllocator
	{
	private:
		UInt64 m_max_size{ 0 };
		UInt64 m_current_offset{ 0 };

	public:
        void Initialize(UInt64 max_size) 
        {
            m_max_size = max_size;
            m_current_offset = 0;
        }

        // サイズとアライメントを指定して、切り出された「バイトオフセット」を返す
        bool Allocate(UInt64 size, UInt64 alignment, UInt64* out_offset) 
        {
            // アライメント調整（4バイトや256バイト境界に整列）
            UInt64 aligned_size = Alignment(size, alignment);
            UInt64 aligned_offset = Alignment(m_current_offset, alignment);

            // プールが満杯（枯渇）
            if (aligned_offset + aligned_size > m_max_size)
                return false;

            *out_offset = aligned_offset;
            m_current_offset = aligned_offset + aligned_size;
            
            return true;
        }

        void Reset() { m_current_offset = 0; }
        UInt64 GetCurrentOffset() const { return m_current_offset; }


	};
}