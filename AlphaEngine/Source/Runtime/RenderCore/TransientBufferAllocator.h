#pragma once
#include "Core/Common.h"
#include "Core/Allocator/FreeListAllocator.h"
#include "RenderCore/Types.h"

namespace Alpha
{
	class Buffer;
	class RenderCore;
	// 一過性
	class TransientBufferAllocator
	{
	private:
		TUnique<Buffer> m_buffer{ nullptr };
		UInt8* m_map_ptr{ nullptr };
		UInt64 m_current_offset{ 0 };

	public:
		TransientBufferAllocator();
		~TransientBufferAllocator();

		struct Args
		{
			RenderCore* render_core{ nullptr };

			UInt64 stride{ 4 };
			UInt64 size{ 4 };

			UInt64 alignment{ 4 };
		};

		void Initialize(const Args& args);

		UInt64 WriteData(const void* data, UInt64 size);

		UInt8* Allocate(UInt64 size, UInt64 alignment, UInt64* out_offset);

		Buffer* GetBuffer() const;

		void Reset();
	};
}
/*
* TransientBufferAllocator
* 毎フレームリセットが呼ばれる
*/