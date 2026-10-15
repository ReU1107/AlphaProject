#pragma once
#include "Core/Common.h"
#include "Core/Allocator/FreeListAllocator.h"
#include "RenderCore/Types.h"
#include <algorithm>

namespace Alpha
{
	class Buffer;
	class RenderCore;
	// 一過性の毎フレーム更新されるものなのでマルチバッファ
	class TransientBufferAllocator
	{
		struct FrameBlock
		{
			TUnique<Buffer> buffer{ nullptr };
			UInt8* map_ptr{ nullptr };
			UInt64 current_offset{ 0 };
		};
	private:
		String m_name{};
		TArray<FrameBlock> m_blocks{};
		UInt64 m_alignment{ 4 };
		UInt64 m_used_size{ 0 };
		UInt64 m_capacity{ 0 };
		UInt32 m_current_block_index{ 0 };

	public:
		~TransientBufferAllocator();

		struct Args
		{
			RenderCore* render_core{ nullptr };	
			UInt32 block_num{ 3 };

			UInt64 stride{ 4 };
			UInt64 size{ 4 };

			UInt64 alignment{ 4 };
			EBufferPresetType type{ EBufferPresetType::Upload };
		};

		void Initialize(const Args& args);
		/*
		* WriteDataはBufferTypeがUpload
		*/
		template<typename T>
		UInt64 WriteData(const T* data)
		{
			UInt64 offset = 0;
			UInt64 size = sizeof(T);
			T* dest = reinterpret_cast<T*>(Allocate(size, &offset));
			std::memcpy(dest, data, size);
			return offset;
		}

		template<typename T>
		UInt64 WriteData(const T* data, UInt32 num)
		{
			UInt64 offset = 0;
			UInt64 size = sizeof(T) * num;
			T* dest = reinterpret_cast<T*>(Allocate(size, &offset));
			std::memcpy(dest, data, size);
			return offset;
		}

		template<typename T>
		UInt64 FillData(const T& data, UInt32 num)
		{
			UInt64 offset = 0;
			UInt64 size = sizeof(T) * num;
			T* dest = reinterpret_cast<T*>(Allocate(size, &offset));

			std::fill_n(dest, num, data);

			return offset;
		}

		UInt8* Allocate(UInt64 size, UInt64* out_offset);
		UInt8* GetMappedPtr();

		UInt64 AllocateOffset(UInt64 size);

		UInt32 GetAllocateCount() const;

		Buffer* GetBuffer() const;

		UInt32 GetSRVIndex() const;
		UInt32 GetUAVIndex() const;


		UInt64 GetCapacity() const;
		UInt64 GetUsedSize() const;
		UInt64 GetRemainingSize() const;

		void Reset(UInt32 current_buffer_index);

		void SetName(StringView name);

		const String& GetName() const { return m_name; }

	};
}
/*
* TransientBufferAllocator
* 毎フレームリセットが呼ばれる
*/