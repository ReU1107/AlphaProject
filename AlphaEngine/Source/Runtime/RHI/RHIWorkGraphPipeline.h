#pragma once
#include "Declarations.h"

namespace Alpha
{
	class RHIWorkGraphPipeline
	{
	public:
		virtual ~RHIWorkGraphPipeline() {}

		virtual bool Initialize(const RHIWorkGraphPipelineDesc& desc) = 0;

	};
}