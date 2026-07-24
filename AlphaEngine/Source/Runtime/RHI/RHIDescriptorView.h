#pragma once
#include "Declarations.h"

namespace Alpha
{
	// Vulkan用の時で書いてたけど使ってない

	class RHIDescriptorView
	{
	public:
		virtual ~RHIDescriptorView() {};

		virtual bool Initialize(const RHIDescriptorViewDesc& desc) = 0;
	};
}