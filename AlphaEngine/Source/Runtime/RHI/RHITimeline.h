#pragma once
#include "Core/Types.h"
#include "Declarations.h"

namespace Alpha
{
	class RHITimeline
	{
	public:
		virtual ~RHITimeline() {}

		virtual bool Initialize(const RHITimelineDesc& desc) = 0;

		// 
		virtual void WaitCPU(uint64_t wait_value) = 0;
	};

}
