#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RHIResource;

	class GPUResource
	{
	protected:
		TUnique<RHIResource> m_rhi{ nullptr };

	public:
		GPUResource();
		~GPUResource();

		RHIResource* GetRHI() const;

	};

}