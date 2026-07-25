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

		TUnique<TransientBufferAllocator> m_scratch_as_allocator{ nullptr };
		TArray<BLASRequest> m_requests{};

	public:
		~ASBuilder();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			UInt64 staging_buffer_size{ 64_MB };
		};

		void Initialize(const Args& args);
		void AddRequest(const BLASRequest& request);

		void RecordBuildCommands(CommandList* command_list);

	};
}