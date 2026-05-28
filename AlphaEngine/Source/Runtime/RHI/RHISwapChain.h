#pragma once
#include "Core/Types.h"
#include "Declarations.h"

namespace Alpha
{
	class RHISwapChain
	{
	public:
		virtual ~RHISwapChain() {}

		virtual bool Initialize(const RHISwapChainDesc& desc) = 0;

		virtual void Present() = 0;

		virtual UInt32 GetCurrentBackBufferIndex() const = 0;

	};
}