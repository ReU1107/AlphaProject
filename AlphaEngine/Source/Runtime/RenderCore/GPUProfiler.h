#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class GPUQueryHeap;
	class CommandList;
	class TransientBufferAllocator;

	struct GPUProfileRecord
	{
		String name{};

		UInt32 begin_index{ MaxUInt32 };
		UInt32 end_index{ MaxUInt32 };

		double elapsed_ms{};
	};

	struct GPUProfileHandle
	{
		UInt32 record_index{ MaxUInt32 };
	};

	class GPUProfiler
	{
		struct FrameData
		{
			TArray<GPUProfileRecord> records;
			UInt32 query_count{ 0 };
			UInt64 readback_offset{ 0 };
			bool pending{ false };
		};

	private:
		TUnique<GPUQueryHeap> m_timestamp_heap{ nullptr };
		TUnique<TransientBufferAllocator> m_timestamp_buffer{ nullptr };
		TArray<FrameData> m_frames;
		TArray<GPUProfileRecord> m_collected_records{};

		UInt64 m_timestamp_frequency{ 0 };

		UInt32 m_current_buffer_index{ 0 };
		UInt32 m_query_index{ 0 };
		UInt32 m_query_capacity{ 0 };


	public:
		~GPUProfiler();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			UInt32 buffer_count{ 3 };
			UInt64 staging_buffer_size{ 64_KB };

			UInt64 timestamp_frequency{ 0 };
		};

		void Initialize(const Args& args);

		void Reset(UInt32 current_buffer_index);

		GPUProfileHandle Begin(StringView name, CommandList* command_list);

		void End(GPUProfileHandle handle, CommandList* command_list);

		void Flush(CommandList* command_list);

		const TArray<GPUProfileRecord>& GetCollectedRecords() const;

	private:
		void Collect();
	};
}