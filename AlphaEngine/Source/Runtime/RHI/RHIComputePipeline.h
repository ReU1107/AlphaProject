#pragma once
#include "Declarations.h"

namespace Alpha
{
	class RHIComputePipeline
	{
	public:
		virtual ~RHIComputePipeline() {}

		virtual bool Initialize(const RHIComputePipelineDesc& desc) = 0;
		
	};
}