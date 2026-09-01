#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha
{
	class RenderCore;
	class CommandList;
	class TransientBufferAllocator;

	class ASBuilder
	{
	private:
		TUnique<TransientBufferAllocator> m_scratch_blas_allocator{ nullptr };
		TArray<BLASRequest> m_build_requests{};
		TArray<BLASRequest> m_update_requests{};

	public:
		~ASBuilder();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			UInt64 staging_buffer_size{ 64_MB };
		};

		void Initialize(const Args& args);
		void AddBuildRequest(const BLASRequest& request);
		void AddUpdateRequest(const BLASRequest& request);

		void RecordBuildCommands(CommandList* command_list);
		void RecordUpdateCommands(CommandList* command_list);

		void Reset(UInt32 current_buffer_index);

	private:
		void RecordCommands(CommandList* command_list, TArray<BLASRequest>& requests);

	};
}