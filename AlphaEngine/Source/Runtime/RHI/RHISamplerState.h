#pragma once
#include "Declarations.h"

namespace Alpha
{
	class RHISamplerState
	{
	public:
		virtual ~RHISamplerState() {}

		virtual bool Initialize(const RHISamplerStateDesc& desc) = 0;
	};
}