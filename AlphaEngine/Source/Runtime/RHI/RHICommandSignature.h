#pragma once
#include "Core/Types.h"
#include "Declarations.h"

namespace Alpha
{
	class RHICommandSignature
	{
	public:
		virtual ~RHICommandSignature() {}

		virtual bool Initialize(const RHICommandSignatureDesc& desc) = 0;

	};

}