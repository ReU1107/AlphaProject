#pragma once
#include "Core/Types.h"
#include "Declarations.h"

namespace Alpha
{
	class RHICommandLayout
	{
	public:
		virtual ~RHICommandLayout() {}

		virtual bool Initialize(const RHICommandLayoutDesc& desc) = 0;

	};

}