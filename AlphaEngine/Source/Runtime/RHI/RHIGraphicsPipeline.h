#pragma once
#include "Declarations.h"

namespace Alpha
{
	class RHIGraphicsPipeline
	{
	public:
		virtual ~RHIGraphicsPipeline() {}

		virtual bool Initialize(const RHIGraphicsPipelineDesc& desc) = 0;

	};
}