#pragma once
#include "Declarations.h"

namespace Alpha
{
	class RHIDescriptorView
	{
	public:
		virtual ~RHIDescriptorView() {};

		virtual bool Initialize(const RHIDescriptorViewDesc& desc) = 0;
	};
}