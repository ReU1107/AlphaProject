#include "GPUProfiler.h"
#include "CommandList.h"
#include "RenderCore.h"
#include "GPUQueryHeap.h"
#include "TransientBufferAllocator.h"

#include "Util.h"


namespace Alpha
{
	GPUProfiler::~GPUProfiler() = default;

	void GPUProfiler::Initialize(const Args& args)
	{
		m_timestamp_buffer = CreateTransientAllocator(args.render_core, sizeof(UInt64), 1024, EBufferPresetType::Readback, L"TimestampBuffer");

		m_timestamp_heap = args.render_core->CreateQueryHeap(128, ERHIQueryType::Timestamp);
		m_frames.resize(3);

		m_timestamp_frequency = args.timestamp_frequency;
	}

	void GPUProfiler::Reset(UInt32 current_buffer_index)
	{
		m_current_buffer_index = current_buffer_index;

		m_timestamp_buffer->Reset(current_buffer_index);
		
		// このbackbufferが3なら2フレーム前のデータを収集
		Collect();
		
		m_query_index = 0;

		auto& frame = m_frames[current_buffer_index];
		frame.records.clear();
		frame.query_count = 0;
		frame.readback_offset = 0;
		frame.pending = false;
	}

	GPUProfileHandle GPUProfiler::Begin(StringView name, CommandList* command_list)
	{
		auto& frame = m_frames[m_current_buffer_index];

		const UInt32 record_index = static_cast<UInt32>(frame.records.size());

		GPUProfileRecord& record = frame.records.emplace_back();
		record.name = name;
		record.begin_index = m_query_index++;
		record.end_index = MaxUInt32;
		record.elapsed_ms = 0.0;

		command_list->BeginEvent(name, m_timestamp_heap.get(), record.begin_index);

		return GPUProfileHandle{
			.record_index = record_index
		};
	}

	void GPUProfiler::End(GPUProfileHandle handle, CommandList* command_list)
	{
		auto& frame = m_frames[m_current_buffer_index];

		GPUProfileRecord& record = frame.records[handle.record_index];

		record.end_index = m_query_index++;

		command_list->EndEvent(record.name, m_timestamp_heap.get(), record.end_index);
	}

	void GPUProfiler::Flush(CommandList* command_list)
	{
		auto& frame = m_frames[m_current_buffer_index];

		if (m_query_index == 0)
			return;

		for (const auto& record : frame.records)
		{
			ALPHA_CHECK_ERROR(record.begin_index != MaxUInt32, L"");
			ALPHA_CHECK_ERROR(record.end_index != MaxUInt32, L"");
		}

		frame.query_count = m_query_index;

		const UInt64 resolve_size = static_cast<UInt64>(frame.query_count) * sizeof(UInt64);

		frame.readback_offset = m_timestamp_buffer->AllocateOffset(resolve_size);


		command_list->ResolveEvent(m_timestamp_heap.get(), 0, m_query_index, m_timestamp_buffer->GetBuffer(), frame.readback_offset);

		frame.pending = true;

	}

	const TArray<GPUProfileRecord>& GPUProfiler::GetCollectedRecords() const
	{
		return m_collected_records;
	}

	void GPUProfiler::Collect()
	{
		auto& frame = m_frames[m_current_buffer_index];

		if (!frame.pending)
			return;

		const UInt8* mapped_ptr = m_timestamp_buffer->GetMappedPtr();

		const UInt64* timestamps = reinterpret_cast<const UInt64*>(mapped_ptr + frame.readback_offset);

		for (auto& record : frame.records)
		{
			const UInt64 start = timestamps[record.begin_index];
			const UInt64 end = timestamps[record.end_index];

			const UInt64 elapsed_ticks = end - start;

			record.elapsed_ms =
				static_cast<double>(elapsed_ticks) /
				static_cast<double>(m_timestamp_frequency) * 1000.0;
		}

		m_collected_records = frame.records;

		frame.pending = false;
	}

}
