#pragma once
#include "Buffer.h"

namespace Alpha
{

	class RingBuffer : public Buffer
	{
	private:
		UInt8* m_map_ptr{ nullptr };
		UInt64 m_current_offset{ 0 };

	public:
		RingBuffer();
		~RingBuffer();

		struct Args
		{
			Buffer::Args base{};
			UInt64 alignment{ 256 };
		};

		void Initialize(const Args& args);

		UInt64 WriteData(const void* data, UInt64 size);

		UInt8* Allocate(UInt64 size, UInt64 alignment, UInt64* out_offset);

		void Reset();
	};

}