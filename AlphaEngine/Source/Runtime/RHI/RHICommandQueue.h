#pragma once
#include "Core/Common.h"
#include "Declarations.h"
#include <span>

namespace Alpha
{
	class RHICommandQueue
	{
	public:
		virtual ~RHICommandQueue() {}

		virtual bool Initialize(const RHICommandQueueDesc& desc) = 0;

		virtual void ExecuteCommandBuffer(RHICommandBuffer* command_buffer) = 0;

		virtual void ExecuteCommandBuffers(std::span<RHICommandBuffer*> command_buffers) = 0;

		virtual void Signal(RHITimeline* timeline_, uint64_t value) = 0;

		virtual void Wait(RHITimeline* timeline_, uint64_t value) = 0;

		virtual void SetName(StringView name) = 0;
	};

}