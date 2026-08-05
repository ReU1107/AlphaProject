#pragma once
#include "Declarations.h"

namespace Alpha
{
	class RHIShader
	{
	public:
		virtual ~RHIShader() {}

		virtual bool Initialize(const RHIShaderDesc& desc) = 0;

	};
}