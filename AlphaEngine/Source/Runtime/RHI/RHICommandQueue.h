#pragma once
#include "Core/Common.h"
#include "Declarations.h"

namespace Alpha
{
	class RHICommandQueue
	{
	public:
		virtual ~RHICommandQueue() {}

		virtual bool Initialize(const RHICommandQueueDesc& desc) = 0;

		virtual void ExecuteCommandBuffer(RHICommandBuffer* command_buffer_) = 0;

		virtual void ExecuteCommandBuffers(RHICommandBuffer** command_buffers_, UInt32 command_buffer_count_) = 0;

		virtual void Signal(RHITimeline* timeline_, uint64_t value) = 0;

		virtual void Wait(RHITimeline* timeline_, uint64_t value) = 0;

		virtual void SetName(StringView name) = 0;
	};

}