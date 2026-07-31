#pragma once
#include "Core/Common.h"
#include "Declarations.h"

namespace Alpha
{
	class RHIRayTracePipeline
	{
	public:
		virtual ~RHIRayTracePipeline() {}

		virtual bool Initialize(const RHIRayTracePipelineDesc& desc) = 0;

		virtual void* GetShaderIdentifier(const StringView export_nmae) const = 0;

	};

}